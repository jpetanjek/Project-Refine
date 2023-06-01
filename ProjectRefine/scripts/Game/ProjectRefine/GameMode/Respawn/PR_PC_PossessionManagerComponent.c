typedef func OnPlayerPossessionEntityChangedDelegate;
void OnPlayerPossessionEntityChangedDelegate(bool isMain, PR_PC_PossessionManagerComponent possessionComponent);

enum PR_EPossessionState
{
	NONE,	// Possessing nothing
	DUMMY,	// Possessing dummy character
	MAIN	// Possessing main character
}

[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "Class for managing possession in PR game modes")]
class PR_PC_PossessionManagerComponentClass : ScriptComponentClass
{
};

//! Temporary class for managing of dummy possession for radio and map markers to work
class PR_PC_PossessionManagerComponent : ScriptComponent
{
	protected const ResourceName DUMMY_CHARACTER_PREFAB = "{A86AE3BF03958A94}Prefabs/Characters/PR_Character_Dummy.et";
	
	protected SCR_PlayerController m_PlayerController;
	
	protected IEntity m_DummyEntity;
	
	protected bool m_bFactionChanged; // Synchronized from event
	
	//------------------------------------------------------------------------------------------------
	// The old possession event, do we even need it any more?
	
	//protected ref ScriptInvokerBase<OnPlayerPossessionEntityChangedDelegate> m_OnPlayerPossessionEntityChangedDelegate = new ScriptInvokerBase<OnPlayerPossessionEntityChangedDelegate>();
	
	//ScriptInvokerBase<OnPlayerPossessionEntityChangedDelegate> GetOnPlayerPossessionEntityChangedDelegate()
	//{
	//	return m_OnPlayerPossessionEntityChangedDelegate;
	//}
	
	// Client only
	//protected void PossessionChanged()
	//{
	//	m_OnPlayerPossessionEntityChangedDelegate.Invoke(m_MainPossessed, this);
	//}
	//------------------------------------------------------------------------------------------------

	
	
	//------------------------------------------------------------------------------------------------
	static PR_PC_PossessionManagerComponent GetPlayerInstance(int playerId)
	{
		SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if(pc)
			return PR_PC_PossessionManagerComponent.Cast(pc.FindComponent(PR_PC_PossessionManagerComponent));
		else
			return null;
	}
	
	//------------------------------------------------------------------------------------------------
	static PR_PC_PossessionManagerComponent GetLocalInstance()
	{
		SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if(pc)
			return PR_PC_PossessionManagerComponent.Cast(pc.FindComponent(PR_PC_PossessionManagerComponent));
		else
			return null;
	}
	
