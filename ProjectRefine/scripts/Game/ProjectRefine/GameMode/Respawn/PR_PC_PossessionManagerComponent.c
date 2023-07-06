typedef func OnPlayerPossessionEntityChangedDelegate;
void OnPlayerPossessionEntityChangedDelegate(bool isMain, PR_PC_PossessionManagerComponent possessionComponent);

enum PR_EPossessionState
{
	NO_FACTION,	// No faction is selected, the rest is unknown
	NONE,		// Possessing nothing
	DUMMY,		// Possessing dummy character
	MAIN,		// Possessing main character
	EDITOR		// We are in Editor UI
}

[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "Class for managing possession in PR game modes")]
class PR_PC_PossessionManagerComponentClass : ScriptComponentClass
{
};

//! Temporary class for managing of dummy possession for radio and map markers to work
class PR_PC_PossessionManagerComponent : ScriptComponent
{	
	protected SCR_PlayerController m_PlayerController;
	
	protected IEntity m_DummyEntity;
	
	protected SCR_EditorManagerCore m_EditorCore;
	
	protected bool m_bFactionChanged = false; // Synchronized from event
	
	protected bool m_bGroupLeadChanged = false; // Our state of being a group leader has changed
	
	protected int m_iLastGroupIWasLeadIn = -1;

	
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
		// In editor?
		SCR_EditorManagerEntity editorManager = m_EditorCore.GetEditorManager(m_PlayerController.GetPlayerId());
		if (editorManager && (editorManager.IsOpened() || editorManager.IsInTransition()))
			return PR_EPossessionState.EDITOR;
		
		// No faction?
		SCR_Faction playerFaction = SCR_Faction.Cast(PR_FactionMemberManager.GetInstance().GetPlayerFaction(m_PlayerController.GetPlayerId()));
		
		if (!playerFaction)
			return PR_EPossessionState.NO_FACTION;
		
		IEntity controlledEntity = m_PlayerController.GetControlledEntity();
		if (!controlledEntity)
			controlledEntity = m_PlayerController.GetMainEntity();
		
		// No controlled entity?
		if (!controlledEntity)
			return PR_EPossessionState.NONE;
		
		ResourceName prefabName = controlledEntity.GetPrefabData().GetPrefabName();
		
		// Controlling dummy?
		if (prefabName == playerFaction.GetDummyPrefab() || prefabName == playerFaction.GetDummyRadioPrefab())
			return PR_EPossessionState.DUMMY;
		else
		{
			// Controlled entity is destroyed?
			
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
		
		SCR_GroupsManagerComponent m_GroupsMgr = SCR_GroupsManagerComponent.GetInstance();
				
		if (m_GroupsMgr)
		{
			m_GroupsMgr.GetOnPlayableGroupCreated().Insert(Event_OnPlayableGroupCreated);
			m_GroupsMgr.GetOnPlayableGroupRemoved().Insert(Event_OnPlayableGroupRemoved);
			
			array<SCR_AIGroup> groups = {};
			m_GroupsMgr.GetAllPlayableGroups(groups);
			for(int i = 0; i < groups.Count(); i++)
			{
				groups[i].GetOnPlayerLeaderChanged().Insert(OnPlayerLeaderChanged);
			}
		}
		
		m_EditorCore = SCR_EditorManagerCore.Cast(SCR_EditorManagerCore.GetInstance(SCR_EditorManagerCore));
	}
	
	void Event_OnPlayableGroupCreated(SCR_AIGroup group)
	{
		group.GetOnPlayerLeaderChanged().Insert(OnPlayerLeaderChanged);
	}
	
	void Event_OnPlayableGroupRemoved(SCR_AIGroup group)
	{
		if(group.GetGroupID() == m_iLastGroupIWasLeadIn)
		{
			m_iLastGroupIWasLeadIn = -1;
			
			m_bGroupLeadChanged = true;

			_print("We are no longer group lead of " + group.GetGroupID());
		}
	}
	
