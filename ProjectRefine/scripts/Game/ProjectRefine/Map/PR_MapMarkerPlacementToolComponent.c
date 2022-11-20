class PR_MapMarkerPlacementToolComponent : ScriptedWidgetComponent
{
	protected ref PR_MapMarkerPlacementToolWidgets widgets = new PR_MapMarkerPlacementToolWidgets();
	
	protected Widget m_wRoot;
	protected vector m_vMarkerPosWorld;
	
	ref ScriptInvoker m_OnMarkerPlacementConfirmed = new ScriptInvoker();
	ref ScriptInvoker m_OnMarkerPlacementCanceled = new ScriptInvoker();
	
	// Icons which will be shown in the ToolSize
	protected static const ResourceName MAP_ICON_IMAGESET = "{D6BEE647C47E3E9E}UI/Map/Icons/MapIcons.imageset";
	protected ref array<string> m_aMarkerIconNames = {
		"small_dot", "small_square", "cross",
		"circle_cross", "circle_info", "circle_warning", "circle_unknown",
		"circle", "eye"
	};
	protected ref array<SCR_ModularButtonComponent> m_aIconSelectionButtons = {};
	protected static int s_iSelectedIconId = -1;
	
	override void HandlerAttached(Widget w)
	{
		widgets.Init(w);
		m_wRoot = w;
		
		widgets.m_OkButtonComponent.m_OnClicked.Insert(OnOkButton);
		widgets.m_CancelButtonComponent.m_OnClicked.Insert(OnCancelButton);
		
		// Init icon grid
		int nIcons = m_aMarkerIconNames.Count();
		int nCols = 6;
		int nRows = nIcons / nCols;
		if (nRows * nCols < nIcons)
			nRows++;
		//for (int i = 0; i < nRows; i++)
		//	grid.SetRowFillWeight(i, 1.0);
		//for (int i = 0; i < nCols; i++)
		//	grid.SetColumnFillWeight(i, 1.0);
		
		int rowId = 0;
		int colId = 0;
		int widgetFlags = WidgetFlags.VISIBLE;
		Widget rowWidget = GetGame().GetWorkspace().CreateWidget(
			WidgetType.HorizontalLayoutWidgetTypeID, widgetFlags, Color.White, 0, widgets.m_IconSelectionVerticalLayout);
		
		for (int iconId = 0; iconId < nIcons; iconId++)
		{
			Widget wIconButton = GetGame().GetWorkspace().CreateWidgets("{EBA716BC1344BDB6}UI/Map/IconSelectionButton.layout", rowWidget);
			//GridSlot.SetRow(wIconButton, rowId);
			//GridSlot.SetColumn(wIconButton, colId);
			
			ImageWidget wIconPreviewImage = ImageWidget.Cast(wIconButton.FindAnyWidget("Image"));
			wIconPreviewImage.LoadImageFromSet(0, MAP_ICON_IMAGESET, m_aMarkerIconNames[iconId]);
			
			SCR_ModularButtonComponent buttonComp = SCR_ModularButtonComponent.Cast(wIconButton.FindHandler(SCR_ModularButtonComponent));
			m_aIconSelectionButtons.Insert(buttonComp);
			buttonComp.m_OnClicked.Insert(OnIconSelectionButton);
			
			colId++;
			if (colId == nCols)
			{
				rowWidget = GetGame().GetWorkspace().CreateWidget(
					WidgetType.HorizontalLayoutWidgetTypeID, widgetFlags, Color.White, 0, widgets.m_IconSelectionVerticalLayout);
								
				colId = 0;
				rowId++;
			}
		}
		
		// Select previously chosen icon, or first icon
		if (s_iSelectedIconId != -1)
			m_aIconSelectionButtons[s_iSelectedIconId].SetToggled(true);
		else
			m_aIconSelectionButtons[0].SetToggled(true);
	}
	
	void Update(float timeSlice, SCR_MapEntity mapEntity)
	{
		WorkspaceWidget workspace = GetGame().GetWorkspace();
		
		float worldX = m_vMarkerPosWorld[0];
		float worldZ = m_vMarkerPosWorld[2];
		int screenXRef, screenYRef; // Screen position in reference coordinates
		mapEntity.WorldToScreen(worldX, worldZ, screenXRef, screenYRef, withPan: true);
		
		FrameSlot.SetPos(m_wRoot, workspace.DPIUnscale(screenXRef), workspace.DPIUnscale(screenYRef));
		//mapEntity.pos
	}
	
	void StartMarkerPlacement(vector markerPosWorld)
	{
		m_vMarkerPosWorld = markerPosWorld;
		
		m_wRoot.SetVisible(true);
		
		widgets.m_MarkerTextEditbox.SetText(string.Empty);
		//GetGame().GetWorkspace().SetFocusedWidget(widgets.m_MarkerTextEditbox);
		//widgets.m_MarkerTextEditbox.ActivateWriteMode();
	}
	
	void StopMarkerPlacement()
	{
		m_wRoot.SetVisible(false);
	}
	
	protected void OnIconSelectionButton(SCR_ModularButtonComponent button)
	{
		int id = m_aIconSelectionButtons.Find(button);
		
		foreach (SCR_ModularButtonComponent comp : m_aIconSelectionButtons)
			comp.SetToggled(false);
		
		button.SetToggled(true);
		s_iSelectedIconId = id;
	}
	
	protected void OnOkButton()
	{
		m_OnMarkerPlacementConfirmed.Invoke();
	}
	
	protected void OnCancelButton()
	{
		m_OnMarkerPlacementCanceled.Invoke();
	}
	
	protected string GetSelectedIconName()
	{
		int nIcons = m_aIconSelectionButtons.Count();
		for (int i = 0; i < nIcons; i++)
		{
			if (m_aIconSelectionButtons[i].GetToggled())
				return m_aMarkerIconNames[i];
		}
		return string.Empty;
	}
	
	// Returns marker properties entered by user
	void GetMarkerProperties(out vector outMarkerPos, out string outMarkerText, out string outMarkerIconName)
	{
		outMarkerPos = m_vMarkerPosWorld;
		outMarkerText = widgets.m_MarkerTextEditbox.GetText();
		outMarkerIconName = GetSelectedIconName();
	}
}