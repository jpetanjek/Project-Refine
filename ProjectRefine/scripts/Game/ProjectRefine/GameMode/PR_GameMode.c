class PR_GameModeClass : SCR_BaseGameModeClass
{
}

class PR_GameMode : SCR_BaseGameMode
{
	[Attribute(desc: "All areas, including main bases, in their order of capture.")]
	protected ref array<ref PR_EntityLink> m_aAreaEntities;
	
	
	[Attribute(desc: "Main base of first faction")]
	protected ref PR_EntityLink m_MainBaseEntity0;
	[Attribute(desc: "Main base of second faction")]
	protected ref PR_EntityLink m_MainBaseEntity1;
	
	protected ref array<PR_CaptureArea> m_aAreas = {}; // Array with all capture areas. It's parallel to m_aAreaEntities array.
	protected PR_CaptureArea m_MainBaseArea0;
	protected PR_CaptureArea m_MainBaseArea1;
	
	protected bool m_bCaptureAreaInitSuccess = false;
	
	//-------------------------------------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
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
	}
	
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
	protected static void _print(string str, LogLevel logLevel = LogLevel.NORMAL)
	{
		Print(string.Format("[PR_GameMode] %1", str), logLevel);
	}
}