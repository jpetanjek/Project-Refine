class PR_GameModeClass : SCR_BaseGameModeClass
{
}

class PR_GameMode : SCR_BaseGameMode
{
	protected static const float GAME_MODE_UPDATE_INTERVAL_S = 1.0;
	
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
	
	
	
	
	// Other
	protected float m_fGameModeUpdateTimer = 0;
	protected bool m_bCaptureAreaInitSuccess = false;
	
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
		
		// Link capture ares with their naighbours
		// Now we just link them sequentially
		if (m_bCaptureAreaInitSuccess)
		{
			for (int i = 0; i < m_aAreas.Count(); i++)
			{
				array<PR_CaptureArea> linked = {};
				if (i >= 1)
					linked.Insert(m_aAreas[i-1]); // Add prev area
				if (i <= m_aAreas.Count()-2)
					linked.Insert(m_aAreas[i+1]); // Add next area
				m_aAreas[i].InitLinkedAreas(linked);
			}
		}
		
		// Init faction points
		FactionManager fm = GetGame().GetFactionManager();
		int nFactions = fm.GetFactionsCount();
		m_aFactionScore.Resize(nFactions);
		m_aFactionScore[0] = m_fInitialFactionScore0;
		m_aFactionScore[1] = m_fInitialFactionScore1;
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		
		// Update game mode based on timer
		// Only on server!
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
			int ownerFactionId = area.GetOwnerFaction();
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
				AddFactionScore(factionId, -1.0 * timeSlice);
			}
		}
		
		Replication.BumpMe();
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
			Print("AddFactionPoints() must be called only on server!", LogLevel.ERROR);
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
	}
	
	void DrawGameModePanel()
	{
		DbgUI.Begin("Game Mode Panel", 100, 100);
		
		FactionManager fm = GetGame().GetFactionManager();
		
		for (int i = 0; i < 2; i++)
		{
			DbgUI.Text(string.Format("%1 score: %2", fm.GetFactionByIndex(i).GetFactionKey(), GetFactionScore(i)));
		}
		
		DbgUI.Text(" ");
		
		DbgUI.End();
	}
}