// Small class for UI to keep track of spawn points
class PR_DeploymentMenu_SpawnPointData : Managed
{
	PR_SpawnPoint m_SpawnPoint;
	PR_MapUiElementComponent m_MapUiElement;
	SCR_ModularButtonComponent m_MapUiButton;	// The button component attached to map UI element
}

class PR_DeploymentMenu : ChimeraMenuBase
{
	protected const ResourceName MAP_CONFIG = "{2D46B52640F70437}Configs/Map/MapConfigDeploymentMenu.conf";
	
	protected ref PR_DeploymentMenuWidgets widgets = new PR_DeploymentMenuWidgets();
	
	protected PR_MapUiElementsModule m_MapUiElementsModule;
	
	//-----------------------------------------------------------------------------------------------------------------------------
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
	
	//-----------------------------------------------------------------------------------------------------------------------------
	// Called by SCR_MapEntity after map has been opened and initialized. Here we can access map modules.
	protected void OnMapOpen(MapConfiguration mapConfig)
	{
		// We need to call it only once
		SCR_MapEntity.GetOnMapOpen().Remove(OnMapOpen);
		
		m_MapUiElementsModule = PR_MapUiElementsModule.Cast(SCR_MapEntity.GetMapInstance().GetMapModule(PR_MapUiElementsModule));
		
		InitSpawnPointUi();
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	override void OnMenuClose()
	{
		super.OnMenuClose();
		
		SCR_MapEntity.GetOnMapOpen().Remove(OnMapOpen);
		
		SCR_MapEntity mapEntity = SCR_MapEntity.GetMapInstance();
		mapEntity.CloseMap();
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
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
		
		// Update spawn point seleciton, only when map is ready
		if (m_MapUiElementsModule)
			UpdateSpawnPointUi(tDelta);
	}
	
	
	//-----------------------------------------------------------------------------------------------------------------------------
	// SPAWN POINT SELECTION
	// Everything related to spawn points is here
	
	protected ref array<ref PR_DeploymentMenu_SpawnPointData> m_aSpawnPointsData = {};
	
	//-----------------------------------------------------------------------------------------------------------------------------
	// This function defines if spawn point should be listed in UI
	bool ShowSpawnPointInUi(PR_SpawnPoint spawnPoint, int myFactionId)
	{
		// True when spawn point exists in world
		// and when our faction owns it
		return spawnPoint && spawnPoint.GetFactionId() == myFactionId;
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	// Called from menu update, but only map is already initialized
	void UpdateSpawnPointUi(float tDelta)
	{
		FactionManager fm = GetGame().GetFactionManager();
		Faction myFaction = SCR_RespawnSystemComponent.GetLocalPlayerFaction();
		int myFactionId = 0; // fm.GetFactionIndex(myFaction); //! ! ! ! ! Pretend we have a faction until faction selection is done ! ! ! ! ! ! ! ! ! ! ! ! ! 
		
		//---------------------------------------------------------------------------------
		// Delete unaccessible or deleted spawn points
		array<ref PR_DeploymentMenu_SpawnPointData> spawnPointsRemove = {};
		foreach (PR_DeploymentMenu_SpawnPointData spData : m_aSpawnPointsData)
		{
			if (!ShowSpawnPointInUi(spData.m_SpawnPoint, myFactionId))
				spawnPointsRemove.Insert(spData);
		}
		foreach (PR_DeploymentMenu_SpawnPointData spData : spawnPointsRemove)
		{
			RemoveSpawnPoint(spData);
		}
		
		//---------------------------------------------------------------------------------
		// Add spawn points which are not in UI yet
		PR_GameMode gm = PR_GameMode.Cast(GetGame().GetGameMode());
		array<PR_SpawnPoint> spawnPoints = PR_SpawnPoint.GetAll();
		foreach (PR_SpawnPoint spawnPoint : spawnPoints)
		{	
			// Have we already added that spawn point?
			bool alreadyAdded = false;
			foreach (PR_DeploymentMenu_SpawnPointData spData : m_aSpawnPointsData)
			{
				if (spData.m_SpawnPoint == spawnPoint && spawnPoint)
				{
					alreadyAdded = true;
					break;
				}
			}
			
			// Ignore if we have already added it to our UI
			if (alreadyAdded)
				continue;
			
			if (ShowSpawnPointInUi(spawnPoint, myFactionId))
			{
				AddSpawnPoint(spawnPoint);
			}
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	// Called on menu open, map is not guaranteed to work yet
	void InitSpawnPointUi()
	{
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	void AddSpawnPoint(notnull PR_SpawnPoint spawnPoint)
	{
		Print(string.Format("AddSpawnPoint: %1 %2", spawnPoint, spawnPoint.GetName()));
		
		PR_DeploymentMenu_SpawnPointData spData = new PR_DeploymentMenu_SpawnPointData();
		
		spData.m_SpawnPoint = spawnPoint;
		//spData.m_sSpawnPointName = spawnPoint.GetName();
		
		// Add to map
		PR_MapUiElementComponent mapUiElement = m_MapUiElementsModule.CreateUiElement("{F2689BBD0CAFEB7B}UI/DeploymentMenu/TestMapUiElement.layout", spawnPoint.GetOwner().GetOrigin());
		spData.m_MapUiElement = mapUiElement;
		spData.m_MapUiButton = SCR_ModularButtonComponent.Cast(mapUiElement.GetRootWidget().FindHandler(SCR_ModularButtonComponent));
		
		m_aSpawnPointsData.Insert(spData);
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	void RemoveSpawnPoint(PR_DeploymentMenu_SpawnPointData spData)
	{
		// Remove Map UI element
		m_MapUiElementsModule.RemoveUiElement(spData.m_MapUiElement);
		
		m_aSpawnPointsData.RemoveItem(spData);
	}
}

