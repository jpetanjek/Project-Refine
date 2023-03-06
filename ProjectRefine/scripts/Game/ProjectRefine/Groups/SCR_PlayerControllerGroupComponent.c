typedef func OnLocalPlayerJoinedGroup;
void OnLocalPlayerJoinedGroup(int groupID);

modded class SCR_PlayerControllerGroupComponent
{
	ref ScriptInvokerBase<OnLocalPlayerJoinedGroup> m_OnLocalPlayerJoinedGroup = new ScriptInvokerBase<OnLocalPlayerJoinedGroup>();
	
	//------------------------------------------------------------------------------------------------
	// Creates a group and sets its name in one request
	void RequestCreateGroupWithName(string groupName)
	{
		Rpc(RPC_AskCreateGroupWithName, groupName);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RPC_AskCreateGroupWithName(string groupName)
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
	override void RPC_DoChangeGroupID(int groupID)
	{
		super.RPC_DoChangeGroupID(groupID);
		m_OnLocalPlayerJoinedGroup.Invoke(groupID);
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
			squadRadioFrequency = myGroup.GetGroupFrequency();							// In a group
		else
			squadRadioFrequency = groupsManager.GetSquadlessRadioFrequency(myFaction);	// Not in a group
		
		squadRadio.TogglePower(true);
		squadRadio.SetFrequency(squadRadioFrequency);
		
		// Commander radio - frequency depends on faction
		if (commanderRadio)
		{
			int commanderRadioFrequency = groupsManager.GetCommanderRadioFrequency(myFaction);
			commanderRadio.TogglePower(true);
			commanderRadio.SetFrequency(commanderRadioFrequency);
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