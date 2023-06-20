class PR_GameModeClass : SCR_BaseGameModeClass
{
}

typedef func OnGameModeStageChangedDelegate;
void OnGameModeStageChangedDelegate(PR_EGameModeStage stage);

enum PR_EGameModeStage
{
	PREPARATION,
	LIVE,
	DEBRIEF
}

enum PR_EGameModeArchetype
{
	ADVANCE_AND_SECURE,	// Able to capture any active capture area
	INVASION			// Only one team is able to capture areas
}

class PR_GameMode : SCR_BaseGameMode
{
	protected static const float GAME_MODE_UPDATE_INTERVAL_S = 1.0;
	
	protected static const float PREPARATION_DURATION_S = 240.0;
	
	[Attribute(desc: "Mission header which will be used if world is launched without mission header. Used mainly for testing.")]
	protected ref PR_MissionHeader m_TestMissionHeader;
	
	// Initialized from mission header
	protected PR_EGameModeArchetype m_eGameModeArchetype;
	
	// Pointers to areas
	protected ref array<PR_CaptureArea> m_aAreas = {}; // Array with all capture areas.
	protected PR_CaptureArea m_MainBaseArea0;
	protected PR_CaptureArea m_MainBaseArea1;
	
	// Pointer to supply holder of main base
	PR_SupplyHolderComponent m_Main0_supply;
	PR_SupplyHolderComponent m_Main1_supply;
	
	// Score of each faction
	[RplProp()]
	protected int m_iFactionScore0;
	[RplProp()]
	protected int m_iFactionScore1;
	
	// Game mode stage
	[RplProp(onRplName: "OnGameModeChangedClient")]
	protected PR_EGameModeStage m_eGameModeStage = PR_EGameModeStage.PREPARATION;
	
	// RPL props - factions
	// IDs of factions which were assigned after initialization and randomization
	// We must replicate them from server to clients after game mode init
	[RplProp()]
	protected int m_iFaction0;	// Invader in Invasion
	[RplProp()]
	protected int m_iFaction1;	// Defender in Invasion
	
	// Events
	ref ScriptInvokerBase<OnGameModeStageChangedDelegate> m_OnGameModeStageChanged = new ScriptInvokerBase<OnGameModeStageChangedDelegate>();
	
	// Other
	protected float m_fGameModeUpdateTimer = 0;
	
	protected SCR_GroupsManagerComponent m_GroupManager;
	
