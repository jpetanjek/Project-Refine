enum PR_EMarkerVisibility
{
	EVERYONE,
	FACTION
}

class PR_MapMarkerPlacementToolComponent : ScriptedWidgetComponent
{
	protected ref PR_MapMarkerPlacementToolWidgets widgets = new PR_MapMarkerPlacementToolWidgets();
	
	protected Widget m_wRoot;
	protected vector m_vMarkerPosWorld;
	
	ref ScriptInvoker m_OnMarkerPlacementConfirmed = new ScriptInvoker();
	ref ScriptInvoker m_OnMarkerPlacementCanceled = new ScriptInvoker();
	
	// Icons which will be shown in the Tool
	protected static const ResourceName MAP_ICON_IMAGESET = "{D6BEE647C47E3E9E}UI/Map/Icons/MapIcons.imageset";
	protected static ref array<string> s_aMarkerIconNames = {
		"small_dot", "small_square", "cross",
		"circle_cross", "circle_info", "circle_warning", "circle_unknown",
		"circle", "eye",
		"arrow_up", "arrow_left", "arrow_down", "arrow_right"
	};
	protected ref array<SCR_ModularButtonComponent> m_aIconSelectionButtons = {};
	protected static int s_iSelectedIconId = -1;
	
	// Colors which will be shown in the tool
	protected static ref array<int> s_aMarkerColors = {
		0xFFC20000,	// Red
		0xff008000,	// Green
		0xFF0003cf,	// Blue
		0xff8000ff,	// Violet
		Color.WHITE,
		Color.BLACK,
	};
	protected ref array<SCR_ModularButtonComponent> m_aColorSelectionButtons = {};
	protected static int s_iSelectedColorId = -1;
	
	protected static int s_iSelectedVisibilityId = -1;
	
	
	override void HandlerAttached(Widget w)
	{
		widgets.Init(w);
		m_wRoot = w;
		
		widgets.m_OkButtonComponent.m_OnClicked.Insert(OnOkButton);
		widgets.m_CancelButtonComponent.m_OnClicked.Insert(OnCancelButton);
		
		InputManager im = GetGame().GetInputManager();
		im.AddActionListener("MapConfirmAddMarker", EActionTrigger.UP, OnConfirmAction);
		
		
		InitIconGrid();
		InitColorGrid();
		InitVisibilityListbox();
	}
	
	override void HandlerDeattached(Widget w)
	{
		InputManager im = GetGame().GetInputManager();
		im.RemoveActionListener("MapConfirmAddMarker", EActionTrigger.UP, OnConfirmAction);
	}
	
