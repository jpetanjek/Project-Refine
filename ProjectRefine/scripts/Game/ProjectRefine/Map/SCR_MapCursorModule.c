[BaseContainerProps()]
modded class SCR_MapCursorModule
{
	protected bool m_bPlacingMarker = false;
	
	protected PR_MapMarkerPlacementToolComponent m_MarkerPlacementToolComponent;
	
	protected Widget m_wTooltipRoot;
	protected TextWidget m_wTooltipText;
	
	protected vector m_vCursorPosWorldPrev; // [x, 0, z]
	
	// Cursor hovering
	protected const static float CURSOR_HOVER_TIMER_THRESHOLD = 0.13;
	protected float m_fHoverTimer = 0;
	protected bool m_bHovering = false;
	
	//------------------------------------------------------------------------------
	// Other
	
	bool CanOpenChat()
	{
		return !m_bPlacingMarker;
	}
	
	//------------------------------------------------------------------------------
	// (De)Initialization
	
	override protected void InitInputs()
	{
		super.InitInputs();
		m_InputManager.AddActionListener("MapAddMarker", EActionTrigger.UP, OnAddMarkerAction);
		m_InputManager.AddActionListener("MapDeleteMarker", EActionTrigger.UP, OnDeleteMarkerAction);
	}
	
	override protected void CleanupInputs()
	{
		super.CleanupInputs();
		m_InputManager.RemoveActionListener("MapAddMarker", EActionTrigger.UP, OnAddMarkerAction);
		m_InputManager.RemoveActionListener("MapDeleteMarker", EActionTrigger.UP, OnDeleteMarkerAction);
	}
	
	override protected void InitWidgets(Widget root)
	{
		super.InitWidgets(root);
		
		Widget markerPlacementToolWidget = root.FindAnyWidget("MarkerPlacementTool");
		if (markerPlacementToolWidget)
		{
			m_MarkerPlacementToolComponent = PR_MapMarkerPlacementToolComponent.Cast(markerPlacementToolWidget.FindHandler(PR_MapMarkerPlacementToolComponent));
			m_MarkerPlacementToolComponent.m_OnMarkerPlacementConfirmed.Insert(OnMarkerPlacementConfirmed);
			m_MarkerPlacementToolComponent.m_OnMarkerPlacementCanceled.Insert(OnMarkerPlacementCanceled);
			m_MarkerPlacementToolComponent.StopMarkerPlacement();
		}
		
		m_wTooltipRoot = root.FindAnyWidget("Tooltip");
		if (m_wTooltipRoot)
		{
			m_wTooltipText = TextWidget.Cast(m_wTooltipRoot.FindAnyWidget("Text"));
			ShowTooltip(false);
		}
	}
	
	override void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);
		m_bPlacingMarker = false;
	}
	
	
	//------------------------------------------------------------------------------
	// Update
	
	override void Update(float timeSlice)
	{
		float worldX, worldZ;
		m_MapEntity.GetMapCursorWorldPosition(worldX, worldZ);
		
		super.Update(timeSlice);
		UpdateMarkerPlacementTool(timeSlice);
		UpdateCursorHover(timeSlice);
		
	}
	
	protected void UpdateMarkerPlacementTool(float timeSlice)
	{
		if (m_bPlacingMarker)
		{
			m_MarkerPlacementToolComponent.Update(timeSlice, m_MapEntity);
		}
		//else
		//{
			//WorkspaceWidget ws = GetGame().GetWorkspace();
			//if (ws.GetFocusedWidget() == 
			//GetGame().GetWorkspace().SetFocusedWidget(null); // To prevent auto focusing on the edit box :/
		//}
	}
	
	
	protected void UpdateCursorHover(float timeSlice)
	{
		vector cursorPosWorld;
		float worldX, worldZ;
		m_MapEntity.GetMapCursorWorldPosition(worldX, worldZ);
		cursorPosWorld[0] = worldX;
		cursorPosWorld[2] = worldZ;
		
		if (cursorPosWorld == m_vCursorPosWorldPrev)
		{
			if (!m_bHovering)
			{
				m_fHoverTimer += timeSlice;
				if (m_fHoverTimer >= CURSOR_HOVER_TIMER_THRESHOLD)
				{
					OnHoverStart();
					m_bHovering = true; // Hover event fires only once
				}
			}
		}
		else
		{
			if (m_bHovering)
				OnHoverEnd();
			
			m_bHovering = false;
			m_fHoverTimer = 0;
		}
		
		m_vCursorPosWorldPrev = cursorPosWorld;
	}
	
	protected void OnHoverStart()
	{
		PR_ActiveMapIconMarker markerUnderCursor = GetMarkerUnderCursor();
		
		if (!markerUnderCursor)
			return;
		
		markerUnderCursor.OnCursorHover(m_MapEntity, this);
	}
	
	protected void OnHoverEnd()
	{
		//Print("On hover end");
		ShowTooltip(false);	
	}
	
	//------------------------------------------------------------------------------
	// Other
	
	protected PR_ActiveMapIconMarker GetMarkerUnderCursor()
	{
		const float distanceThreshold = 16; // If marker is further than this, it's too far away
		
		WorkspaceWidget ws = GetGame().GetWorkspace();
		
		// Find marker under cursor
		int cursorx, cursory;
		GetCursorPosition(cursorx, cursory);
		vector cursorPosScreen = Vector(ws.DPIScale(cursorx), ws.DPIScale(cursory), 0);
		vector markerPosScreen;
		int markerPosx, markerPosy;
		//PrintFormat("Cursor pos: %1", cursorPosScreen);
		
		float minDistance = 99999;
		PR_ActiveMapIconMarker closestMarker = null;
		
		foreach (PR_ActiveMapIconMarker marker : PR_ActiveMapIconMarker.s_aAllMarkers)
		{
			vector markerPosWorld = marker.GetOrigin();
			
			m_MapEntity.WorldToScreen(markerPosWorld[0], markerPosWorld[2], markerPosx, markerPosy, withPan: true);
			markerPosScreen[0] = markerPosx;
			markerPosScreen[1] = markerPosy;
			
			//PrintFormat("Marker %1: screen pos: %2", marker, markerPosScreen);
			
			float distance = vector.Distance(markerPosScreen, cursorPosScreen);
			if (distance > distanceThreshold)
				continue;
			
			if (distance < minDistance)
			{
				minDistance = distance;
				closestMarker = marker;
			}
		}
		
		//PrintFormat("Closest marker: %1", closestMarker);
		
		return closestMarker;
	}
	
	void ShowTooltip(bool show, string text = string.Empty)
	{
		if (!m_wTooltipRoot)
			return;
		
		m_wTooltipRoot.SetVisible(show);
		m_wTooltipText.SetText(text);
		
		WorkspaceWidget ws = GetGame().GetWorkspace();
		int cursorx, cursory;
		GetCursorPosition(cursorx, cursory);
		
		if (show)
			FrameSlot.SetPos(m_wTooltipRoot, cursorx, cursory + 12);
	}
	
	//------------------------------------------------------------------------------
	// Callbacks from inputs
	
	protected void OnAddMarkerAction(float value, EActionTrigger reason)
	{
		if (!m_MarkerPlacementToolComponent)
			return;
		
		// Bail if already placing a marker
		//if (m_bPlacingMarker)
		//	return;
		
		float worldX, worldZ;
		m_MapEntity.GetMapCursorWorldPosition(worldX, worldZ);
		vector markerPosWorld = Vector(worldX, 0, worldZ);
		m_MarkerPlacementToolComponent.StartMarkerPlacement(markerPosWorld);
		
		m_bPlacingMarker = true;
	}
	
	protected void OnDeleteMarkerAction(float value, EActionTrigger reason)
	{
		PR_ActiveMapIconMarker markerUnderCursor = GetMarkerUnderCursor();
		if (!markerUnderCursor)
			return;
		
		PR_ActiveMapIconPlayerControllerComponent.GetLocalInstance().AskDeleteMapMarker(markerUnderCursor);
	}
	
	//------------------------------------------------------------------------------
	// Callbacks from marker placement tool
	
	protected void OnMarkerPlacementConfirmed()
	{
		
		m_bPlacingMarker = false;
		
		// Get data from marker tool
		// Send request to server
		vector markerPos;
		string markerText;
		string markerIconName;
		int markerColor;
		m_MarkerPlacementToolComponent.GetMarkerProperties(markerPos, markerText, markerIconName, markerColor);
		
		PR_ActiveMapIconPlayerControllerComponent.GetLocalInstance().AskAddMapMarker(markerPos, markerText, markerIconName, markerColor);
		
		m_MarkerPlacementToolComponent.StopMarkerPlacement();
	}
	
	protected void OnMarkerPlacementCanceled()
	{
		m_bPlacingMarker = false;
		m_MarkerPlacementToolComponent.StopMarkerPlacement();
	}
}