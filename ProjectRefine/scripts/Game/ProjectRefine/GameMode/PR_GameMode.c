class PR_GameModeClass : SCR_BaseGameModeClass
{
}

typedef func OnGameModeStageChangedDelegate;
void OnGameModeStageChangedDelegate(PR_EGameModeStage stage);

enum PR_EGameModeStage
{
	PREPARATION,
	LIVE,
	DEBRIF
}

class PR_GameMode : SCR_BaseGameMode
{
	protected static const float GAME_MODE_UPDATE_INTERVAL_S = 1.0;
	
	[Attribute("1", UIWidgets.EditBox, desc: "When true, initial faction owners of areas are randomized")]
	protected bool m_bRandomizeFactions;
	
	// Attributes - areas
	[Attribute(desc: "All areas, including main bases, in their order of capture.")]
	protected ref array<ref PR_EntityLink> m_aAreaEntities;
	
	[Attribute(desc: "Main base of first faction")]
	protected ref PR_EntityLink m_MainBaseEntity0;
	[Attribute(desc: "Main base of second faction")]
	protected ref PR_EntityLink m_MainBaseEntity1;
	
	// Attributes - faction score
	[Attribute("10", UIWidgets.EditBox, "Initial amount of points of first faction")]
	protected float m_fInitialFactionScore0;
	
	[Attribute("10", UIWidgets.EditBox, "Initial amount of points of second faction")]
	protected float m_fInitialFactionScore1;
	
	// Pointers to areas
	protected ref array<PR_CaptureArea> m_aAreas = {}; // Array with all capture areas. It's parallel to m_aAreaEntities array.
	protected PR_CaptureArea m_MainBaseArea0;
	protected PR_CaptureArea m_MainBaseArea1;
	
	// Score of each faction
	protected ref array<float> m_aFactionScore = {}; //!! It's synchronized via replication
	
	// Game mode stage
	protected PR_EGameModeStage m_eGameModeStage = PR_EGameModeStage.PREPARATION;
	
	// Events
	ref ScriptInvokerBase<OnGameModeStageChangedDelegate> m_OnGameModeStageChanged = new ScriptInvokerBase<OnGameModeStageChangedDelegate>();
	
	// Other
	protected float m_fGameModeUpdateTimer = 0;
	protected float m_fGameModeTotalTime = 0;
	protected bool m_bCaptureAreaInitSuccess = false;

	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Public functions
	
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
		
		// Initialize game mode stage
		m_eGameModeStage = PR_EGameModeStage.PREPARATION;
		
		// Randomize factions - if enabled
		FactionManager fm = GetGame().GetFactionManager();
		array<int> factionIds = {};
		factionIds.Resize(fm.GetFactionsCount());
		for (int i = 0; i < fm.GetFactionsCount(); i++)
			factionIds[i] = i;
		if (m_bRandomizeFactions)
		{
			if(Math.RandomInt(0, 2) == 0)
				factionIds.Sort(true); // Reverse whole array
		}
		
		// Init capture areas
		m_bCaptureAreaInitSuccess = true;
		if (m_aAreaEntities)
		{
			foreach (PR_EntityLink link : m_aAreaEntities)
			{
				IEntity captureAreaEntity = link.Init();
				if (captureAreaEntity == null)
				{
					m_bCaptureAreaInitSuccess = false;
					continue;
				}
					
				PR_CaptureArea captureAreaComp = FindCaptureAreaOnEntity(captureAreaEntity);
				if (!captureAreaComp)
				{
					m_bCaptureAreaInitSuccess = false;
					continue;
				}
				
				m_aAreas.Insert(captureAreaComp);
			}
		}
		
		if (m_MainBaseEntity0)
		{
			m_MainBaseEntity0.Init();
			m_MainBaseArea0 = FindCaptureAreaOnEntity(m_MainBaseEntity0.Get());
		}
		if (m_MainBaseEntity1)
		{
			m_MainBaseEntity1.Init();
			m_MainBaseArea1 = FindCaptureAreaOnEntity(m_MainBaseEntity1.Get());
		}
		
		if (!m_MainBaseArea0 || !m_MainBaseArea1)
			m_bCaptureAreaInitSuccess = false;
		
		if (!m_bCaptureAreaInitSuccess)
			_print("Could not initialize capture areas! Game mode will not work!", LogLevel.ERROR);
		else
			_print("Capture areas initialized successfully!", LogLevel.NORMAL);
		
		// Finish init of capture areas:
		// - Link capture areas with their naighbours
		// - Set capture area faction
		if (m_bCaptureAreaInitSuccess)
		{
			for (int i = 0; i < m_aAreas.Count(); i++)
			{
				array<PR_CaptureArea> linked = {};
				if (i >= 1)
					linked.Insert(m_aAreas[i-1]); // Add prev area
				if (i <= m_aAreas.Count()-2)
					linked.Insert(m_aAreas[i+1]); // Add next area
				
				PR_CaptureArea area = m_aAreas[i];
				int initialOwnerFactionId = area.GetInitialOwnerFactionId();
				if (initialOwnerFactionId != -1)
					initialOwnerFactionId = factionIds[initialOwnerFactionId];
				area.Init(linked, initialOwnerFactionId);
			}
		}
		
