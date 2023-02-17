modded class SCR_PlayerControllerGroupComponent
{
	protected IEntity m_PrevControlledEntity;
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		SCR_PlayerController pc = SCR_PlayerController.Cast(owner);
		pc.m_OnControlledEntityChanged2.Insert(OnControlledEntityChanged);
	}
	
	void OnControlledEntityChanged(IEntity oldEntity, IEntity newEntity)
	{
		PrintFormat("Changed from %1 to %2", oldEntity, newEntity);
		
		if (newEntity && Replication.IsServer())
			TuneRadio(newEntity);
	}

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
		
		squadRadio.SetFrequency(squadRadioFrequency);
		
		// Commander radio - frequency depends on faction
		if (commanderRadio)
		{
			int commanderRadioFrequency = groupsManager.GetCommanderRadioFrequency(myFaction);
			commanderRadio.SetFrequency(commanderRadioFrequency);
		}
	}
	
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