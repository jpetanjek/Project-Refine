typedef func OnPlayerPossessionEntityChangedDelegate;
void OnPlayerPossessionEntityChangedDelegate(bool isMain, PR_PC_PossessionManagerComponent possessionComponent);

[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "Class for managing possession in PR game modes")]
class PR_PC_PossessionManagerComponentClass : ScriptComponentClass
{
};

//! Temporary class for managing of dummy possession for radio and map markers to work
class PR_PC_PossessionManagerComponent : ScriptComponent
{
	private SCR_PlayerController m_playerController;
	
	private GenericEntity m_MainPossessionEntity;
	private GenericEntity m_DummyPossessionEntity;
	
	//------------------------------------------------------------------------------------------------
	protected ref ScriptInvokerBase<OnPlayerPossessionEntityChangedDelegate> m_OnPlayerPossessionEntityChangedDelegate = new ScriptInvokerBase<OnPlayerPossessionEntityChangedDelegate>();
	
	ScriptInvokerBase<OnPlayerPossessionEntityChangedDelegate> GetOnPlayerPossessionEntityChangedDelegate()
	{
		return m_OnPlayerPossessionEntityChangedDelegate;
	}
	
	//------------------------------------------------------------------------------------------------
	[RplProp(onRplName: "PossessionChanged")]
	bool m_MainPossessed = false;
	
	// Client only
	protected void PossessionChanged()
	{
		m_OnPlayerPossessionEntityChangedDelegate.Invoke(m_MainPossessed, this);
	}

	//------------------------------------------------------------------------------------------------
	static PR_PC_PossessionManagerComponent GetPlayerInstance(int playerId)
	{
		SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if(pc)
			return PR_PC_PossessionManagerComponent.Cast(pc.FindComponent(PR_PC_PossessionManagerComponent));
		else
			return null;
	}
	
	static PR_PC_PossessionManagerComponent GetLocalInstance()
	{
		SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if(pc)
			return PR_PC_PossessionManagerComponent.Cast(pc.FindComponent(PR_PC_PossessionManagerComponent));
		else
			return null;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	protected override void EOnInit(IEntity owner)
	{
		m_playerController = SCR_PlayerController.Cast(owner);		
		
		if(Replication.IsClient())
			m_playerController.m_OnControlledEntityChanged.Insert(OnControlledEntityChangedServer);
		
		PR_FactionMemberManager factionMemberManager = PR_FactionMemberManager.GetInstance();
		if(factionMemberManager)
			factionMemberManager.GetOnPlayerChangedFaction().Insert(OnPlayerChangedFaction);	
	}
	
	void ~PR_PC_PossessionManagerComponent()
	{
		PR_FactionMemberManager factionMemberManager = PR_FactionMemberManager.GetInstance();
		if(factionMemberManager)
			factionMemberManager.GetOnPlayerChangedFaction().Remove(OnPlayerChangedFaction);	
	}
	
	//------------------------------------------------------------------------------------------------
	void OnPlayerChangedFaction(int playerID, int newFactionIdx)
	{
		if(m_playerController.GetPlayerId() != playerID)
			return;
		
		if(newFactionIdx == -1 && m_DummyPossessionEntity)
		{
			RplComponent.DeleteRplEntity(m_DummyPossessionEntity, false);
		}

		if(newFactionIdx != -1 && !m_MainPossessed)
		{
			PossessDummyEntity(SpawnDummy());
		}
	}
	
	protected void OnControlledEntityChangedServer(IEntity from, IEntity to)
	{
		// Client only
		if(!to)
			Rpc(RpcCompletelyUnnecessaryRpc);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcCompletelyUnnecessaryRpc()
	{
		PossessDummyEntity(SpawnDummy());
	}
	
	GenericEntity SpawnDummy()
	{
		if(Replication.IsClient())
			return null;
		
		return SCR_RespawnSystemComponent.GetInstance().DoSpawn("{D864A7C6EF92C953}Prefabs/Characters/Factions/BLUFOR/US_Army/PR_Character_US_Base.et","0 0 0");
	}
	
	void PossessMainEntity(GenericEntity mainEntity)
	{
		if(Replication.IsClient() || !mainEntity)
			return;
		
		Print("PossessMainEntity");
		
		m_MainPossessionEntity = mainEntity;
		// Delete dummy
		RplComponent.DeleteRplEntity(m_DummyPossessionEntity, false);
		
		m_playerController.SetPossessedEntity(mainEntity);
		
		m_MainPossessed = true;
		Replication.BumpMe();
		
		m_OnPlayerPossessionEntityChangedDelegate.Invoke(m_MainPossessed, this);
	}
	
	void PossessDummyEntity(GenericEntity dummyEntity)
	{
		if(Replication.IsClient() || !dummyEntity)
			return;
		
		Print("PossessDummyEntity");
		
		m_DummyPossessionEntity = dummyEntity;
		// Dereference only, we want it to stay so it can be looted, garbage manager will take care of deletion
		m_MainPossessionEntity = null;
		
		m_playerController.SetPossessedEntity(dummyEntity);
		
		m_MainPossessed = false;
		Replication.BumpMe();
		
		m_OnPlayerPossessionEntityChangedDelegate.Invoke(m_MainPossessed, this);
	}
	
}