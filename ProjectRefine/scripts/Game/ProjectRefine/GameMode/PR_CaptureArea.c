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
	
	[Attribute("", UIWidgets.EditBox, desc: "Name of the area")]
	protected string m_sName;
	
	[Attribute("-1", UIWidgets.EditBox, desc: "Owner faction at game start")]
	protected int m_iInitialOwnerFaction;
	
	[Attribute("true", UIWidgets.CheckBox, desc: "True if area is capturable by soldier occupation, false if not.")]
	protected bool m_bCapturable;
	
	// Called whenever any of state variables changes. It's not associated to m_eState only!
	ref ScriptInvoker<PR_CaptureArea> m_OnAnyPropertyChanged = new ScriptInvoker<PR_CaptureArea>();
	
	// Called whenever owner faction changes.
	// !!! Works only on server!
	ref ScriptInvoker<PR_CaptureArea, int, int> m_OnOwnerFactionChanged = new ScriptInvoker<PR_CaptureArea, int, int>(); // Old faction, new faction
	
	// Called when links are initialized, for each added link
	ref ScriptInvoker<PR_CaptureArea, PR_CaptureArea> m_OnLinkAdded = new ScriptInvoker<PR_CaptureArea, PR_CaptureArea>();
	
	// Linked areas in all directions
	protected ref array<PR_CaptureArea> m_aLinkedAreas = {};
	
	// Asset spawners in this capture area
	protected ref array<PR_AssetSpawner> m_aAssetSpawners = {};
	
	// Spawn point attached to this capture area
	protected SCR_SpawnPoint m_SpawnPoint;
	protected ref SCR_UIInfo m_SpawnPointUiInfo;
	
	// Preparation phase collider
	PR_PreparationPhaseCollider m_Collider;
	[RplProp(onRplName: "OnCollisionChanged")]
	bool m_bColliderDisabled = false;
	
	// Variables related to area capture
	// Some of them are only needed for debugging
	
	protected int m_iCharactersInArea;							// Total amount of characters in area
	protected ref array<int> m_aCharactersFromFactions = {};	// Array with count of characters from each faction which can capture this point
	protected int m_iDominatingFaction = -1;					// Id of faction which has most characters now.
	protected int m_iDominatingCharacters = 0;					// Amount of characters from faction which has most characters here
	protected int m_iLosingCharacters = 0;						// Amount of characters from second faction which has most characters here
	
	[RplProp(onRplName: "OnRplPropChanged")]
	protected int m_iOwnerFaction;								// Current owner faction
	[RplProp(onRplName: "OnRplPropChanged")]
	protected PR_EAreaState m_eState = PR_EAreaState.NEUTRAL;	// Current state
	[RplProp(onRplName: "OnRplPropChanged")]
	protected float m_fPoints = 0.0;							// Amount of points (capture progress)
	[RplProp(onRplName: "OnRplPropChanged")]
	protected int m_iPointsOwnerFaction;						// Faction which owns those points now
	
	//------------------------------------------------------------------------------------------------
	void OnUpdateGameMode(float timeSlice, array<SCR_ChimeraCharacter> charactersInArea)
	{
		if(!m_bColliderDisabled)
		{
			m_bColliderDisabled = true;
			
			IEntity childEntity = GetOwner().GetChildren();
			
			// Check for preparation phase collider
			childEntity = GetOwner().GetChildren();
			while (childEntity)
			{
				PR_PreparationPhaseCollider collider = PR_PreparationPhaseCollider.Cast(childEntity);
				if (collider)
				{
					collider.GetPhysics().Destroy();
					break;
				}
				childEntity = childEntity.GetSibling();
			}
		}
		
		//PrintFormat("OnUpdateGameMode: %1", timeSlice);
		m_iCharactersInArea = charactersInArea.Count();
		
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
	
	void OnCollisionChanged()
	{
		if(m_bColliderDisabled)
		{			
			IEntity childEntity = GetOwner().GetChildren();
			
			// Check for preparation phase collider
			childEntity = GetOwner().GetChildren();
			while (childEntity)
			{
				PR_PreparationPhaseCollider collider = PR_PreparationPhaseCollider.Cast(childEntity);
				if (collider)
				{
					collider.GetPhysics().Destroy();
					break;
				}
				childEntity = childEntity.GetSibling();
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	int GetOwnerFactionId()
	{
		return m_iOwnerFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetInitialOwnerFactionId()
	{
		return m_iInitialOwnerFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetPointsOwnerFactionId()
	{
		return m_iPointsOwnerFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	// Returns amount of points in relative scale, from 0 to 1.0
	float GetPointsRelative()
	{
		return m_fPoints / POINTS_MAX;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetName()
	{
		return m_sName;
	}
	
	//------------------------------------------------------------------------------------------------
	array<PR_AssetSpawner> GetAssetSpawners()
	{
		array<PR_AssetSpawner> a = {};
		a.Copy(m_aAssetSpawners);
		return a;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateCaptureState(float timeSlice)
	{
		bool invokeOnStateChanged = false;
		
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
					invokeOnStateChanged = true;
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
					
					m_OnOwnerFactionChanged.Invoke(this, -1, m_iOwnerFaction);
					
					SetSpawnPointOwnerFaction(m_iOwnerFaction);
				}
				else if (m_fPoints <= 0.0)
				{
					// Reached zero, now neutral
					int prevOwnerFaction = m_iOwnerFaction;
					m_iPointsOwnerFaction = -1;
					m_iOwnerFaction = -1;
					m_eState = PR_EAreaState.NEUTRAL;
					
					m_OnOwnerFactionChanged.Invoke(this, prevOwnerFaction, -1);
					
					SetSpawnPointOwnerFaction(-1);
				}
				
				invokeOnStateChanged = true;
				
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
					invokeOnStateChanged = true;
				}
				
				break;
			}
		}
		
		if (invokeOnStateChanged)
			m_OnAnyPropertyChanged.Invoke(this);
	}
	
	//------------------------------------------------------------------------------------------------
	// Called by Replication on client whenever anything changes
	void OnRplPropChanged()
	{
		m_OnAnyPropertyChanged.Invoke(this);
	}

	//------------------------------------------------------------------------------------------------
	bool CanBeCapturedByFaction(int factionId)
	{
		if (factionId == -1 || !m_bCapturable)
			return false;
		
		// There must be at least one linked area owned by same faction
		foreach (PR_CaptureArea area : m_aLinkedAreas)
		{
			if (area.m_iOwnerFaction == factionId)
				return true;
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.DIAG);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
	
	//------------------------------------------------------------------------------------------------
	// Called by game mode
	void Init(array<PR_CaptureArea> neighbourAreas, int ownerFactionId)
	{
		RplComponent rpl = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		
		FactionManager fm = GetGame().GetFactionManager();
		
		// Init linked areas
		m_aLinkedAreas.Copy(neighbourAreas);
		
		foreach (PR_CaptureArea area : neighbourAreas)
		{
			m_OnLinkAdded.Invoke(this, area);
		}
		
		// Init owner faction - only for Master
		// If it's a proxy, owner faction is initialized from replication
		if (rpl.IsMaster())
		{
			m_iOwnerFaction = ownerFactionId;
			
			m_iPointsOwnerFaction = m_iOwnerFaction;
			if (m_iOwnerFaction != -1)
			{
				m_fPoints = POINTS_MAX;
				m_eState = PR_EAreaState.CAPTURED;
				
				m_OnOwnerFactionChanged.Invoke(this, -1, m_iOwnerFaction); 
			}
			else
			{
				m_fPoints = 0;
				m_eState = PR_EAreaState.NEUTRAL;
			}
			m_OnAnyPropertyChanged.Invoke(this);
		}
		
		// Init asset spawners
		IEntity childEntity = GetOwner().GetChildren();
		while (childEntity)
		{
			PR_AssetSpawner assetSpawner = PR_AssetSpawner.Cast(childEntity);
			if (assetSpawner)
			{
				assetSpawner.Init(this);
				m_aAssetSpawners.Insert(assetSpawner);
			}
			
			childEntity = childEntity.GetSibling();
		}
		
		// Init spawn point
		InitSpawnPoint(ownerFactionId);
	}
	
	
	//------------------------------------------------------------------------------------------------
	// Methods related to control of the spawn point attached here
	
	//------------------------------------------------------------------------------------------------
	protected void InitSpawnPoint(int ownerFactionId)
	{
		RplComponent rpl = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		
		// Find the spawn point entity among children
		IEntity childEntity = GetOwner().GetChildren();
		m_SpawnPoint = null;
		while (childEntity)
		{
			m_SpawnPoint = SCR_SpawnPoint.Cast(childEntity);
			if (m_SpawnPoint)
				break;
			childEntity = childEntity.GetSibling();
		}
		
		if (m_SpawnPoint)
		{
			// Set initial owner faction - only for Master
			if (rpl.IsMaster())
				SetSpawnPointOwnerFaction(ownerFactionId);
			
			// Set UIInfo
			m_SpawnPointUiInfo = SCR_UIInfo.CreateInfo(m_sName);
			m_SpawnPoint.LinkInfo(m_SpawnPointUiInfo);
			
			// Search for PR_CharacterSpawnPosition entities and pass them to spawn point
			childEntity = GetOwner().GetChildren();
			int nCharSpawnPosAdded = 0;
			while (childEntity)
			{
				PR_CharacterSpawnPosition playerSpawnPos = PR_CharacterSpawnPosition.Cast(childEntity);
				if (playerSpawnPos)
				{
					m_SpawnPoint.AddChildPosition(playerSpawnPos);
					nCharSpawnPosAdded++;
				}
				childEntity = childEntity.GetSibling();
			}
			if (nCharSpawnPosAdded == 0)
				_print("No PR_CharacterSpawnPosition found!", LogLevel.ERROR);
		}
		else
			_print("No SCR_SpawnPoint found", LogLevel.ERROR);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SetSpawnPointOwnerFaction(int ownerFactionId)
	{
		m_SpawnPoint.SetFaction(GetGame().GetFactionManager().GetFactionByIndex(ownerFactionId));
	}
	
	
	
	//------------------------------------------------------------------------------------------------
	// Misc
	
	
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
	override void _WB_AfterWorldUpdate(IEntity owner, float timeSlice)
	{
		DrawDebugCylinder();
	}

	// Draws a cylinder where the area is located at
	protected void DrawDebugCylinder()
	{
		Shape.CreateCylinder(Color.RED, ShapeFlags.VISIBLE | ShapeFlags.ONCE | ShapeFlags.WIREFRAME, GetOwner().GetOrigin(), m_fRadius, 75.0);
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	protected void _print(string str, LogLevel logLevel = LogLevel.NORMAL)
	{
		Print(string.Format("[PR_CaptureArea] %1: %2", m_sName, str), logLevel);
	}
	
	//------------------------------------------------------------------------------------------------
	//void PR_CaptureArea(IEntityComponentSource src, IEntity ent, IEntity parent)
	//{
	//}
};