	protected bool m_bInvokePreparation = false;

	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Public functions
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Returns Game Mode Archetype
	int GetArchetype()
	{
		return m_eGameModeArchetype;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Returns invading faction
	int GetInvadingFactionId()
	{
		return m_iFaction0;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Returns defending faction
	int GetDefendingFactionId()
	{
		return m_iFaction1;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Returns area at given position
	PR_CaptureArea GetAreaAtPos(vector posWorld)
	{
		foreach (PR_CaptureArea a : m_aAreas)
		{
			if (vector.DistanceXZ(a.GetOwner().GetOrigin(), posWorld) < a.m_fRadius)
				return a;
		}
		
		return null;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Returns true if given capture area is a main base
	bool IsMainBaseArea(PR_CaptureArea area)
	{
		return (area == m_MainBaseArea0) || (area == m_MainBaseArea1);
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	void PR_GameMode(IEntitySource src, IEntity parent)
	{
		int mask = EntityEvent.INIT | EntityEvent.FRAME | EntityEvent.DIAG;
		SetEventMask(mask);
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	override void OnGameStart()
	{
		super.OnGameStart();
		
		if (!GetGame().InPlayMode())
			return;
		
		//--------------------------------------------------------
		// Resolve mission config
		
		PR_MissionHeader header = PR_MissionHeader.Cast(GetGame().GetMissionHeader());
		if (!header)
			header = m_TestMissionHeader;
		
		// At this point it's a complete failure, since we require the values from mission header
		if (!header)
		{
			_print("Fatal error: no mission config is provided. The world must be started with a mission config, otherwise it must be provided through m_TestMissionHeader property", LogLevel.ERROR);
			return;
		}
		
		//--------------------------------------------------------
		// Resolve archetype
		m_eGameModeArchetype = header.m_eRefineGameModeArchetype;
		
		//--------------------------------------------------------
		// Resolve factions
		
		array<int> factionIds = {};
		FactionManager fm = GetGame().GetFactionManager();
		if (!fm)
		{
			_print("Fatal error: Faction Manager was not found.", LogLevel.ERROR);
			return;
		}
		array<string> factionKeys = {header.m_sRefineFaction_0, header.m_sRefineFaction_1};
		foreach (int i, string factionKey : factionKeys)
		{
			Faction faction = fm.GetFactionByKey(factionKey);
			if (!faction)
			{
				_print(string.Format("Fatal error: faction %1 with key \'%2\' was not found", i, factionKey));
				return;
			}
			
			int factionId = fm.GetFactionIndex(faction);
			_print(string.Format("Using faction %1: key \'%2\', ID: %3", i, factionKey, factionId));
			factionIds.Insert(factionId);
		}
		
		
		//--------------------------------------------------------
		// Randomize factions
		
		bool randomizeFactions = header.m_bRefineRandomizeFactions;
		
		// Randomize factions - if enabled
		if (IsMaster())
		{
			if (randomizeFactions)
			{
				_print("Randomizing factions...");
				RandomGenerator generator = new RandomGenerator();
				generator.SetSeed(System.GetUnixTime());
				int randInt = generator.RandInt(0, 2048);
				bool shuffleFactions = (randInt % 2) == 0;
				_print(string.Format("  Random int: %1, shuffle factions: %2", randInt, shuffleFactions));
				
				if (shuffleFactions)
				{
					int temp = factionIds[0];
					factionIds[0] = factionIds[1];
					factionIds[1] = temp;
				}
			}
			
			m_iFaction0 = factionIds[0];
			m_iFaction1 = factionIds[1];
			Replication.BumpMe();
		}
		
		
		//--------------------------------------------------------
		// Init capture areas
		
		// Find all child capture areas, sort them by order
		array<PR_CaptureArea> areas = {};
		bool areasInitSuccess = FindCaptureAreas(areas, true);
		
		if (!areasInitSuccess)
		{
			_print("Fatal error: could not initialize capture areas!", LogLevel.ERROR);
			return;
		}
		
		m_aAreas.Copy(areas);
		m_MainBaseArea0 = m_aAreas[0];
		m_MainBaseArea1 = m_aAreas[m_aAreas.Count()-1];
		
		_print("Capture areas initialized successfully!", LogLevel.NORMAL);
		
		// Finish init of capture areas:
		// - Link capture areas with their naighbours
		// - Set capture area faction
		for (int i = 0; i < m_aAreas.Count(); i++)
		{
			array<PR_CaptureArea> linked = {};
			if (i >= 1)
				linked.Insert(m_aAreas[i-1]); // Add prev area
			if (i <= m_aAreas.Count()-2)
				linked.Insert(m_aAreas[i+1]); // Add next area
			
			PR_CaptureArea area = m_aAreas[i];
			
			// Resolve initial owner faction
			int initialOwnerFactionId = -1;
			
			if(m_eGameModeArchetype == PR_EGameModeArchetype.INVASION)
			{
				// 0 is considered main of invader
				if(m_MainBaseArea0 == area)
					initialOwnerFactionId = GetInvadingFactionId();
				else
					initialOwnerFactionId = GetDefendingFactionId();
			}
			else if (m_eGameModeArchetype == PR_EGameModeArchetype.ADVANCE_AND_SECURE)
			{
				if (area == m_MainBaseArea0)
					initialOwnerFactionId = m_iFaction0;
				else if (area == m_MainBaseArea1)
					initialOwnerFactionId = m_iFaction1;
			}
			
			if (IsMaster())
				area.InitMaster(linked, initialOwnerFactionId);
			else
				area.InitProxy(linked);
			
			// Subscribe to events
			area.m_OnOwnerFactionChanged.Insert(OnCaptureAreaFactionChanged);
			
			// Subscribe to asset spawner events
			array<PR_AssetSpawner> assetSpawners = area.GetAssetSpawners();
			foreach (PR_AssetSpawner spawner : assetSpawners)
			{
				spawner.m_OnAssetSpawned.Insert(OnAssetSpawned);
			}
		}
		
		//--------------------------------------------------------
		// Init faction points
		if (IsMaster())
		{
			if(m_eGameModeArchetype == PR_EGameModeArchetype.INVASION)
			{
				m_iFactionScore0 = 100; // Attackers
				m_iFactionScore1 = 500; // Defenders
			}
			else
			{
				m_iFactionScore0 = 300;
				m_iFactionScore1 = 300;
			}			
			Replication.BumpMe();
		}
		
		//--------------------------------------------------------
		// Initialize game mode stage
		m_eGameModeStage = PR_EGameModeStage.PREPARATION;
		
		Replication.BumpMe();
	}
	
	// Returns true on success
	protected bool FindCaptureAreas(notnull array<PR_CaptureArea> outAreas, bool enableLogging)
	{
		/*
		// Find all child capture areas, sort them by order
		array<PR_CaptureArea> areas = outAreas;
		areas.Clear();
		IEntity childEntity = GetChildren();
		while (childEntity)
		{
			PR_CaptureArea ca = PR_CaptureArea.Cast(childEntity.FindComponent(PR_CaptureArea));
			if (ca)
				areas.Insert(ca);
			childEntity = childEntity.GetSibling();
		}
		*/
		
		array<PR_CaptureArea> areas = outAreas;
		PR_CaptureArea.GetAll(areas);
		for (int i = areas.Count()-1; i >= 0; i--)	// Exclude entities from previews and such
		{
			PR_CaptureArea a = areas[i];
			if (a.GetOwner().GetWorld() != GetWorld())
				areas.Remove(i);
		}
		SCR_Sorting<PR_CaptureArea, PR_CaptureArea_CompareOrder>.HeapSort(areas, false);
		
		
		
		if (enableLogging)
		{
			_print(string.Format("Found %1 capture areas:", areas.Count()));
		foreach (int i, PR_CaptureArea a : m_aAreas)
			_print(string.Format("%1: order: %2, entity: %3, name: %4", i, a.m_iOrder, a.GetOwner().GetName(), a.GetName()));
		}
			
		if (areas.Count() < 2)
		{
			if (enableLogging)
				_print("Fatal error: did not find enough capture areas", LogLevel.ERROR);
			
			return false;
		}
		
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	protected float m_timer0 = 0;
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		
		// Update game mode based on timer
		// Only on master!
		if (IsMaster())
		{
			m_fGameModeUpdateTimer += timeSlice;
			if (m_fGameModeUpdateTimer >= GAME_MODE_UPDATE_INTERVAL_S)
			{
				UpdateGameMode(m_fGameModeUpdateTimer);
				m_fGameModeUpdateTimer -= GAME_MODE_UPDATE_INTERVAL_S;
			}
		}
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		InitDiagMenu();
		
		if(Replication.IsClient())
			return;
		
		SCR_AIGroup.GetOnPlayerRemoved().Insert(OnGroupPlayerRemoved);
		
		m_GroupManager = SCR_GroupsManagerComponent.GetInstance();
		
		if(m_GroupManager)
		{
			m_GroupManager.GetOnPlayableGroupCreated().Insert(Event_OnPlayableGroupCreated);
		}
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	override void _WB_AfterWorldUpdate(float timeSlice)
	{
		// Find all capture areas same as during start, draw arrows
		array<PR_CaptureArea> areas = {};
		FindCaptureAreas(areas, false);
		
		vector offset = Vector(0, 20, 0);
		for (int i = 0; i < areas.Count() - 1; i++)
		{
			PR_CaptureArea a0 = areas[i];
			PR_CaptureArea a1 = areas[i+1];
			Shape.CreateArrow(a0.GetOwner().GetOrigin() + offset, a1.GetOwner().GetOrigin() + offset, 2.5, Color.PINK, ShapeFlags.ONCE | ShapeFlags.NOZBUFFER);
		}
	}	
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Player not part of group managerment
	
	//-------------------------------------------------------------------------------------------------------------------------------
	void OnGroupPlayerRemoved(SCR_AIGroup group, int playerID)
	{
		GetGame().GetCallqueue().CallLater(OnElapsedNoGroupTime, 90000, false, playerID, false);
	}
	
	override void OnPlayerConnected(int playerId)
	{
		super.OnPlayerConnected(playerId);
		GetGame().GetCallqueue().CallLater(OnElapsedNoGroupTime, 90000, false, playerId, false);
	}
	
	void OnElapsedNoGroupTime(int playerID, bool kick)
	{
		if (!m_GroupManager)
				return;
		
		if(!kick)
		{
			if(m_GroupManager.GetPlayerGroup(playerID) == null)
			{
				// Issue notification to player
				SCR_NotificationsComponent.SendToPlayer(playerID, ENotification.PR_NO_GROUP);
				GetGame().GetCallqueue().CallLater(OnElapsedNoGroupTime, 30000, false, playerID, true);
			}
		}
		else
		{
			if(m_GroupManager.GetPlayerGroup(playerID) == null)
			{
				// Kick player
				GetGame().GetPlayerManager().KickPlayer(playerID, PlayerManagerKickReason.KICK, 60);
			}
		}
	}

	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Game mode update
	
	//-------------------------------------------------------------------------------------------------------------------------------
	protected void UpdateGameMode(float timeSlice)
	{
		// Do nothing if game has ended or not started yet
		if (!IsRunning())
			return;
		
		switch (m_eGameModeStage)
		{
			case PR_EGameModeStage.PREPARATION:
			{
				TickGameModePreparation(timeSlice);
				break;
			}
			case PR_EGameModeStage.LIVE:
			{
				TickGameModeLive(timeSlice);
				break;
			}
			case PR_EGameModeStage.DEBRIEF:
			{
				TickGameModeDebrief(timeSlice);
				break;
			}
		}
	}
	
	void TickGameModePreparation(float timeSlice)
	{
		if(!m_bInvokePreparation)
		{
			m_bInvokePreparation = true;
			m_OnGameModeStageChanged.Invoke(m_eGameModeStage);
		}
		
		if(m_fTimeElapsed > PREPARATION_DURATION_S || DiagMenu.GetBool(SCR_DebugMenuID.REFINE_DISABLE_PREPARATION_STAGE))
		{
			RequestNextGameModeStage();
		}
	}
	
	void TickGameModeLive(float timeSlice)
	{
		//---------------------------------------------
		// Update areas
		
		array<SCR_ChimeraCharacter> allCharacters = SCR_ChimeraCharacter.GetAllCharacters();
		
		// Remove dead characters
		for (int i = allCharacters.Count() - 1; i >= 0; i--)
		{
			SCR_ChimeraCharacter c = allCharacters[i];
			if (c.m_DamageManager.IsDestroyed())
				allCharacters.Remove(i);
		}
		
		// Iterate all areas, find characters in each area
		array<SCR_ChimeraCharacter> charactersInArea = {};
		foreach (PR_CaptureArea area : m_aAreas)
		{
			vector areaCenter = area.GetOwner().GetOrigin();
			float areaRadius = area.m_fRadius;
			
			charactersInArea.Clear();
			for (int i = allCharacters.Count() - 1; i >= 0; i--)
			{
				SCR_ChimeraCharacter character = allCharacters[i];
				if (vector.DistanceXZ(character.GetOrigin(), areaCenter) < area.m_fRadius)
				{
					charactersInArea.Insert(character);
					allCharacters.Remove(i); // Character can only be in one area. We will not check this character for next area.
				}
			}
			
			// Call update on that area
			area.OnUpdateGameMode(timeSlice, charactersInArea);
		}
		
		//---------------------------------------------
		// Resuply main bases
		if(!m_Main0_supply && m_MainBaseArea0)
			m_Main0_supply = PR_SupplyHolderComponent.Cast(m_MainBaseArea0.GetOwner().FindComponent(PR_SupplyHolderComponent));
		
		if(!m_Main1_supply && m_MainBaseArea1)
			m_Main1_supply = PR_SupplyHolderComponent.Cast(m_MainBaseArea1.GetOwner().FindComponent(PR_SupplyHolderComponent));
		
		if(m_Main0_supply)
			m_Main0_supply.AddSupplies(100);
		
		if(m_Main1_supply)
			m_Main1_supply.AddSupplies(100);

		//---------------------------------------------
		// Update factions score
		
		FactionManager fm = GetGame().GetFactionManager();
		array<Faction> factions = {};
		fm.GetFactionsList(factions);
		int nFactions = factions.Count();
		
		// Count how many areas each faction owns
		array<int> factionAreas = {};
		factionAreas.Resize(nFactions);
		for (int i = 0; i < nFactions; i++)
			factionAreas[i] = 0;
		foreach (PR_CaptureArea area : m_aAreas)
		{
			int ownerFactionId = area.GetOwnerFactionId();
			if (ownerFactionId != -1)
				factionAreas[ownerFactionId] = factionAreas[ownerFactionId] + 1;
		}
		
		if(m_eGameModeArchetype == PR_EGameModeArchetype.INVASION)
		{
			if(factionAreas[GetInvadingFactionId()] >= (m_aAreas.Count() - 1))
			{
				// Defender has lost
				SetFactionScore(GetDefendingFactionId(), -1);
			}
		}
		else
		{
			// Find faction with biggest amount of areas
			int maxOwnedAreas = 0;
			int maxOwnedAreasFactionId = -1;
			for (int i = 0; i < nFactions; i++)
			{
				if (factionAreas[i] > maxOwnedAreas)
				{
					maxOwnedAreas = factionAreas[i];
					maxOwnedAreasFactionId = i;
				}
			}
			
			// Remove tickets from all losing factions
			for (int factionId = 0; factionId < nFactions; factionId++)
			{
				if (factionId == maxOwnedAreasFactionId)
					continue;
				
				// How many areas this factions owns less than the winning faction
				int ownedAreasDifference = maxOwnedAreas - factionAreas[factionId];
				
				if (ownedAreasDifference > 0)
				{
					AddFactionScore(factionId, -1); // !!! Change score decrease rate!
				}
			}
		}
		
		//---------------------------------------------
		// End game if some faction has depleted points
		if (m_iFactionScore0 < 0 || m_iFactionScore1 < 0)
		{
			// Some faction has lost all its points
			// End the game
			RequestNextGameModeStage();
		}
	}
	
	void TickGameModeDebrief(float timeSlice)
	{
		array<int> winnerFactions = {};
		if (m_iFactionScore0 > m_iFactionScore1)
			winnerFactions.Insert(m_iFaction0);
		else if (m_iFactionScore1 > m_iFactionScore0)
			winnerFactions.Insert(m_iFaction1);
		
		// TODO: Delete all assets and players, move all cameras to some position?
		
		// TODO: Display score board?
		
		// TODO: Some timer logic?
		{
			SCR_GameModeEndData gameModeEndData = SCR_GameModeEndData.Create(SCR_GameModeEndData.ENDREASON_SCORELIMIT, winnerIds: null, winnerFactionIds: winnerFactions);
			EndGameMode(gameModeEndData);
		}
	}
	
	void RequestNextGameModeStage()
	{
		switch (m_eGameModeStage)
		{
			case PR_EGameModeStage.PREPARATION:
			{
				m_eGameModeStage = PR_EGameModeStage.LIVE;
				
				break;
			}
			case PR_EGameModeStage.LIVE:
			{
				m_eGameModeStage = PR_EGameModeStage.DEBRIEF;
				break;
			}
			case PR_EGameModeStage.DEBRIEF:
			{
				break;
			}
		}
		m_OnGameModeStageChanged.Invoke(m_eGameModeStage);
				
		Replication.BumpMe();
	}
	
	void OnGameModeChangedClient()
	{
		m_OnGameModeStageChanged.Invoke(m_eGameModeStage);
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	PR_EGameModeStage GetGameModeStage()
	{
		return m_eGameModeStage;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Returns how much more the preparation stage is going to last, in seconds
	float GetPreparationStageRemainingTime()
	{
		return PREPARATION_DURATION_S - m_fTimeElapsed;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Faction score
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Call this to add or remove faction points.
	// points - can be negative or positive
	void AddFactionScore(int factionId, int points)
	{
		if (!IsMaster())
		{
			Print("AddFactionPoints() must be called only on master!", LogLevel.ERROR);
			return;
		}
		
		bool bump = false;
		if (factionId == m_iFaction0)
		{
			m_iFactionScore0 = m_iFactionScore0 + points;
			bump = true;
		}
		else if (factionId == m_iFaction1)
		{
			m_iFactionScore1 = m_iFactionScore1 + points;
			bump = true;
		}
			
		if (bump)
			Replication.BumpMe();
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	void SetFactionScore(int factionId, int value)
	{
		if (!IsMaster())
		{
			Print("SetFactionScore() must be called only on master!", LogLevel.ERROR);
			return;
		}
		
		bool bump = false;
		if (factionId == m_iFaction0)
		{
			m_iFactionScore0 = value;
			bump = true;
		}
		else if (factionId == m_iFaction1)
		{
			m_iFactionScore1 = value;
			bump = true;
		}
			
		if (bump)
			Replication.BumpMe();
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	int GetFactionScore(int factionId)
	{
		if (factionId == m_iFaction0)
			return m_iFactionScore0;
		else if (factionId == m_iFaction1)
			return m_iFactionScore1;
				
		return 0;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Capture areas
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Called on master
	void OnCaptureAreaFactionChanged(PR_CaptureArea area, int oldFactionId, int newFactionId)
	{
		int areaId = m_aAreas.Find(area);
		RpcDo_OnCaptureAreaFactionChanged(areaId, oldFactionId, newFactionId);
		Rpc(RpcDo_OnCaptureAreaFactionChanged, areaId, oldFactionId, newFactionId);
		
		// Add tickets to faction that newly captured the point
		if(newFactionId != -1 && m_eGameModeStage == PR_EGameModeStage.LIVE)
		{
			if(m_eGameModeArchetype == PR_EGameModeArchetype.INVASION)
			{
				if(newFactionId == GetInvadingFactionId())
					AddFactionScore(newFactionId, 100);
			}
			else
			{
				AddFactionScore(newFactionId, 60);
			}
		}
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcDo_OnCaptureAreaFactionChanged(int areaId, int oldFactionId, int newFactionId)
	{
		if (!m_aAreas.IsIndexValid(areaId))
			return;
		PR_CaptureArea area = m_aAreas[areaId];
		
		foreach (auto comp : m_aAdditionalGamemodeComponents)
		{
			PR_BaseGameModeComponent prComp = PR_BaseGameModeComponent.Cast(comp);
			if (prComp)
				prComp.OnCaptureAreaFactionChanged(area, oldFactionId, newFactionId);
		}
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	array<PR_CaptureArea> GetCaptureAreas()
	{
		array<PR_CaptureArea> a = {};
		a.Copy(m_aAreas);
		return a;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Assets
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Roles
	
	//-------------------------------------------------------------------------------------------------------------------------------
	
	void Event_OnPlayableGroupCreated(SCR_AIGroup group)
	{
		if(Replication.IsClient())
			return;
		
		if (!group)
			return;
		
		PR_GroupRoleManagerComponent groupRoleManagerComponent = PR_GroupRoleManagerComponent.Cast(group.FindComponent(PR_GroupRoleManagerComponent));
		
		if(!groupRoleManagerComponent)
			return;
		
		// Hard coded group initialization for now
		// Get faction of group
		SCR_Faction groupFaction = SCR_Faction.Cast(group.GetFaction());
		PR_RoleList fullRoleList = groupFaction.GetRoleList();
		if(fullRoleList == null)
			return;
				
		array<PR_Role> roleList = {};
		fullRoleList.GetRoleList(roleList);
		
		array<int> roleAvailability = {};
		
		for(int i = 0; i < roleList.Count(); i++)
		{
			roleAvailability.Insert(roleList[i].GetDefaultCount());
		}
		
		groupRoleManagerComponent.InitializeAvailability(roleAvailability);
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Called from PR_PC_PossessionManagerComponent
	void OnPlayerMainCharacterPossessed(IEntity entity)
	{
		if (!entity)
			return;
		
		ScriptedDamageManagerComponent damageManager = ScriptedDamageManagerComponent.Cast(entity.FindComponent(ScriptedDamageManagerComponent));
		if (damageManager)
		{
			damageManager.GetOnDestroyed().Remove(OnPlayerCharacterDestroyed);
			damageManager.GetOnDestroyed().Insert(OnPlayerCharacterDestroyed);
		}
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Called by asset spawner
	void OnAssetSpawned(PR_AssetSpawner spawner, IEntity entity, int factionId)
	{
		ScriptedDamageManagerComponent damageManager = ScriptedDamageManagerComponent.Cast(entity.FindComponent(ScriptedDamageManagerComponent));
		if (damageManager)
			damageManager.GetOnDestroyed().Insert(OnAssetDestroyed);
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Called only for assets spawned by asset spawner
	void OnAssetDestroyed(IEntity entity)
	{
		if(m_eGameModeStage != PR_EGameModeStage.LIVE)
			return;
		
		FactionManager fm = GetGame().GetFactionManager();
		
		// Not character - probably vehicle
		PR_AssetInformerComponent infoComp = PR_AssetInformerComponent.Cast(entity.FindComponent(PR_AssetInformerComponent));
		if (!infoComp)
		{
			_print(string.Format("Didn't find PR_AssetInformerComponent on entity: %1, %2", entity, entity.GetPrefabData().GetPrefabName()));
			return;
		}
		
		int cost = GetAssetCost(infoComp.GetAssetType());
		AddFactionScore(infoComp.GetInitialFactionId(), -cost);
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	protected void OnPlayerCharacterDestroyed(IEntity entity, IEntity killer)
	{
		// Only for Master
		if (!IsMaster())
			return;
		
		// Bail if we are not in live game stage yet
		if(m_eGameModeStage != PR_EGameModeStage.LIVE)
			return;

		FactionAffiliationComponent factionComp = FactionAffiliationComponent.Cast(entity.FindComponent(FactionAffiliationComponent));
		Faction faction = factionComp.GetAffiliatedFaction();
		
		// Bail if friendly fire - prevent losing tickets in that case
		/*
		if (killer)
		{
			FactionAffiliationComponent killerFactionComp = FactionAffiliationComponent.Cast(killer.FindComponent(FactionAffiliationComponent));
			if (killerFactionComp)
			{
				if (killerFactionComp.GetAffiliatedFaction() == faction)
					return;
			}
		}
		*/
		
		// Remove score from faction of player
		FactionManager fm = GetGame().GetFactionManager();
		int factionId = fm.GetFactionIndex(faction);
		
		int cost = GetAssetCost(PR_EAssetType.SOLDIER);
		AddFactionScore(factionId, -cost);
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Returns cost of asset
	static int GetAssetCost(PR_EAssetType assetType)
	{
		switch(assetType)
		{
			case PR_EAssetType.SOLDIER:						return 1;
			case PR_EAssetType.TRANSPORT:					return 4;
			case PR_EAssetType.ARMORED_TRANSPORT:			return 6;
			case PR_EAssetType.TROOP_TRANSPORT:				return 5;
			case PR_EAssetType.ARMED_TRANSPORT:				return 10;
			case PR_EAssetType.FUEL:						return 6;
			case PR_EAssetType.SUPPLY:						return 5;
			case PR_EAssetType.COMMAND:						return 20;
			case PR_EAssetType.ARMORED_PERSONNEL_CARRIER:	return 30;
		}
		return 1;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Utils
	
	//-------------------------------------------------------------------------------------------------------------------------------
	protected static void _print(string str, LogLevel logLevel = LogLevel.NORMAL)
	{
		Print(string.Format("[PR_GameMode] %1", str), logLevel);
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Replication
	

	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Debugging
	
	void InitDiagMenu()
	{
		const string cheatMenuName = "Project Refine";
		
		DiagMenu.RegisterMenu(SCR_DebugMenuID.REFINE_MENU_ID, cheatMenuName, "");
		
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_SHOW_CAPTURE_AREA_STATE, "",		"Show Capture Areas", cheatMenuName);
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_SHOW_ASSET_SPAWNER_STATE, "",		"Show Asset Spawners", cheatMenuName);
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_SHOW_SPAWN_POINT_STATE, "", 		"Show Spawn Points", cheatMenuName);
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_SHOW_BUILDING_PROVIDER_STATE, "", 	"Show Building Providers", cheatMenuName);
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_SHOW_SUPPLY_HOLDER_INFO, "", 		"Show Supply Holders", cheatMenuName);
		
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_GAME_MODE_PANEL, "", 				"Open Game Mode Panel", cheatMenuName);
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_OPEN_BUILDING_DISPLAY, "", 		"Open Building Display", cheatMenuName);
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_OPEN_DEPLOYMENT_MENU, "", 			"Open Deployment Menu", cheatMenuName);
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_OPEN_FACTION_MENU, "", 			"Open Faction Menu", cheatMenuName);
		
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_DISABLE_DEPLOYMENT_COUNTDOWN, "", 	"Disable Depl. Countdown", cheatMenuName);
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_DISABLE_AUTO_DEPLOYMENT_MENU, "", 	"Disable Auto Depl. Menu", cheatMenuName);
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_DISABLE_PREPARATION_STAGE, "", 	"Disable Preparation Stage", cheatMenuName);
		
		
		DiagMenu.RegisterRange(SCR_DebugMenuID.REFINE_HOLDER_IDX, "", 					"Supply: Holder IDX", cheatMenuName, "0, 128, 0, 1");
		DiagMenu.RegisterRange(SCR_DebugMenuID.REFINE_TARGET_IDX, "", 					"Supply: Target IDX", cheatMenuName, "0, 128, 0, 1");
		DiagMenu.RegisterRange(SCR_DebugMenuID.REFINE_SUPPLY_AMOUNT, "", 				"Supply: Amount", cheatMenuName, "0, 1000, 100, 100");
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_GIVE_SUPPLY, "", 					"Supply: Give", cheatMenuName);
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_TAKE_SUPPLY, "", 					"Supply: Take", cheatMenuName);
		
	}
	
	void DrawGameModePanel()
	{
		DbgUI.Begin("Game Mode Panel", 100, 100);
		
		FactionManager fm = GetGame().GetFactionManager();
		
		array<int> factionIds = {m_iFaction0, m_iFaction1};
		for (int i = 0; i < 2; i++)
		{
			int factionId = factionIds[i];
			DbgUI.Text(string.Format("%1 score: %2", fm.GetFactionByIndex(factionId).GetFactionKey(), GetFactionScore(factionId)));
		}
		
		DbgUI.Text(string.Format("Total Time Elapsed: %1", m_fTimeElapsed.ToString(8, 3)));
		DbgUI.Text(string.Format("Stage: %1", typename.EnumToString(PR_EGameModeStage, m_eGameModeStage)));
		
		DbgUI.Text(" ");
		
		DbgUI.End();
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	override void EOnDiag(IEntity owner, float timeSlice)
	{
		if (DiagMenu.GetBool(SCR_DebugMenuID.REFINE_GAME_MODE_PANEL))
			DrawGameModePanel();
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.REFINE_OPEN_DEPLOYMENT_MENU))
		{
			// Assign faction to all players
			array<int>  allPlayers = {};
			GetGame().GetPlayerManager().GetAllPlayers(allPlayers);
			foreach (int playerId : allPlayers)
			{
				SCR_RespawnSystemComponent.GetInstance().SetPlayerFaction(playerId, 0);
			}
			
			RpcDo_DiagOpenMenu(ChimeraMenuPreset.RefineDeploymentMenu);
			Rpc(RpcDo_DiagOpenMenu, ChimeraMenuPreset.RefineDeploymentMenu);
			
			DiagMenu.SetValue(SCR_DebugMenuID.REFINE_OPEN_DEPLOYMENT_MENU, 0);
		}
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.REFINE_OPEN_FACTION_MENU))
		{
			MenuManager mm = GetGame().GetMenuManager();
			RpcDo_DiagOpenMenu(ChimeraMenuPreset.RefineFactionSelectionMenu);
			Rpc(RpcDo_DiagOpenMenu, ChimeraMenuPreset.RefineFactionSelectionMenu);
			
			DiagMenu.SetValue(SCR_DebugMenuID.REFINE_OPEN_FACTION_MENU, 0);
		}
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.REFINE_OPEN_BUILDING_DISPLAY))
		{
			PR_BuildingDisplay.ActivateBuilding();
			DiagMenu.SetValue(SCR_DebugMenuID.REFINE_OPEN_BUILDING_DISPLAY, 0);
		}
	}
	
	// Just for debugging - opens deployment menu for everyone
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	void RpcDo_DiagOpenMenu(int menuId)
	{	
		MenuManager mm = GetGame().GetMenuManager();
		mm.CloseMenuByPreset(menuId);
		mm.OpenMenu(menuId);
	}
}