	protected void OnPlayerLeaderChanged(int groupID, int playerID)
	{
		if(m_iLastGroupIWasLeadIn == groupID)
		{
			m_iLastGroupIWasLeadIn = -1;
			
			m_bGroupLeadChanged = true;

			_print("We are no longer group lead of " + groupID);
		}
		
		if(playerID == m_PlayerController.GetPlayerId())
		{
			m_iLastGroupIWasLeadIn = groupID;
			
			m_bGroupLeadChanged = true;

			_print("We became group lead of " + groupID);
		}
	}

	
	//------------------------------------------------------------------------------------------------
	// Called by PR_GM_PossessingManagerComponent
	// Sadly EOnFrame doesn't work on server for player-owned player controllers :( thus we update it from elsewhere
	void EOnFrameServer(IEntity owner, float timeSlice)
	{
		if (m_bFactionChanged || m_bGroupLeadChanged)
		{
			// Clean up previous dummy entity if it existed
			if (m_DummyEntity)
			{
				_print("Deleting current dummy entity");
				DeleteDummyEntity();
			}
		}
		
		PR_EPossessionState state = GetState();
		
		// What to do depends on state
		
		if (state == PR_EPossessionState.NO_FACTION && m_DummyEntity)
		{
			// We don't have a faction
			// Delete dummy if we have it
			DeleteDummyEntity();
		}
		else if (state == PR_EPossessionState.NONE)
		{
			// Create dummy if we don't have it, possess it
			// The prefab depends on whether we are a group leader or not
			
			SCR_AIGroup group = SCR_GroupsManagerComponent.GetInstance().GetPlayerGroup(m_PlayerController.GetPlayerId());
			bool isLeader = false;
			if (group)
				isLeader = group.IsPlayerLeader(m_PlayerController.GetPlayerId());
			
			if(isLeader)
			{
				_print("Possessing dummy leader");
				PossessDummyEntity(SpawnDummyEntity(SCR_Faction.Cast(PR_FactionMemberManager.GetInstance().GetPlayerFaction(m_PlayerController.GetPlayerId())), 1));
			}
			else
			{
				_print("Possessing regular dummy");
				PossessDummyEntity(SpawnDummyEntity(SCR_Faction.Cast(PR_FactionMemberManager.GetInstance().GetPlayerFaction(m_PlayerController.GetPlayerId())), 0));
			}
		}
		else if (state == PR_EPossessionState.MAIN && m_DummyEntity)
		{
			// Clean up dummy if we are back at main character
			// This is needed because when we switch to Editor UI, a dummy is created again,
			// So when we switch back, we should clean it up
			DeleteDummyEntity();
		}
		else if (state == PR_EPossessionState.EDITOR && m_DummyEntity)
		{
			DeleteDummyEntity();
		}
		
		// If we are in DUMMY or MAIN state, don't do anything
		
		m_bFactionChanged = false;
		m_bGroupLeadChanged = false;
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
		if(!m_PlayerController && m_PlayerController.GetPlayerId() != playerID)
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
		
		PR_GameMode gm = PR_GameMode.Cast(GetGame().GetGameMode());
		if (gm)
			gm.OnPlayerMainCharacterPossessed(mainEntity);
		
		SCR_PossessingManagerComponent scrPossessingMgr = SCR_PossessingManagerComponent.GetInstance();
		if (scrPossessingMgr)
			scrPossessingMgr.SetMainEntity(m_PlayerController.GetPlayerId(), mainEntity, mainEntity, false);
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
	
	protected IEntity SpawnDummyEntity(SCR_Faction faction, bool squadLead)
	{
		_print("SpawnDummyEntity()");
		
		
		ResourceName dummyPrefab;
		if( squadLead)
			dummyPrefab = faction.GetDummyRadioPrefab();
		else	
			dummyPrefab = faction.GetDummyPrefab();

		EntitySpawnParams sp = new EntitySpawnParams();
		sp.TransformMode = ETransformMode.WORLD;
		sp.Transform[0] = Vector(1, 0, 0);
		sp.Transform[1] = Vector(0, 1, 0);
		sp.Transform[2] = Vector(0, 0, 1);
		sp.Transform[3] = faction.GetFactionDummySpawnPosition();
		IEntity ent = GetGame().SpawnEntityPrefab(Resource.Load(dummyPrefab), params: sp);
		
		//IEntity ent = SCR_RespawnSystemComponent.GetInstance().DoSpawn(dummyPrefab, faction.GetFactionDummySpawnPosition());
		
		// Verify that prefab name matches,
		// If not then the whole logic fails and we will end up with entities being infinitely created
		ResourceName spawnedResourceName = ent.GetPrefabData().GetPrefabName();
		if (spawnedResourceName != dummyPrefab)
		{
			_print(string.Format("Created dummy character has wrong prefab name! %1 != %2", spawnedResourceName, dummyPrefab), LogLevel.ERROR);
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