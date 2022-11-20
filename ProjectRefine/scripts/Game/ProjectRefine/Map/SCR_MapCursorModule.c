[BaseContainerProps()]
modded class SCR_MapCursorModule
{
	protected bool m_bPlacingMarker = false;
	
	protected PR_MapMarkerPlacementToolComponent m_MarkerPlacementToolComponent;
	
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
		m_InputManager.AddActionListener("MapAddMarker", EActionTrigger.UP, OnInputAddMarker);
	}
	
	override protected void CleanupInputs()
	{
		super.CleanupInputs();
		m_InputManager.RemoveActionListener("MapAddMarker", EActionTrigger.UP, OnInputAddMarker);
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
		super.Update(timeSlice);
		HandleMarkerPlacementTool(timeSlice);
	}
	
	protected void HandleMarkerPlacementTool(float timeSlice)
	{
		if (m_bPlacingMarker)
		{
			m_MarkerPlacementToolComponent.Update(timeSlice, m_MapEntity);
		}
	}
	
	
	
	//------------------------------------------------------------------------------
	// Callbacks from inputs
	
	protected void OnInputAddMarker(float value, EActionTrigger reason)
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