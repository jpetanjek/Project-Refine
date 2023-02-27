class PR_DeploymentMenu : ChimeraMenuBase
{
	protected const ResourceName MAP_CONFIG = "{2D46B52640F70437}Configs/Map/MapConfigDeploymentMenu.conf";
	
	protected ref PR_DeploymentMenuWidgets widgets = new PR_DeploymentMenuWidgets();
	
	
	protected PR_MapUiElementsModule m_MapUiElementsModule;
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		widgets.Init(GetRootWidget());
		
		SCR_MapEntity.GetOnMapOpen().Insert(OnMapOpen);
		
		// Open map
		SCR_MapEntity mapEntity = SCR_MapEntity.GetMapInstance();
		MapConfiguration mapConfigFullscreen = mapEntity.SetupMapConfig(EMapEntityMode.FULLSCREEN, MAP_CONFIG, GetRootWidget() );
		mapEntity.OpenMap(mapConfigFullscreen);
	}
	
	// Called by SCR_MapEntity after map has been opened and initialized. Here we can access map modules.
	protected void OnMapOpen(MapConfiguration mapConfig)
	{
		// We need to call it only once
		SCR_MapEntity.GetOnMapOpen().Remove(OnMapOpen);
		
		m_MapUiElementsModule = PR_MapUiElementsModule.Cast(SCR_MapEntity.GetMapInstance().GetMapModule(PR_MapUiElementsModule));
		
		// Tests
		PR_GameMode gm = PR_GameMode.Cast(GetGame().GetGameMode());
		array<PR_SpawnPoint> spawnPoints = PR_SpawnPoint.GetAll();
		
		foreach (PR_SpawnPoint sp : spawnPoints)
		{
			m_MapUiElementsModule.CreateUiElement("{F2689BBD0CAFEB7B}UI/DeploymentMenu/TestMapUiElement.layout", sp.GetOwner().GetOrigin());
		}
	}
	
	override void OnMenuClose()
	{
		super.OnMenuClose();
		
		SCR_MapEntity.GetOnMapOpen().Remove(OnMapOpen);
		
		SCR_MapEntity mapEntity = SCR_MapEntity.GetMapInstance();
		mapEntity.CloseMap();
	}
	
	override void OnMenuUpdate(float tDelta)
	{
		
		// Activate map context but only when cursor is over the map part of the menu
		float areax, areay, areaw, areah;
		widgets.m_MapInteractionArea.GetScreenPos(areax, areay);
		widgets.m_MapInteractionArea.GetScreenSize(areaw, areah);
		
		int mousex, mousey;
		WidgetManager.GetMousePos(mousex, mousey);
		
		WorkspaceWidget ws = GetGame().GetWorkspace();
		
		if (mousex < (areax + areaw))
			GetGame().GetInputManager().ActivateContext("MapContext");
	}
}