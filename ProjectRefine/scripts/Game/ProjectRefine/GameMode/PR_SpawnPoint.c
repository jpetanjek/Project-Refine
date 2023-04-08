[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class PR_SpawnPointClass : ScriptComponentClass
{
};

typedef func OnPlayerEnqueuedOnSpawnPoint;
void OnPlayerEnqueuedOnSpawnPoint(int playerID, PR_SpawnPoint spawnPoint);

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
	
	[RplProp(onRplName: "EnqueuedPlayersChanged")]
	protected ref array<int> m_aEnqueuedPlayers = {};
	
	protected ref array<int> m_aOldEnqueuedPlayers = {};
	
	protected float m_fRespawnWaveRateSec = 5;
	protected float m_fRespawmTimer = 0;
	
	// Events
	// Server and client event
	protected ref ScriptInvokerBase<OnPlayerEnqueuedOnSpawnPoint> m_OnPlayerEnqueuedOnSpawnPoint = new ScriptInvokerBase<OnPlayerEnqueuedOnSpawnPoint>();
	
	ScriptInvokerBase<OnPlayerEnqueuedOnSpawnPoint> GetOnPlayerEnqueuedOnSpawnPoint()
	{
		return m_OnPlayerEnqueuedOnSpawnPoint;
	}	
	
	//------------------------------------------------------------------------------------------------
	// Initialization
	void PR_SpawnPoint(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		s_aAll.Insert(this);
	}
	
	void ~PR_SpawnPoint()
	{
		s_aAll.RemoveItem(this);
	}

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
		PR_FactionMemberManager factionMemberManager = PR_FactionMemberManager.Cast(GetGame().GetGameMode().FindComponent(PR_FactionMemberManager));
		if(factionMemberManager)
		{
			factionMemberManager.GetOnPlayerChangedFaction().Insert(OnPlayerChangedFaction);	
		}
		
		// Subscribe to player disconnect
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode)
			gameMode.GetOnPlayerDisconnected().Insert(OnPlayerDisconnected);
	}
	
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
					int playerID = m_aOldEnqueuedPlayers[i];
					m_OnPlayerEnqueuedOnSpawnPoint.Invoke(playerID, this);
				}
			}
		}
	}
	
	// Dequeue's
	void OnPlayerChangedFaction(int playerID, int newFactionIdx)
	{
		// Remove him from queue if he is enqueued && is not same faction any more
		if(newFactionIdx != GetFactionId())
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
	
	//------------------------------------------------------------------------------------------------
	// Runtime Logic
	bool CanPlayerEnqueue(int playerID)
	{
		PR_FactionMemberManager factionMemberManager = PR_FactionMemberManager.Cast(GetGame().GetGameMode().FindComponent(PR_FactionMemberManager));
		if(!factionMemberManager)
			return false;
		
		if(GetFactionId() != factionMemberManager.GetPlayerFactionIndex(playerID))
			return false;
		
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		if(!groupsManager)
			return false;
		
		// If you aren't in a group you can't enqueu
		SCR_AIGroup playerGroup = groupsManager.GetPlayerGroup(playerID);
		if(!playerGroup)
			return false;
		
		// TODO: Does spawn point belong to his group (rally point)
		
		return true;
	}
	
	bool CanPlayerSpawn(int playerID)
	{
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		if(!groupsManager)
			return false;
		
		SCR_AIGroup playerGroup = groupsManager.GetPlayerGroup(playerID);
		if(!playerGroup)
			return false;
		
		// Does he have claimed role?
		PR_GroupRoleManagerComponent roleManager = PR_GroupRoleManagerComponent.Cast(playerGroup.FindComponent(PR_GroupRoleManagerComponent));
        if(!roleManager)
            return false;
        
        // Cannot enqueue if you don't have a claimed role
        if(!roleManager.GetPlayerRole(playerID))
            return false;
		
		// TODO: Reserved resource check
		
		return true;
	}
	
	//! Even if faction owns a spawn point, respawn there might be blocked for various reasons.
	bool IsRespawnAllowed()
	{
		if(Replication.IsServer())
		{
			// TODO: Are there any enemy close?
			// TODO: Does it have signal?
			return true;
		}
		else
		{
			// Check replicated property from server
			return true;
		}
	}
	
	// Maybe better FixedFrame?
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		// Tick respawn wave timer
		m_fRespawmTimer += timeSlice;
		
		// If respawn timer has ticked down
		if(IsRespawnAllowed() && !m_aEnqueuedPlayers.IsEmpty() && m_fRespawmTimer >= m_fRespawnWaveRateSec)
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
							if(role && CanPlayerSpawn(playerID))
							{
								GenericEntity spawnedEntity = PR_RespawnSystemComponent.GetInstance().DoSpawn(role.GetPrefab(), GetRandomSpawnPosition());
								
								PlayerController playerController = GetGame().GetPlayerManager().GetPlayerController(playerID);
								playerController.SetControlledEntity(spawnedEntity);							
								
								// TODO: Decrement cost of spawn from supplies
								
								m_aEnqueuedPlayers.Remove(i);
							}
						}
					}
				}
			}
		}
		
		// If respawn timer has ticked down
		if(m_fRespawmTimer >= m_fRespawnWaveRateSec)
		{
			m_fRespawmTimer = 0;
		}
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
	bool IsAvailable()
	{
		return IsRespawnAllowed() && CanPlayerEnqueue(SCR_PlayerController.GetLocalPlayerId());
	}

	//-------------------------------------------------------------------------------------------------------------------------------
	// Debug
	protected void _print(string str, LogLevel logLevel = LogLevel.NORMAL)
	{
		Print(string.Format("[PR_SpawnPoint] %1: %2", GetName(), str), logLevel);
	}
};