	void InitIconGrid()
	{
		const int nCols = 6; // Columns
		
		int nIcons = s_aMarkerIconNames.Count();
		int nRows = nIcons / nCols;
		if (nRows * nCols < nIcons)
			nRows++;
		
		int rowId = 0;
		int colId = 0;
		int widgetFlags = WidgetFlags.VISIBLE;
		Widget rowWidget = GetGame().GetWorkspace().CreateWidget(
			WidgetType.HorizontalLayoutWidgetTypeID, widgetFlags, Color.White, 0, widgets.m_IconSelectionVerticalLayout);
		
		for (int iconId = 0; iconId < nIcons; iconId++)
		{
			Widget wIconButton = GetGame().GetWorkspace().CreateWidgets("{EBA716BC1344BDB6}UI/Map/IconSelectionButton.layout", rowWidget);
			
			ImageWidget wIconPreviewImage = ImageWidget.Cast(wIconButton.FindAnyWidget("Image"));
			wIconPreviewImage.LoadImageFromSet(0, MAP_ICON_IMAGESET, s_aMarkerIconNames[iconId]);
			
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
	
	void InitColorGrid()
	{
		const int nCols = 3; // Columns
		
		int nItems = s_aMarkerColors.Count();
		int nRows = nItems / nCols;
		if (nRows * nCols < nItems)
			nRows++;
		
		int rowId = 0;
		int colId = 0;
		int widgetFlags = WidgetFlags.VISIBLE;
		Widget rowWidget = GetGame().GetWorkspace().CreateWidget(
			WidgetType.HorizontalLayoutWidgetTypeID, widgetFlags, Color.White, 0, widgets.m_ColorSelectionVerticalLayout);
		
		for (int i = 0; i < nItems; i++)
		{
			Widget wIconButton = GetGame().GetWorkspace().CreateWidgets("{116A6A70CEEB7CF7}UI/Map/ColorSelectionButton.layout", rowWidget);
			
			ImageWidget wImage = ImageWidget.Cast(wIconButton.FindAnyWidget("Image"));
			Color color = Color.FromIntSRGB(s_aMarkerColors[i]);
			wImage.SetColor(color);
			
			SCR_ModularButtonComponent buttonComp = SCR_ModularButtonComponent.Cast(wIconButton.FindHandler(SCR_ModularButtonComponent));
			m_aColorSelectionButtons.Insert(buttonComp);
			buttonComp.m_OnClicked.Insert(OnColorSelectionButton);
			
			colId++;
			if (colId == nCols)
			{
				rowWidget = GetGame().GetWorkspace().CreateWidget(
					WidgetType.HorizontalLayoutWidgetTypeID, widgetFlags, Color.White, 0, widgets.m_ColorSelectionVerticalLayout);
								
				colId = 0;
				rowId++;
			}
		}
		
		// Select previously chosen icon, or first icon
		if (s_iSelectedColorId != -1)
			m_aColorSelectionButtons[s_iSelectedColorId].SetToggled(true);
		else
			m_aColorSelectionButtons[0].SetToggled(true);
	}
	
	void InitVisibilityListbox()
	{
		SCR_ListBoxComponent lb = widgets.m_VisibilityListboxComponent;
		
		lb.AddItem("My Faction");
		lb.AddItem("Everyone");
		
		lb.m_OnChanged.Insert(OnVisibilityListbox);
	}
	
	void UpdateVisibilityListboxAvailableItems()
	{
		SCR_ListBoxComponent lb = widgets.m_VisibilityListboxComponent;
		
		// Check if we have our own faction. If we don't then disable the 'my faction' option.
		bool haveOwnFaction = false;
		PlayerController pc = GetGame().GetPlayerController();
		if (pc)
		{
			haveOwnFaction = pc.GetControlledEntity() != null;
			if (!haveOwnFaction)
			{
				SCR_RespawnSystemComponent respawnSystem = SCR_RespawnSystemComponent.GetInstance();
				SCR_PlayerRespawnInfo playerRespawnInfo = respawnSystem.FindPlayerRespawnInfo(pc.GetPlayerId());
				if (playerRespawnInfo)
					haveOwnFaction = playerRespawnInfo.GetPlayerFactionIndex() >= 0;
			}
		}	
		
		int defaultSelection = 0;
		bool myFactionOptionEnabled = true;
		
		if (!haveOwnFaction)
		{
			myFactionOptionEnabled = false;
			defaultSelection = 1;
		}
		else
		{
			if (s_iSelectedVisibilityId != -1)
				defaultSelection = s_iSelectedVisibilityId;
			else
				defaultSelection = 0;
			
			myFactionOptionEnabled = true;
		}

		lb.GetElementComponent(0).SetEnabled(myFactionOptionEnabled);
		lb.SetAllItemsSelected(false);
		lb.SetItemSelected(defaultSelection, true);
	}
	
	void Update(float timeSlice, SCR_MapEntity mapEntity)
	{
		WorkspaceWidget workspace = GetGame().GetWorkspace();
		
		float worldX = m_vMarkerPosWorld[0];
		float worldZ = m_vMarkerPosWorld[2];
		int screenXRef, screenYRef; // Screen position in reference coordinates
		mapEntity.WorldToScreen(worldX, worldZ, screenXRef, screenYRef, withPan: true);
		
		FrameSlot.SetPos(m_wRoot, workspace.DPIUnscale(screenXRef), workspace.DPIUnscale(screenYRef));
	}
	
	void StartMarkerPlacement(vector markerPosWorld)
	{
		m_vMarkerPosWorld = markerPosWorld;
		
		m_wRoot.SetVisible(true);
		
		widgets.m_MarkerTextEditBoxComponent.SetValue(string.Empty);
		
		UpdateVisibilityListboxAvailableItems();
		
		GetGame().GetWorkspace().SetFocusedWidget(widgets.m_OkButton);
		//GetGame().GetWorkspace().SetFocusedWidget(widgets.m_MarkerTextEditbox);
		//widgets.m_MarkerTextEditbox.ActivateWriteMode();
	}
	
	void StopMarkerPlacement()
	{
		m_wRoot.SetVisible(false);
	}
	
	void OnVisibilityListbox(SCR_ListBoxComponent comp, int item, bool newSelected)
	{
		s_iSelectedVisibilityId = comp.GetSelectedItem();
	}
	
	protected void OnIconSelectionButton(SCR_ModularButtonComponent button)
	{
		int id = m_aIconSelectionButtons.Find(button);
		
		foreach (SCR_ModularButtonComponent comp : m_aIconSelectionButtons)
			comp.SetToggled(false);
		
		button.SetToggled(true);
		s_iSelectedIconId = id;
	}
	
	protected void OnColorSelectionButton(SCR_ModularButtonComponent button)
	{
		int id = m_aColorSelectionButtons.Find(button);
		
		foreach (auto comp : m_aColorSelectionButtons)
			comp.SetToggled(false);
		
		button.SetToggled(true);
		s_iSelectedColorId = id;
	}
	
	protected void OnConfirmAction()
	{
		m_OnMarkerPlacementConfirmed.Invoke();
		GetGame().GetWorkspace().SetFocusedWidget(null); // Otherwise game focuses on invisible text edit box
	}
	
	protected void OnOkButton()
	{
		m_OnMarkerPlacementConfirmed.Invoke();
		GetGame().GetWorkspace().SetFocusedWidget(null);
	}
	
	protected void OnCancelButton()
	{
		m_OnMarkerPlacementCanceled.Invoke();
		GetGame().GetWorkspace().SetFocusedWidget(null);
	}
	
	protected string GetSelectedIconName()
	{
		int nIcons = m_aIconSelectionButtons.Count();
		for (int i = 0; i < nIcons; i++)
		{
			if (m_aIconSelectionButtons[i].GetToggled())
				return s_aMarkerIconNames[i];
		}
		return string.Empty;
	}
	
	protected int GetSelectedColor()
	{
		int n = m_aColorSelectionButtons.Count();
		for (int i = 0; i < n; i++)
		{
			if (m_aColorSelectionButtons[i].GetToggled())
				return s_aMarkerColors[i];
		}
		return Color.PINK;
	}
	
	// Returns marker properties entered by user
	void GetMarkerProperties(out vector outMarkerPos, out string outMarkerText, out string outMarkerIconName,
		out int outMarkerColor, out PR_EMarkerVisibility outVisibility)
	{
		outMarkerPos = m_vMarkerPosWorld;
		outMarkerText = widgets.m_MarkerTextEditBoxComponent.GetValue();
		outMarkerIconName = GetSelectedIconName();
		outMarkerColor = GetSelectedColor();
		
		PR_EMarkerVisibility v = -1;
		switch (widgets.m_VisibilityListboxComponent.GetSelectedItem())
		{
			case 0: v = PR_EMarkerVisibility.FACTION; break;
			case 1: v = PR_EMarkerVisibility.EVERYONE; break;
		}
		outVisibility = v;
	}
}