typedef func OnPlayerEnqueuedOnSpawnPoint;
void OnPlayerEnqueuedOnSpawnPoint(int playerID, PR_BaseSpawnPoint spawnPoint);

enum PR_ESpawnCondition
{
	SPAWN_AVAILABLE,	// All is fine and we can deploy
	NO_GROUP,
	NO_ROLE
}

enum PR_ESpawnPointStateFlags
{
	OCCUPIED_BY_ENEMY	= 1<<0,
	ACTIVE 				= 1<<1// finish this later
}



[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class PR_BaseSpawnPointClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
/*!
Component which represents spawn point functionality.
*/
class PR_BaseSpawnPoint : ScriptComponent
{
	protected const float ENEMY_EVALUATION_INTERVAL_MS = 1000;
	// How many enemies must be in radius for this spawn point to be occupied by enemy
	protected const int ENEMY_COUNT_OCCUPATION_THRESHOLD = 2;
	
	protected static ref array<PR_BaseSpawnPoint> s_aAll = {};
	
	// Spawn positions for characters
	protected ref array<PR_CharacterSpawnPosition> m_aSpawnPositions = {};
	
	[RplProp(onRplName: "EnqueuedPlayersChanged")]
	protected ref array<int> m_aEnqueuedPlayers = {};
	
	protected ref array<int> m_aOldEnqueuedPlayers = {};
	
	// Respawn timer
	[RplProp()]
	protected float m_fNextRespawnWaveTime_ms; // Replication.Time()
	protected float m_fRespawnWaveInterval_ms = 7000.0;

	protected float m_fNextEnemyEvaluationTime_ms; // World time
		
	// Events
	// Server and client event
	protected ref ScriptInvokerBase<OnPlayerEnqueuedOnSpawnPoint> m_OnPlayerEnqueuedOnSpawnPoint = new ScriptInvokerBase<OnPlayerEnqueuedOnSpawnPoint>();
	
	// Flags - broadcasted flags representing this spawn point state
	[RplProp()]
	protected PR_ESpawnPointStateFlags m_eStateFlags;
	
	[Attribute("1.0", uiwidget: UIWidgets.EditBox)]
	protected float m_fEnemyOccupationRadius;
	
	ScriptInvokerBase<OnPlayerEnqueuedOnSpawnPoint> GetOnPlayerEnqueuedOnSpawnPoint()
	{
		return m_OnPlayerEnqueuedOnSpawnPoint;
	}
	
	//------------------------------------------------------------------------------------------------
	// Initialization
	void PR_BaseSpawnPoint(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		s_aAll.Insert(this);
	}
	
	void ~PR_BaseSpawnPoint()
	{
		s_aAll.RemoveItem(this);
	}

	override void EOnInit(IEntity owner)
	{
		// Search for PR_CharacterSpawnPosition entities and pass them to spawn point
		IEntity childEntity = GetOwner().GetChildren();
		int nCharSpawnPosAdded = 0;
		while (childEntity)
		{
			PR_CharacterSpawnPosition spawnPos = PR_CharacterSpawnPosition.Cast(childEntity);
			if (spawnPos)
			{
				m_aSpawnPositions.Insert(spawnPos);
				nCharSpawnPosAdded++;
			}
			childEntity = childEntity.GetSibling();
		}
		
		if (nCharSpawnPosAdded == 0)
			_print("No character spawn positions found!", LogLevel.ERROR);
		
		
		// Subscribe to player chancing faction
		BaseGameMode gm = GetGame().GetGameMode();
		if (gm)
		{
			PR_FactionMemberManager factionMemberManager = PR_FactionMemberManager.Cast(gm.FindComponent(PR_FactionMemberManager));
			if(factionMemberManager)
				factionMemberManager.GetOnPlayerChangedFaction().Insert(OnPlayerChangedFaction);	
		}
		
		// Subscribe to player disconnect
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode)
			gameMode.GetOnPlayerDisconnected().Insert(OnPlayerDisconnected);
	}
	
	override void OnPostInit(IEntity owner)
	{
		if(Replication.IsServer())
			SetEventMask(owner, EntityEvent.INIT | EntityEvent.DIAG | EntityEvent.FRAME);
		else
			SetEventMask(owner, EntityEvent.INIT | EntityEvent.DIAG);
		
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
	
	//------------------------------------------------------------------------------------------------
	// Virtual functions. They must be implemented in base classes.
	
	// Must return ID of owner faction
	int GetOwnerFactionId() { return -1; }
	
	// Must return name for UI
	string GetName();
		
	//------------------------------------------------------------------------------------------------
	// Getters
	
	//! Returns all spawn points
	static array<PR_BaseSpawnPoint> GetAll()
	{
		array<PR_BaseSpawnPoint> a = {};
		a.Copy(s_aAll);
		return a;
	}
	
	// Returns owner faction
	Faction GetFaction()
	{
		return GetGame().GetFactionManager().GetFactionByIndex(GetOwnerFactionId());
	}
	
	//-------------------------------------------------------------------------------------------
	PR_ESpawnPointStateFlags GetStateFlags()
	{
		return m_eStateFlags;
	}
	
	//-------------------------------------------------------------------------------------------
	float GetNextRespawnWaveTime()
	{
		return m_fNextRespawnWaveTime_ms;
	}
	
	//-------------------------------------------------------------------------------------------
	// Events
	
	// Client side resolving of newly enqueued players based on old state vs new state
	void EnqueuedPlayersChanged()
	{
		if(Replication.IsClient())
		{
			// Calculate changes
			if(m_aOldEnqueuedPlayers.Count())
			{
				for(int i = 0; i < m_aOldEnqueuedPlayers.Count(); i++)
				{
					int playerID = m_aOldEnqueuedPlayers[i];
					
					if(!m_aEnqueuedPlayers.Contains(playerID))
					{
						m_OnPlayerEnqueuedOnSpawnPoint.Invoke(playerID, this);
					}
				}
			}
			else
			{
				for(int i = 0; i < m_aEnqueuedPlayers.Count(); i++)
				{
					int playerID = m_aEnqueuedPlayers[i];
					m_OnPlayerEnqueuedOnSpawnPoint.Invoke(playerID, this);
				}
			}
		}
	}
	
	// Dequeue's
	void OnPlayerChangedFaction(int playerID, int newFactionIdx)
	{
		// Remove him from queue if he is enqueued && is not same faction any more
		if(newFactionIdx != GetOwnerFactionId())
		{
			DequeuePlayer(playerID);
		}
	}
	
	void OnPlayerDisconnected(int playerID)
	{
		DequeuePlayer(playerID);
	}
		
	void DequeuePlayer(int playerID)
	{
		int idx = m_aEnqueuedPlayers.Find(playerID);
		if(idx != -1)
		{
			m_aEnqueuedPlayers.Remove(idx);
			Replication.BumpMe();
		}
	}
	
	//! Enqueue player for spawning on this spawn point
	void EnqueuePlayer(int playerID)
	{	
		for(int i = 0; i < s_aAll.Count(); i++)
		{
			s_aAll[i].DequeuePlayer(playerID);
		}
		
		m_aEnqueuedPlayers.Insert(playerID);
		Replication.BumpMe();
	}
	
	bool IsPlayerEnqueued(int playerId)
	{
		return m_aEnqueuedPlayers.Find(playerId) != -1;
	}
	
	//------------------------------------------------------------------------------------------------
	// Runtime Logic
	bool CanPlayerEnqueue(int playerID)
	{
		PR_FactionMemberManager factionMemberManager = PR_FactionMemberManager.Cast(GetGame().GetGameMode().FindComponent(PR_FactionMemberManager));
		if(!factionMemberManager)
			return false;
		
		if(GetOwnerFactionId() != factionMemberManager.GetPlayerFactionIndex(playerID))
			return false;
		
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		if(!groupsManager)
			return false;
		
		// If you aren't in a group you can't enqueu
		SCR_AIGroup playerGroup = groupsManager.GetPlayerGroup(playerID);
		if(!playerGroup)
			return false;
		
		// If you are already spawned you cannot enqueue
		if(GetGame().GetPlayerManager().GetPlayerController(playerID).GetControlledEntity())
			return false;
		
		// TODO: Does spawn point belong to his group (rally point)
		
		return true;
	}
	
	// This code must also work on client, it's used in deployment menu UI
	static PR_ESpawnCondition CanPlayerSpawn(int playerId)
	{
		// Have group?
		SCR_GroupsManagerComponent groupsMgr = SCR_GroupsManagerComponent.GetInstance();
		SCR_AIGroup playerGroup = groupsMgr.GetPlayerGroup(playerId);
		if (!playerGroup)
			return PR_ESpawnCondition.NO_GROUP;
		
		// Have role?
		PR_GroupRoleManagerComponent groupRoleMgr = PR_GroupRoleManagerComponent.Cast(playerGroup.FindComponent(PR_GroupRoleManagerComponent));
		if(!groupRoleMgr.GetPlayerRole(playerId))
			return PR_ESpawnCondition.NO_ROLE;
		
		// TODO: Reserved resource check
		
		// All good
		return PR_ESpawnCondition.SPAWN_AVAILABLE;
	}
	
	// Conditions related to spawn point alone
	bool IsRespawnAllowed()
	{
		if ((m_eStateFlags & PR_ESpawnPointStateFlags.OCCUPIED_BY_ENEMY) == 0)
			return true;
		else
			return false;
	}
	
	// Maybe better FixedFrame?
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		float currentTime_ms = Replication.Time();
		
		// If respawn timer has ticked down
		bool disableDeploymentChecks = DiagMenu.GetBool(SCR_DebugMenuID.REFINE_DISABLE_DEPLOYMENT_COUNTDOWN);
		if((currentTime_ms >= m_fNextRespawnWaveTime_ms) || disableDeploymentChecks)
		{
			// Spawn players
			if (IsRespawnAllowed())
			{
				for(int i = m_aEnqueuedPlayers.Count()-1; i >= 0 ; i--)
				{
					int playerID = m_aEnqueuedPlayers[i];
					
					// Spawn with claimed role
					SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
					if(groupsManager)
					{
						SCR_AIGroup playerGroup = groupsManager.GetPlayerGroup(playerID);
						if(playerGroup)
						{
							PR_GroupRoleManagerComponent roleManager = PR_GroupRoleManagerComponent.Cast(playerGroup.FindComponent(PR_GroupRoleManagerComponent));
							if(roleManager)
							{
								// Cannot spawn if you don't have a claimed role
								PR_Role role = roleManager.GetPlayerRole(playerID);
								if(role && CanPlayerSpawn(playerID) == PR_ESpawnCondition.SPAWN_AVAILABLE)
								{
									GenericEntity spawnedEntity = SCR_RespawnSystemComponent.GetInstance().DoSpawn(role.GetPrefab(), GetRandomSpawnPosition());
									
									SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerID));
									playerController.SetPossessedEntity(spawnedEntity);
																	
									// TODO: Decrement cost of spawn from supplies
									
									m_aEnqueuedPlayers.Remove(i);
								}
							}
						}
					}
				}
			}
			
			// Set and broadcast next respawn wave time ...
			m_fNextRespawnWaveTime_ms = currentTime_ms + m_fRespawnWaveInterval_ms;
			Replication.BumpMe();
		}
		
		// Evaluate if spawn point is occupied by enemies
		if (GetGame().GetWorld().GetWorldTime() > m_fNextEnemyEvaluationTime_ms)
		{
			EvaluateEnemies();
			m_fNextEnemyEvaluationTime_ms = GetGame().GetWorld().GetWorldTime() + ENEMY_EVALUATION_INTERVAL_MS;
		}
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Evaluates if spawn point is occupied by enemies
	void EvaluateEnemies()
	{
		PR_ESpawnPointStateFlags oldFlags = m_eStateFlags;
		
		Faction thisFaction = GetFaction();
		vector thisPos = GetOwner().GetOrigin();
		int nEnemiesInRadius = 0;
		

		if (m_fEnemyOccupationRadius > 0)
		{
			array<SCR_ChimeraCharacter> allCharacters = SCR_ChimeraCharacter.GetAllCharacters();
		
			foreach(SCR_ChimeraCharacter character : allCharacters)
			{
				if (character.m_DamageManager.IsDestroyed())
					continue;
				
				if (character.m_pFactionComponent.GetAffiliatedFaction() != thisFaction)
				{
					if (vector.DistanceXZ(character.GetOrigin(), thisPos) < m_fEnemyOccupationRadius)
					{
						nEnemiesInRadius++;
						if (nEnemiesInRadius >= ENEMY_COUNT_OCCUPATION_THRESHOLD)
							break;
					}
				}
			}
		}
		
		if (nEnemiesInRadius >= ENEMY_COUNT_OCCUPATION_THRESHOLD)
			m_eStateFlags |= PR_ESpawnPointStateFlags.OCCUPIED_BY_ENEMY;
		else
			m_eStateFlags &= ~PR_ESpawnPointStateFlags.OCCUPIED_BY_ENEMY;
		
		if (m_eStateFlags != oldFlags)
			Replication.BumpMe();	
	}
	
	// Returns world space coordinate of a random spawn position
	// TODO: Too capture point specific
	vector GetRandomSpawnPosition()
	{
		if (m_aSpawnPositions.IsEmpty())
			return GetOwner().GetOrigin();
		
		int id = Math.RandomInt(0, m_aSpawnPositions.Count());
		return m_aSpawnPositions[id].GetOrigin();
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// UI Logic
	/*
	// We don't use it?
	bool IsAvailable2()
	{
		return IsRespawnAllowed() && CanPlayerEnqueue(SCR_PlayerController.GetLocalPlayerId());
	}
	*/

	//-------------------------------------------------------------------------------------------------------------------------------
	// Debug
	protected void _print(string str, LogLevel logLevel = LogLevel.NORMAL)
	{
		Print(string.Format("[PR_SpawnPoint] %1: %2", GetName(), str), logLevel);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnDiag(IEntity owner, float timeSlice)
	{
		if (DiagMenu.GetBool(SCR_DebugMenuID.REFINE_SHOW_SPAWN_POINT_STATE))
		{
			// Draw debug text
			const int COLOR_TEXT = Color.WHITE;
		 	const int COLOR_BACKGROUND = Color.BLACK;
			
			string s;
			
			float timeTilSpawnWave_ms = m_fNextRespawnWaveTime_ms - Replication.Time();
			float timeTillSpawnWave_s = timeTilSpawnWave_ms / 1000.0;
			s = s + string.Format("Spawn Wave: %1\n", timeTillSpawnWave_s.ToString(4, 1));
			
			s = s + string.Format("Flags: %1\n", SCR_Enum.FlagsToString(PR_ESpawnPointStateFlags, m_eStateFlags));
			
			vector pos = owner.GetOrigin() + Vector(0, 5, 0);
			DebugTextWorldSpace.Create(GetGame().GetWorld(), s, DebugTextFlags.ONCE, pos[0], pos[1], pos[2], size: 13.0, color: COLOR_TEXT, bgColor: COLOR_BACKGROUND);
			
			// Cylinder with enemy occupation radius
			Shape.CreateCylinder(Color.RED, ShapeFlags.VISIBLE | ShapeFlags.ONCE | ShapeFlags.WIREFRAME, GetOwner().GetOrigin(), m_fEnemyOccupationRadius, 40.0);
		}
	}
};
