typedef func OnLocalPlayerChangedGroup;
void OnLocalPlayerChangedGroup(int groupID);

modded class SCR_PlayerControllerGroupComponent
{
	ref ScriptInvokerBase<OnLocalPlayerChangedGroup> m_OnLocalPlayerChangedGroup = new ScriptInvokerBase<OnLocalPlayerChangedGroup>();
	
	//------------------------------------------------------------------------------------------------
	// Note that it will only let you delete the group if you are the group leader
	void RequestDeleteGroup(int groupID)
	{
		Rpc(RPC_AskDeleteGroup, groupID);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RPC_AskDeleteGroup(int groupID)
	{
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		if (!groupsManager)
			return;
		
		SCR_AIGroup group = groupsManager.FindGroup(groupID);
		if (!group)
			return;
		
		// Are you even the leader of the group?
		int groupLeaderId = group.GetLeaderID();
		int myId = PlayerController.Cast(GetOwner()).GetPlayerId();
		if (myId != groupLeaderId)
			return;
		
		groupsManager.DeleteGroupRemovePlayersDelayed(group);
	}
	
	//------------------------------------------------------------------------------------------------
	// Creates a group and sets its name in one request
	void RequestCreateGroupEx(string groupName, bool privateGroup)
	{
		Rpc(RPC_AskCreateGroupEx, groupName, privateGroup);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RPC_AskCreateGroupEx(string groupName, bool privateGroup)
	{
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		if (!groupsManager)
			return;
		
		SCR_RespawnSystemComponent respawnSystemComponent = SCR_RespawnSystemComponent.GetInstance();
		if (!respawnSystemComponent)
			return;
		
		Faction faction = respawnSystemComponent.GetPlayerFaction(GetPlayerID());
		if (!faction)
			return;
		
		// I don't think we need it in our mod?!
		// We check if there is any empty group already for our faction
		//if (groupsManager.TryFindEmptyGroup(faction))
		//	return;
		
		// No empty group found, we allow creation of new group
		SCR_AIGroup newGroup = groupsManager.CreateNewPlayableGroup(faction);
		
		// No new group was created, return
		if (!newGroup)
			return;
		
		// Set name
		PlayerController pc = PlayerController.Cast(GetOwner());
		int myPlayerId = pc.GetPlayerId();
		newGroup.SetCustomName(groupName, myPlayerId);
		
		// New group sucessfully created
		// The player should be automatically added/moved to it
		RPC_AskJoinGroup(newGroup.GetGroupID());
		
		// Make group private
		newGroup.SetPrivate(privateGroup);
	}
	
	
	//------------------------------------------------------------------------------------------------
	void RequestLeaveGroup()
	{
		Rpc(RPC_AskLeaveGroup);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RPC_AskLeaveGroup()
	{
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		PlayerController pc = PlayerController.Cast(GetOwner());
		groupsManager.RemovePlayerFromGroup(pc.GetPlayerId());
		
		RPC_DoChangeGroupID(-1);
		Rpc(RPC_DoChangeGroupID, -1);
	}
	
	//------------------------------------------------------------------------------------------------
	// Overriden to disable auto creation of group for the kicked player
	override void RPC_AskKickPlayer(int playerID)
	{
		SCR_GroupsManagerComponent groupsManager;
		SCR_PlayerControllerGroupComponent playerGroupController;
		SCR_AIGroup group;
		if (!InitiateComponents(playerID, groupsManager, playerGroupController, group))
			return;
		
		//requesting player is not leader of the targets group, do nothing
		if (!group.IsPlayerLeader(GetPlayerID()))
			return;
		
		groupsManager.RemovePlayerFromGroup(playerID);
		
		//SCR_AIGroup newGroup = groupsManager.GetFirstNotFullForFaction(group.GetFaction(), group, true);
		//if (!newGroup)
		//	newGroup = groupsManager.CreateNewPlayableGroup(group.GetFaction());
				
		//if (!newGroup)
		//	return;
		//playerGroupController.RequestJoinGroup(newGroup.GetGroupID());
	}
	
	//------------------------------------------------------------------------------------------------
	override void RPC_DoChangeGroupID(int groupID)
	{
		super.RPC_DoChangeGroupID(groupID);
		m_OnLocalPlayerChangedGroup.Invoke(groupID);
	}
	
	
	//------------------------------------------------------------------------------------------------
	static SCR_PlayerControllerGroupComponent GetLocalInstance()
	{
		return GetLocalPlayerControllerGroupComponent(); // I could break my keyboard trying to write this
	}
	
	
	//------------------------------------------------------------------------------------------------
	// Related to radio
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		SCR_PlayerController pc = SCR_PlayerController.Cast(owner);
		pc.m_OnControlledEntityChanged2.Insert(OnControlledEntityChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnControlledEntityChanged(IEntity oldEntity, IEntity newEntity)
	{
		//PrintFormat("Changed from %1 to %2", oldEntity, newEntity);
		
		if (newEntity && Replication.IsServer())
			TuneRadio(newEntity);
	}

	//------------------------------------------------------------------------------------------------
	// Tunes both squad radio and command radio
	void TuneRadio(notnull IEntity entity)
	{
		BaseRadioComponent squadRadio = FindRadio(entity, EGadgetType.RADIO);
		
		if (!squadRadio)
			Print(string.Format("Could not find squad radio for: %1, %2", entity, entity.GetPrefabData().GetPrefabName()), LogLevel.ERROR);
		
		BaseRadioComponent commanderRadio = FindRadio(entity, EGadgetType.RADIO_BACKPACK);
		
		PlayerController pc = PlayerController.Cast(GetOwner());
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		Faction myFaction = SCR_RespawnSystemComponent.GetInstance().GetPlayerFaction(pc.GetPlayerId());
		
		if (!myFaction)
			return; // wtf get a faction
		
		// Squad radio - frequency depends on group
		SCR_AIGroup myGroup = groupsManager.GetPlayerGroup(pc.GetPlayerId());
		int squadRadioFrequency;
		if (myGroup)
			squadRadioFrequency = myGroup.GetRadioFrequency();							// In a group
		else
			squadRadioFrequency = groupsManager.GetSquadlessRadioFrequency(myFaction);	// Not in a group
		
		
		TuneRadioTransceivers(squadRadio, true, squadRadioFrequency);
		
		// Commander radio - frequency depends on faction
		if (commanderRadio)
		{
			int commanderRadioFrequency = groupsManager.GetCommanderRadioFrequency(myFaction);
			
			TuneRadioTransceivers(commanderRadio, true, commanderRadioFrequency);
		}
	}
	
	// Sets power and frequency on all transceivers of radio
	private void TuneRadioTransceivers(BaseRadioComponent radio, bool power, int freq)
	{
		radio.SetPower(power);
		for (int i = 0; i < radio.TransceiversCount(); i++)
		{
			BaseTransceiver trx = radio.GetTransceiver(i);
			
			if (!trx)
				continue;
			
			// Transceiver 0 is set to proper frequency, and we don't care about second one, it must be disabled
			if (i == 0)
				trx.SetFrequency(freq);
			else
				trx.SetFrequency(trx.GetMinFrequency());
		}
	}
	
	//------------------------------------------------------------------------------------------------
	private BaseRadioComponent FindRadio(notnull IEntity controlledEntity, EGadgetType gadgetType)
	{
		SCR_GadgetManagerComponent gagdetManager = SCR_GadgetManagerComponent.Cast(controlledEntity.FindComponent(SCR_GadgetManagerComponent));
		if (!gagdetManager)
			return null;
		
		IEntity radio = gagdetManager.GetGadgetByType(gadgetType);
		if (!radio)
			return null;
		
		return BaseRadioComponent.Cast(radio.FindComponent(BaseRadioComponent));
	}
}