		// Init faction points
		int nFactions = fm.GetFactionsCount();
		m_aFactionScore.Resize(nFactions);
		if (IsMaster())
		{
			m_aFactionScore[factionIds[0]] = m_fInitialFactionScore0;
			m_aFactionScore[factionIds[1]] = m_fInitialFactionScore1;
		}
		
		if (IsMaster())
		{
			// Subscribe to capture area events
			foreach (PR_CaptureArea area : m_aAreas)
			{
				area.m_OnOwnerFactionChanged.Insert(OnCaptureAreaFactionChanged);
				
				// Subscribe to asset spawner events
				array<PR_AssetSpawner> assetSpawners = area.GetAssetSpawners();
				foreach (PR_AssetSpawner spawner : assetSpawners)
				{
					spawner.m_OnAssetSpawned.Insert(OnAssetSpawned);
				}
			}
		}
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		
		// Update game mode based on timer
		// Only on master!
		if (IsMaster())
		{
			m_fGameModeUpdateTimer += timeSlice;
			m_fGameModeTotalTime += timeSlice;
			if (m_fGameModeUpdateTimer >= GAME_MODE_UPDATE_INTERVAL_S)
			{
				UpdateGameMode(m_fGameModeUpdateTimer);
				m_fGameModeUpdateTimer -= GAME_MODE_UPDATE_INTERVAL_S;
			}
		}
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	override void EOnDiag(IEntity owner, float timeSlice)
	{
		if (DiagMenu.GetBool(SCR_DebugMenuID.REFINE_GAME_MODE_PANEL))
			DrawGameModePanel();
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		InitDiagMenu();
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	override void _WB_AfterWorldUpdate(float timeSlice)
	{
		foreach (auto areaLink : m_aAreaEntities)
		{
			if (areaLink)
				areaLink.Draw(this);
		}
		
		if (m_MainBaseEntity0)
			m_MainBaseEntity0.Draw(this);
		if (m_MainBaseEntity1)
			m_MainBaseEntity1.Draw(this);
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
				TickGameModePlay(timeSlice);
				break;
			}
			case PR_EGameModeStage.DEBRIF:
			{
				TickGameModeDebrif(timeSlice);
				break;
			}
		}
		
