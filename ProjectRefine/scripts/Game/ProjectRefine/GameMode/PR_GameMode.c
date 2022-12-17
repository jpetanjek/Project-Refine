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
	
	// Pointers to areas
	protected ref array<PR_CaptureArea> m_aAreas = {}; // Array with all capture areas. It's parallel to m_aAreaEntities array.
	protected PR_CaptureArea m_MainBaseArea0;
	protected PR_CaptureArea m_MainBaseArea1;
	
	protected bool m_bCaptureAreaInitSuccess = false;
	
	protected float m_fGameModeUpdateTimer = 0;
	
	
	
	
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
}