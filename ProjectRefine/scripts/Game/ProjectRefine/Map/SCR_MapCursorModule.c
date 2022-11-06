[BaseContainerProps()]
modded class SCR_MapCursorModule
{
	protected bool m_bPlacingMarker = false;
	protected vector m_vMarkerWorldPos;
	
	protected PR_MapMarkerPlacementToolComponent m_MarkerPlacementToolComponent;
	
	protected ref array<ref CanvasWidgetCommand> m_MarkerPreviewCommands; 
	protected ref ImageDrawCommand m_MarkerPreviewCommand;
	
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
		}
	}
	
	override void Init()
	{
		super.Init();
		
		if (!m_MarkerPreviewCommand)
		{
			// Init marker preview draw command
			ImageDrawCommand c = new ImageDrawCommand();

			c.m_iColor = 0xFFFFFFFF;
			c.m_fRotation = 0;
			c.m_Pivot = vector.Zero;
			c.m_pTexture = m_MapWidget.LoadTexture("{69406E538F52D49A}UI/Textures/Map/topographicIcons/icons_topographic_map_atlas.edds");
			c.m_Position = Vector(500, 500, 0);
			c.m_Size = Vector(100, 100, 0);
			c.m_iFlags = WidgetFlags.STRETCH;
			
			m_MarkerPreviewCommand = c;
			m_MarkerPreviewCommands = {m_MarkerPreviewCommand};
		}
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
			
			m_MapWidget.SetDrawCommands(m_MarkerPreviewCommands);
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
		
		float worldX, worldY;
		m_MapEntity.GetMapCursorWorldPosition(worldX, worldY);
		vector markerPosWorld = Vector(worldX, worldY, 0);
		m_MarkerPlacementToolComponent.StartMarkerPlacement(markerPosWorld);
		
		m_bPlacingMarker = true;
	}
	
	
	
	//------------------------------------------------------------------------------
	// Callbacks from marker placement tool
	
	protected void OnMarkerPlacementConfirmed()
	{
		m_MarkerPlacementToolComponent.StopMarkerPlacement();
		
		m_bPlacingMarker = false;
		
		// Get data from marker tool
		// Send request to server
	}
	
	protected void OnMarkerPlacementCanceled()
	{
		m_bPlacingMarker = false;
		m_MarkerPlacementToolComponent.StopMarkerPlacement();
	}
}