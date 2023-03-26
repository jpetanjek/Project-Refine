[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class PR_SpawnPointClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
/*!
Component which represents spawn point functionality.
*/
class PR_SpawnPoint : ScriptComponent
{
	protected static ref array<PR_SpawnPoint> s_aAll = {};
	
	// Spawn positions for characters
	protected ref array<PR_CharacterSpawnPosition> m_aSpawnPositions = {};

	// TODO: Too capture area specific
	protected PR_CaptureArea m_CaptureArea; // Temporary, might get removed later
	
	protected ref array<int> m_aEnqueuedPlayers;
	
	protected float m_fRespawnWaveRateMs = 10000;
	protected float m_fRespawmTimer = 0;

	
	//------------------------------------------------------------------------------------------------
	void PR_SpawnPoint(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		s_aAll.Insert(this);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~PR_SpawnPoint()
	{
		s_aAll.RemoveItem(this);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		m_CaptureArea = PR_CaptureArea.Cast(owner.FindComponent(PR_CaptureArea));
		
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
		PR_FactionManager factionManager = PR_FactionManager.Cast(GetGame().GetFactionManager());
		if(factionManager)
		{
			factionManager.GetOnPlayerChangedFaction().Insert(OnPlayerChangedFaction);	
		}
		
		// Subscribe to player disconnect
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode)
			gameMode.GetOnPlayerDisconnected().Insert(OnPlayerDisconnected);
		
		// Subscribe to any player changing role
		PR_GroupRoleManagerComponent.m_OnPlayerClaimedRoleChanged.Insert(OnPlayerClaimedRoleChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
	
		
	//------------------------------------------------------------------------------------------------
	// Getters
	
	//! Returns all spawn points
	static array<PR_SpawnPoint> GetAll()
	{
		array<PR_SpawnPoint> a = {};
		a.Copy(s_aAll);
		return a;
	}
	
	//! Returns ID of faction which is allowed to spawn here
	int GetFactionId()
	{
		return m_CaptureArea.GetOwnerFactionId(); // For now we delegate it to spawn point
	}
	
	//! Returns spawn point name
	string GetName()
	{
		return m_CaptureArea.GetName();
	}
	
	
	//------------------------------------------------------------------------------------------------
	// Runtime Logic
	
	//! Enqueue player for spawning on this spawn point
	void EnqueuePlayer(int playerID)
	{
		if(CanPlayerEnqueue(playerID))
			m_aEnqueuedPlayers.Insert(playerID);
	}
	
	bool CanPlayerEnqueue(int playerID)
	{
		PR_FactionManager factionManager = PR_FactionManager.Cast(GetGame().GetFactionManager());
		if(!factionManager)
			return false;
		
		if(GetFactionId() != factionManager.GetPlayerFactionIndex(playerID))
			return false;
		
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		if(!groupsManager)
			return false;
		
		// If you aren't in a group you can't enqueu
		SCR_AIGroup playerGroup = groupsManager.GetPlayerGroup(playerID);
		if(!playerGroup)
			return false;
		
		PR_GroupRoleManagerComponent roleManager = PR_GroupRoleManagerComponent.Cast(playerGroup.FindComponent(PR_GroupRoleManagerComponent));
		if(!roleManager)
			return false;
		
		// Cannot enqueue if you don't have a claimed role
		if(!roleManager.GetPlayerRole(playerID))
			return false;
		
		// TODO: Reserved resource check
		
		return true;
	}
	
	void OnPlayerChangedFaction(int playerID, int newFactionIdx)
	{
		// Remove him from queue if he is enqueued && is not same faction any more
		if(newFactionIdx != GetFactionId())
		{
			int idx = m_aEnqueuedPlayers.Find(playerID);
			if(idx != -1)
				m_aEnqueuedPlayers.Remove(idx);
		}
		
	}
	
	void OnPlayerClaimedRoleChanged(PR_GroupRoleManagerComponent groupRoleManagerComponent, int playerID, int role)
	{
		// Remove him from queue if he is enqueued && doesn't have a role
		if(role == -1)
		{
			int idx = m_aEnqueuedPlayers.Find(playerID);
			if(idx != -1)
				m_aEnqueuedPlayers.Remove(idx);
		}
	}
	
	void OnPlayerDisconnected(int playerID)
	{
		int idx = m_aEnqueuedPlayers.Find(playerID);
		if(idx != -1)
			m_aEnqueuedPlayers.Remove(idx);
	}
	
	//! Even if faction owns a spawn point, respawn there might be blocked for various reasons.
	bool GetRespawnAllowed()
	{
		// TODO: Are there any enemy close?
		// TODO: Are there enough supplies to spawn?
		// TODO: Does it have signal?
		return true;
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
	
	
	// Maybe better FixedFrame?
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		// Tick respawn wave timer
		m_fRespawmTimer += timeSlice;
		
		if(GetRespawnAllowed() && !m_aEnqueuedPlayers.IsEmpty())
		{
			for(int i = 0; i < m_aEnqueuedPlayers.Count(); i++)
			{
				int playerID = m_aEnqueuedPlayers[i];
				// If respawn timer has ticked down
				if(m_fRespawmTimer >= m_fRespawnWaveRateMs)
				{
					// Find Empty spot
					
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
								if(role)
								{
									PR_RespawnSystemComponent.GetInstance().DoSpawn(role.GetPrefab(), GetRandomSpawnPosition());
									
									// TODO: Decrement cost of spawn from supplies
								}
							}
						}
					}
				}
			}
		}
		
		// If respawn timer has ticked down
		if(m_fRespawmTimer >= m_fRespawnWaveRateMs)
		{
			m_fRespawmTimer = 0;
		}
	}	
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// UI Logic
	bool IsAvailable()
	{
		return GetRespawnAllowed() && CanPlayerEnqueue(SCR_PlayerController.GetLocalPlayerId());
	}

	//-------------------------------------------------------------------------------------------------------------------------------
	// Debug
	protected void _print(string str, LogLevel logLevel = LogLevel.NORMAL)
	{
		Print(string.Format("[PR_SpawnPoint] %1: %2", GetName(), str), logLevel);
	}
};