		Replication.BumpMe();
	}
	
	void TickGameModePreparation(float timeSlice)
	{
		if(m_fGameModeTotalTime > 30)
		{
			RequestNextGameModeStage();
		}
	}
	
	void TickGameModePlay(float timeSlice)
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
			{
				factionAreas[ownerFactionId] = factionAreas[ownerFactionId] + 1;
			}
		}
		
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
				AddFactionScore(factionId, -1.0 * timeSlice); // !!! Change score decrease rate!
			}
		}
		
		
		//---------------------------------------------
		// End game if some faction has depleted points
		float minScore = 1.0;
		int factionIdMinScore = -1;
		for (int i = 0; i < nFactions; i++)
		{
			if (m_aFactionScore[i] < minScore)
			{
				minScore = m_aFactionScore[i];
				factionIdMinScore = i;
			}
		}
		
		if (minScore < 0)
		{
			// Some faction has lost all its points
			// End the game
			RequestNextGameModeStage();
		}
	}
	
	void TickGameModeDebrif(float timeSlice)
	{
		array<int> winnerFactions = {}; // All factions except the one which has lowest amount of points
		// Do this only once
		{
			FactionManager fm = GetGame().GetFactionManager();
			array<Faction> factions = {};
			fm.GetFactionsList(factions);
			int nFactions = factions.Count();
			
			float minScore = 1.0;
			int factionIdMinScore = -1;
			for (int i = 0; i < nFactions; i++)
			{
				if (m_aFactionScore[i] < minScore)
				{
					minScore = m_aFactionScore[i];
					factionIdMinScore = i;
				}
			}
			
			
			for (int factionId = 0; factionId < nFactions; factionId++)
			{
				if (factionId != factionIdMinScore)
					winnerFactions.Insert(factionId);				
			}
		}
		
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
				m_eGameModeStage = PR_EGameModeStage.DEBRIF;
				break;
			}
			case PR_EGameModeStage.DEBRIF:
			{
				break;
			}
		}
		m_OnGameModeStageChanged.Invoke(m_eGameModeStage);
	}
	
	PR_EGameModeStage GetGameModeStage()
	{
		return m_eGameModeStage;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Faction score
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Call this to add or remove faction points.
	// points - can be negative or positive
	void AddFactionScore(int factionId, float points)
	{
		if (!IsMaster())
		{
			Print("AddFactionPoints() must be called only on master!", LogLevel.ERROR);
			return;
		}
		float newScore = m_aFactionScore[factionId] + points;
		RpcDo_SetFactionScore(factionId, newScore);
		Rpc(RpcDo_SetFactionScore, factionId, newScore);
		Replication.BumpMe();
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	float GetFactionScore(int factionId)
	{
		return m_aFactionScore[factionId];
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_SetFactionScore(int factionId, float score)
	{
		m_aFactionScore[factionId] = score;
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
	// Assets
	
	//-------------------------------------------------------------------------------------------------------------------------------
	protected override void OnPlayerKilled(int playerId, IEntity player, IEntity killer)
	{
		super.OnPlayerKilled(playerId, player, killer);
		
		if (!IsMaster())
			return;
		
		if(m_eGameModeStage != PR_EGameModeStage.LIVE)
			return;
		
		FactionManager fm = GetGame().GetFactionManager();
		FactionAffiliationComponent factionComp = FactionAffiliationComponent.Cast(player.FindComponent(FactionAffiliationComponent));
		Faction faction = factionComp.GetAffiliatedFaction();
		int factionId = fm.GetFactionIndex(faction);
		
		float cost = GetAssetCost(PR_EAssetType.SOLDIER);
		AddFactionScore(factionId, -cost);
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
		PR_EntityInfoComponent infoComp = PR_EntityInfoComponent.Cast(entity.FindComponent(PR_EntityInfoComponent));
		if (!infoComp)
		{
			_print(string.Format("Didn't find PR_EntityInfoComponent on entity: %1, %2", entity, entity.GetPrefabData().GetPrefabName()));
			return;
		}
		
		float cost = GetAssetCost(infoComp.GetAssetType());
		AddFactionScore(infoComp.GetInitialFactionId(), -cost);
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Returns cost of asset
	static float GetAssetCost(PR_EAssetType assetType)
	{
		switch(assetType)
		{
			case PR_EAssetType.SOLDIER:						return 1.0;
			case PR_EAssetType.TRANSPORT:					return 4.0;
			case PR_EAssetType.ARMORED_TRANSPORT:			return 6.0;
			case PR_EAssetType.TROOP_TRANSPORT:				return 5.0;
			case PR_EAssetType.ARMED_TRANSPORT:				return 10.0;
			case PR_EAssetType.FUEL:						return 6.0;
			case PR_EAssetType.SUPPLY:						return 5.0;
			case PR_EAssetType.COMMAND:						return 20.0;
			case PR_EAssetType.ARMORED_PERSONEL_CARRIER:	return 30.0;
		}
		return 1.0;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Utils
	
	//-------------------------------------------------------------------------------------------------------------------------------
	protected static PR_CaptureArea FindCaptureAreaOnEntity(IEntity ent)
	{
		if (!ent)
			return null;
		
		PR_CaptureArea captureAreaComp = PR_CaptureArea.Cast(ent.FindComponent(PR_CaptureArea));
		if (!captureAreaComp)
		{
			_print(string.Format("Didn't find PR_CaptureArea component on area entity: %1", ent.GetName()), LogLevel.ERROR);
			return null;
		}
		return captureAreaComp;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	protected static void _print(string str, LogLevel logLevel = LogLevel.NORMAL)
	{
		Print(string.Format("[PR_GameMode] %1", str), logLevel);
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Replication
	
	//------------------------------------------------------------------------------------------------
	override bool RplSave(ScriptBitWriter writer)
	{
		super.RplSave(writer);
		
		// Faction score
		int nFactions = m_aFactionScore.Count();
		writer.WriteInt(nFactions);
		for (int i = 0; i < nFactions; i++)
			writer.WriteFloat(m_aFactionScore[i]);
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool RplLoad(ScriptBitReader reader)
	{
		super.RplLoad(reader);
		
		// Faction score
		int nFactions;
		if (!reader.ReadInt(nFactions))
			return false;
		
		if (m_aFactionScore.Count() != nFactions)
			m_aFactionScore.Resize(nFactions);
		for (int i = 0; i < nFactions; i++)
		{
			float score;
			if (!reader.ReadFloat(score))
				return false;
			m_aFactionScore[i] = score;
		}
		
		return true;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Debugging
	
	void InitDiagMenu()
	{
		const string cheatMenuName = "Project Refine";
		
		DiagMenu.RegisterMenu(SCR_DebugMenuID.REFINE_MENU_ID, cheatMenuName, "");
		
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_GAME_MODE_PANEL, "", "Game Mode Panel", cheatMenuName);
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_SHOW_CAPTURE_AREAS_STATE, "", "Capture Areas State", cheatMenuName);
		DiagMenu.RegisterBool(SCR_DebugMenuID.REFINE_SHOW_ASSET_SPAWNER_STATE, "", "Asset Spawner State", cheatMenuName);
	}
	
	void DrawGameModePanel()
	{
		DbgUI.Begin("Game Mode Panel", 100, 100);
		
		FactionManager fm = GetGame().GetFactionManager();
		
		for (int i = 0; i < 2; i++)
		{
			DbgUI.Text(string.Format("%1 score: %2", fm.GetFactionByIndex(i).GetFactionKey(), GetFactionScore(i)));
		}
		
		DbgUI.Text(string.Format("Total time elapsed:%1 Game Mode Stage:%2", m_fGameModeTotalTime ,m_eGameModeStage));
		
		DbgUI.Text(" ");
		
		DbgUI.End();
	}
}