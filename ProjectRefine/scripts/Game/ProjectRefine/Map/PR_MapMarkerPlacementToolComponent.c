class PR_MapMarkerPlacementToolComponent : ScriptedWidgetComponent
{
	protected ref PR_MapMarkerPlacementToolWidgets widgets = new PR_MapMarkerPlacementToolWidgets();
	
	protected Widget m_wRoot;
	protected vector m_vMarkerPosWorld;
	
	ref ScriptInvoker m_OnMarkerPlacementConfirmed = new ScriptInvoker();
	ref ScriptInvoker m_OnMarkerPlacementCanceled = new ScriptInvoker();
	
	override void HandlerAttached(Widget w)
	{
		widgets.Init(w);
		m_wRoot = w;
		
		widgets.m_OkButtonComponent.m_OnClicked.Insert(OnOkButton);
		widgets.m_CancelButtonComponent.m_OnClicked.Insert(OnCancelButton);
		
		StopMarkerPlacement();
	}
	
	void Update(float timeSlice, SCR_MapEntity mapEntity)
	{
		WorkspaceWidget workspace = GetGame().GetWorkspace();
		
		float worldX = m_vMarkerPosWorld[0];
		float worldY = m_vMarkerPosWorld[1];
		int screenXRef, screenYRef; // Screen position in reference coordinates
		mapEntity.WorldToScreen(worldX, worldY, screenXRef, screenYRef, withPan: true);
		
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
	
	protected void OnOkButton()
	{
		m_OnMarkerPlacementConfirmed.Invoke();
	}
	
	protected void OnCancelButton()
	{
		m_OnMarkerPlacementCanceled.Invoke();
	}
	
	// Returns marker properties entered by user
	void GetMarkerProperties(out vector outMarkerPos, out string outMarkerText)
	{
		outMarkerPos = m_vMarkerPosWorld;
		outMarkerText = widgets.m_MarkerTextEditbox.GetText();
	}
}