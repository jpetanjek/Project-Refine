[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class PR_CaptureAreaClass : ScriptComponentClass
{
	// prefab properties here
};

enum PR_EAreaState
{
	NEUTRAL = 0,
	CONTESTING = 1,
	CAPTURED = 2
}

class PR_CaptureArea : ScriptComponent
{
	const float POINTS_MAX = 100.0; // How many points it needs to have to be captured	
	const float CAPTURE_RATE_PER_CHARACTER = 10.0; // How many points are addded per second per each character
	
	[Attribute("10", UIWidgets.EditBox, desc: "Radius of capture zone")]
	float m_fRadius;
	
	[Attribute("-1", UIWidgets.EditBox, desc: "Owner faction at game start")]
	protected int m_iInitialOwnerFaction;
	
	[Attribute("true", UIWidgets.CheckBox, desc: "True if area is capturable by soldier occupation, false if not.")]
	protected bool m_bCapturable;
	
	// Linked areas in all directions
	protected ref array<PR_CaptureArea> m_aLinkedAreas = {};
	
	
	// Variables related to area capture
	// Some of them are only needed for debugging
	
	protected int m_iCharactersInArea;							// Total amount of characters in area
	protected ref array<int> m_aCharactersFromFactions = {};	// Array with count of characters from each faction which can capture this point
	protected ref array<int> m_aLinkedAreasOwners = {};			// Array with owner faction IDs of each linked area
	protected int m_iDominatingFaction = -1;					// Id of faction which has most characters now.
	protected int m_iDominatingCharacters = 0;					// Amount of characters from faction which has most characters here
	protected int m_iLosingCharacters = 0;						// Amount of characters from second faction which has most characters here
	
	[RplProp()]
	protected int m_iOwnerFaction;								// Current owner faction
	[RplProp()]
	protected PR_EAreaState m_eState = PR_EAreaState.NEUTRAL;	// Current state
	[RplProp()]
	protected float m_fPoints = 0.0;							// Amount of points (capture progress)
	[RplProp()]
	protected int m_iPointsOwnerFaction;						// Faction which owns those points now
	
	//------------------------------------------------------------------------------------------------
	void OnUpdateGameMode(float timeSlice, array<SCR_ChimeraCharacter> charactersInArea)
	{
		//PrintFormat("OnUpdateGameMode: %1", timeSlice);
		m_iCharactersInArea = charactersInArea.Count();
		
		// Check who owns each of our neighbours
		foreach (int i, PR_CaptureArea area : m_aLinkedAreas)
			m_aLinkedAreasOwners[i] = area.m_iOwnerFaction; 
		
		// Count characters from each faction
		FactionManager fm = GetGame().GetFactionManager();
		array<Faction> factions = {};
		fm.GetFactionsList(factions);
		int nFactions = factions.Count();
		if (m_aCharactersFromFactions.Count() != nFactions)
			m_aCharactersFromFactions.Resize(nFactions);
		
		for (int i = 0; i < nFactions; i++)
			m_aCharactersFromFactions[i] = 0;
		
		foreach (SCR_ChimeraCharacter c : charactersInArea)
		{
			Faction f = c.m_pFactionComponent.GetAffiliatedFaction();
			if (!f)
				continue;
			int factionId = factions.Find(f);
			m_aCharactersFromFactions[factionId] = m_aCharactersFromFactions[factionId] + 1;
		}
		
		// Reset to 0 characters from factions which can't capture this
		for (int i = 0; i < nFactions; i++)
		{
			if (!CanBeCapturedByFaction(i))
				m_aCharactersFromFactions[i] = 0;
		}
		
		// Find faction which has biggest and second biggest amount of characters
		// This can support any amount of contesting factions
		int maxCharacters = 0;
		int secondMaxCharacters = 0;
		int winningFactionId = -1;
		for (int i = 0; i < nFactions; i++) // Find max characters
		{
			int nCharacters = m_aCharactersFromFactions[i];
			if (nCharacters > maxCharacters)
			{
				maxCharacters = nCharacters;
				winningFactionId = i;
			}
		}
		for (int i = 0; i < nFactions; i++) // Find second max characters
		{
			int nCharacters = m_aCharactersFromFactions[i];
			if (nCharacters > secondMaxCharacters && i != winningFactionId)
			{
				secondMaxCharacters = nCharacters;
			}
		}
		m_iDominatingFaction = winningFactionId;
		m_iDominatingCharacters = maxCharacters;
		m_iLosingCharacters = secondMaxCharacters;
		
		// Logic of capturing
		UpdateCaptureState(timeSlice);
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	int GetOwnerFaction()
	{
		return m_iOwnerFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateCaptureState(float timeSlice)
	{
		// Bail if non capturable
		if (!m_bCapturable)
			return;
		
		int winnersExcess = m_iDominatingCharacters - m_iLosingCharacters; // How many attackers over defenders

		switch (m_eState)
		{
			case PR_EAreaState.NEUTRAL:
			{
				if (winnersExcess != 0)
				{
					m_iPointsOwnerFaction = m_iDominatingFaction;
					m_fPoints = 0;
					m_eState = PR_EAreaState.CONTESTING;
				}
				break;
			}
			
			case PR_EAreaState.CONTESTING:
			{				
				float captureRate = winnersExcess * timeSlice * CAPTURE_RATE_PER_CHARACTER;
				
				// If the dominant faction is not the one which owns the points, remove points
				if (m_iPointsOwnerFaction != m_iDominatingFaction)
					captureRate = -captureRate;
				
				m_fPoints += captureRate;
				m_fPoints = Math.Clamp(m_fPoints + captureRate, 0, POINTS_MAX);
				
				if (m_fPoints == POINTS_MAX)
				{
					// Max points, now captured
					m_iOwnerFaction = m_iPointsOwnerFaction; // New owner is the faction which has been capturing
					m_eState = PR_EAreaState.CAPTURED;
				}
				else if (m_fPoints <= 0.0)
				{
					// Reached zero, now neutral
					m_iPointsOwnerFaction = -1;
					m_iOwnerFaction = -1;
					m_eState = PR_EAreaState.NEUTRAL;
				}
				
				break;
			}
			
			case PR_EAreaState.CAPTURED:
			{
				if (m_iDominatingFaction != m_iOwnerFaction &&
					m_iDominatingFaction != -1 &&
					winnersExcess > 0)
				{
					// Another faction is capturing this
					m_eState = PR_EAreaState.CONTESTING;
				}
				
				break;
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	bool CanBeCapturedByFaction(int factionId)
	{
		// True when any linked area is owned by same faction
		return (factionId != -1) && (m_aLinkedAreasOwners.Find(factionId) != -1);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.DIAG);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		m_iOwnerFaction = m_iInitialOwnerFaction;
		
		m_iPointsOwnerFaction = m_iOwnerFaction;
		if (m_iOwnerFaction != -1)
		{
			m_fPoints = POINTS_MAX;
			m_eState = PR_EAreaState.CAPTURED;
		}
		else
		{
			m_fPoints = 0;
			m_eState = PR_EAreaState.NEUTRAL;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnDiag(IEntity owner, float timeSlice)
	{
		if (DiagMenu.GetBool(SCR_DebugMenuID.REFINE_SHOW_CAPTURE_AREAS_STATE))
		{
			// Draw debug text
			const int COLOR_TEXT = Color.WHITE;
		 	const int COLOR_BACKGROUND = Color.BLACK;
			
			FactionManager fm = GetGame().GetFactionManager();
			
			string strCharsFromFactions;
			foreach (int n : m_aCharactersFromFactions)
				strCharsFromFactions = strCharsFromFactions + string.Format("%1 ", n);
			
			string s;
			s = s + string.Format("Owner:        %1\n", GetFactionKey(m_iOwnerFaction, fm));
			s = s + string.Format("Points owner: %1\n", GetFactionKey(m_iPointsOwnerFaction, fm));
			s = s + string.Format("Dominating:   %1\n", GetFactionKey(m_iDominatingFaction, fm));
			s = s + string.Format("Points: %1\n", m_fPoints);
			s = s + string.Format("State: %1\n", typename.EnumToString(PR_EAreaState, m_eState));
			s = s + string.Format("Factions characters: %1\n", strCharsFromFactions);
			s = s + string.Format("Characters (dom./lose): %1/%2", m_iDominatingCharacters, m_iLosingCharacters);
			
			vector pos = owner.GetOrigin() + Vector(0, 10, 0);
			DebugTextWorldSpace.Create(GetGame().GetWorld(), s, DebugTextFlags.ONCE, pos[0], pos[1], pos[2], size: 13.0, color: COLOR_TEXT, bgColor: COLOR_BACKGROUND);
			
			DrawDebugCylinder();
		}
	}
	string GetFactionKey(int factionId, FactionManager fm)
	{
		if (factionId == -1)
			return string.Empty;
		
		return fm.GetFactionByIndex(factionId).GetFactionKey();
	}
	
	//------------------------------------------------------------------------------------------------
	void InitLinkedAreas(array<PR_CaptureArea> neighbours)
	{
		m_aLinkedAreas.Copy(neighbours);
		m_aLinkedAreasOwners.Resize(m_aLinkedAreas.Count());
		for (int i = 0; i < m_aLinkedAreasOwners.Count(); i++)
		{
			m_aLinkedAreasOwners[i] = m_aLinkedAreas[i].m_iOwnerFaction;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void _WB_AfterWorldUpdate(IEntity owner, float timeSlice)
	{
		DrawDebugCylinder();
	}

	// Draws a cylinder where the area is located at
	protected void DrawDebugCylinder()
	{
		Shape.CreateCylinder(Color.RED, ShapeFlags.VISIBLE | ShapeFlags.ONCE | ShapeFlags.WIREFRAME, GetOwner().GetOrigin(), m_fRadius, 75.0);
	}
	
	//------------------------------------------------------------------------------------------------
	//void PR_CaptureArea(IEntityComponentSource src, IEntity ent, IEntity parent)
	//{
	//}
};