	//------------------------------------------------------------------------------------------------
	PR_EPossessionState GetState()
	{
		IEntity controlledEntity = m_PlayerController.GetControlledEntity();
		if (!controlledEntity)
			controlledEntity = m_PlayerController.GetMainEntity();
		
		if (!controlledEntity)
			return PR_EPossessionState.NONE;
		
		ResourceName prefabName = controlledEntity.GetPrefabData().GetPrefabName();
		if (prefabName == DUMMY_CHARACTER_PREFAB)
			return PR_EPossessionState.DUMMY;
		else
		{
			// Check damage mgr state, because even when possessed entity is killed, GetControlledEntity returns it.
			DamageManagerComponent damageMgr = DamageManagerComponent.Cast(controlledEntity.FindComponent(DamageManagerComponent));
			if (damageMgr.IsDestroyed())
				return PR_EPossessionState.NONE;
			else
				return PR_EPossessionState.MAIN;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		m_PlayerController = SCR_PlayerController.Cast(owner);	
		
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	//------------------------------------------------------------------------------------------------
	protected override void EOnInit(IEntity owner)
	{
		PR_FactionMemberManager factionMemberManager = PR_FactionMemberManager.GetInstance();
		if(factionMemberManager)
			factionMemberManager.GetOnPlayerChangedFaction().Insert(OnPlayerChangedFaction);	
	}
	
	//------------------------------------------------------------------------------------------------
	// Called by PR_GM_PossessingManagerComponent
	// Sadly EOnFrame doesn't work on server for player-owned player controllers :( thus we update it from elsewhere
	void EOnFrameServer(IEntity owner, float timeSlice)
	{
		if (m_bFactionChanged)
		{
			// Clean up previous dummy entity if it existed
			if (m_DummyEntity)
			{
				_print("Faction has changed, deleting dummy entity");
				DeleteDummyEntity();
			}
			m_bFactionChanged = false;
		}
		
		int factionId = PR_FactionMemberManager.GetInstance().GetPlayerFactionIndex(m_PlayerController.GetPlayerId());
		
		if (factionId == -1 && m_DummyEntity)
		{
			// We don't have a faction
			// Delete dummy if we have it
			DeleteDummyEntity();
		}
		else if (factionId != -1)
		{
			// We have a faction
			// What to do depends on state
			
			PR_EPossessionState state = GetState();
			
			if (state == PR_EPossessionState.NONE)
			{
				// Create dummy if we don't have it, possess it
				PossessDummyEntity(SpawnDummyEntity());
			}
			else if (state == PR_EPossessionState.MAIN && m_DummyEntity)
			{
				// Clean up dummy if we are back at main character
				// This is needed because when we switch to Editor UI, a dummy is created again,
				// So when we switch back, we should clean it up
				DeleteDummyEntity();
			}
			
			// If we are in DUMMY or MAIN state, don't do anything
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ~PR_PC_PossessionManagerComponent()
	{
		PR_FactionMemberManager factionMemberManager = PR_FactionMemberManager.GetInstance();
		if(factionMemberManager)
			factionMemberManager.GetOnPlayerChangedFaction().Remove(OnPlayerChangedFaction);	
		
		_print("~PR_PC_PossessionManagerComponent()");
		if (m_DummyEntity)
			DeleteDummyEntity();
	}
	
	
	//------------------------------------------------------------------------------------------------
	protected void OnPlayerChangedFaction(int playerID, int newFactionIdx)
	{
		if(m_PlayerController.GetPlayerId() != playerID)
			return;
		
		// Hell knows if the event is called from RPC or something like that,
		// better run logic in next EOnFrame
		m_bFactionChanged = true;
	}
	
	//------------------------------------------------------------------------------------------------
	void PossessMainEntity(notnull IEntity mainEntity)
	{
		_print("PossessMainEntity()");
		
		// First possess the new entity, then delete the dummy entity
		// !!! This must happen exactly in this order, otherwise on deletion of dummy entity
		// Editor UI will open itself.
		// This is triggered by GameMode.OnControllableDeleted and whatever is subscribed to it.
		
		m_PlayerController.SetPossessedEntity(mainEntity);
		
		DeleteDummyEntity();
		
		PR_EPossessionState state = GetState();
		_print(string.Format("...new state: %1", typename.EnumToString(PR_EPossessionState, state)));
	}
	
	
	
	//------------------------------------------------------------------------------------------------
	// Dummy Entity
	
	//------------------------------------------------------------------------------------------------
	protected void PossessDummyEntity(notnull IEntity dummyEntity)
	{
		_print("PossessDummyEntity()");
		
		m_DummyEntity = dummyEntity;
		
		m_PlayerController.SetPossessedEntity(dummyEntity);
	}
	
	protected IEntity SpawnDummyEntity()
	{
		_print("SpawnDummyEntity()");
		
		
		IEntity ent = SCR_RespawnSystemComponent.GetInstance().DoSpawn(DUMMY_CHARACTER_PREFAB, vector.Zero);
		
		// Verify that prefab name matches,
		// If not then the whole logic fails and we will end up with entities being infinitely created
		ResourceName spawnedResourceName = ent.GetPrefabData().GetPrefabName();
		if (spawnedResourceName != DUMMY_CHARACTER_PREFAB)
		{
			_print(string.Format("Created dummy character has wrong prefab name! %1 != %2", spawnedResourceName, DUMMY_CHARACTER_PREFAB), LogLevel.ERROR);
			RplComponent.DeleteRplEntity(ent, false);
			return null;
		}
		
		return ent;
	}
	
	protected void DeleteDummyEntity()
	{
		_print("DeleteDummyEntity()");
		
		if (m_DummyEntity)
		{
			RplComponent.DeleteRplEntity(m_DummyEntity, false);
			_print("...Deleted");
		}
		else
		{
			_print("...Nothing to delete");
		}
	}
	
	
	//------------------------------------------------------------------------------------------------
	// Utils
	
	protected void _print(string str, LogLevel logLevel = LogLevel.NORMAL)
	{
		int id = -1;
		if (m_PlayerController)
			id = m_PlayerController.GetPlayerId();
		
		Print(string.Format("[PR_PC_PossessionManagerComponent ID: %1]: %2", id, str), logLevel);
	}
}