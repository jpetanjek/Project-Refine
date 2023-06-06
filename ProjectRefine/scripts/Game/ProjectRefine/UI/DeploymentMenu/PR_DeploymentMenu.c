// Small class for UI to keep track of spawn points
class PR_DeploymentMenu_SpawnPointData : Managed
{
	PR_BaseSpawnPoint m_SpawnPoint;
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
		
		SCR_NavigationButtonComponent navBack = SCR_NavigationButtonComponent.GetNavigationButtonComponent("Back", GetRootWidget());
		navBack.m_OnActivated.Insert(OnBackButton);
		
		GetGame().GetInputManager().AddActionListener("ChatToggle", EActionTrigger.DOWN, OnChatToggleButton); 
		
		// Open map
		SCR_MapEntity mapEntity = SCR_MapEntity.GetMapInstance();
		MapConfiguration mapConfigFullscreen = mapEntity.SetupMapConfig(EMapEntityMode.REFINE_DEPLOYMENT_MENU, MAP_CONFIG, GetRootWidget() );
		mapEntity.OpenMap(mapConfigFullscreen);
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	void OnBackButton()
	{
		if (DiagMenu.GetBool(SCR_DebugMenuID.REFINE_DISABLE_AUTO_DEPLOYMENT_MENU))
		{
			Close();
		}
		else
		{
			// Call this through call queue, because otherwise pause menu opens, then closes instantly as it received event for Back button
			GetGame().GetCallqueue().CallLater(OnBackButtonDelayed, 50, false);
		}
	}
	void OnBackButtonDelayed()
	{
		ArmaReforgerScripted.OpenPauseMenu(false, true);
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	// Called by SCR_MapEntity after map has been opened and initialized. Here we can access map modules.
	protected void OnMapOpen(MapConfiguration mapConfig)
	{
		// We need to call it only once
		SCR_MapEntity.GetOnMapOpen().Remove(OnMapOpen);
		
		m_MapUiElementsModule = PR_MapUiElementsModule.Cast(SCR_MapEntity.GetMapInstance().GetMapModule(PR_MapUiElementsModule));
		
		InitSpawnPointUi();
		
		// Zoom and pan the map somewhere
		PR_GameMode gm = PR_GameMode.Cast(GetGame().GetGameMode());
		if (gm)
		{
			array<PR_CaptureArea> captureAreas = gm.GetCaptureAreas();
			
			vector acc = vector.Zero;
			foreach (PR_CaptureArea a : captureAreas)
			{
				acc = acc + a.GetOwner().GetOrigin();
			}
			acc = acc / (float)captureAreas.Count();
			
			SCR_MapEntity.GetMapInstance().ZoomPanSmooth(0, acc[0], acc[2]);
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	override void OnMenuClose()
	{
		super.OnMenuClose();
		
		SCR_MapEntity.GetOnMapOpen().Remove(OnMapOpen);
		
		SCR_MapEntity mapEntity = SCR_MapEntity.GetMapInstance();
		mapEntity.CloseMap();
		
		GetGame().GetInputManager().RemoveActionListener("MenuChat", EActionTrigger.DOWN, OnChatToggleButton);
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
		
		// Update chat panel - this must be called for chat to work
		widgets.m_ChatPanelComponent.OnUpdateChat(tDelta);
	}
	
	
	//-----------------------------------------------------------------------------------------------------------------------------
	void OnChatToggleButton()
	{
		if (!widgets.m_ChatPanelComponent.IsOpen())
			SCR_ChatPanelManager.GetInstance().OpenChatPanel(widgets.m_ChatPanelComponent);
	}
	
	
	//-----------------------------------------------------------------------------------------------------------------------------
	// SPAWN POINT SELECTION
	// Everything related to spawn points is here
	
	protected ref array<ref PR_DeploymentMenu_SpawnPointData> m_aSpawnPointsData = {};
	protected PR_DeploymentMenu_SpawnPointData m_SelectedSpawnPoint;
	
	//-----------------------------------------------------------------------------------------------------------------------------
	PR_DeploymentMenu_SpawnPointData FindSpawnPointData(SCR_ModularButtonComponent button)
	{
		foreach (PR_DeploymentMenu_SpawnPointData spdata : m_aSpawnPointsData)
		{
			if (spdata.m_MapUiButton == button)
				return spdata;
		}
		return null;
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	// This function defines if spawn point should be listed in UI
	bool ShowSpawnPointInUi(PR_BaseSpawnPoint spawnPoint, int myFactionId)
	{
		// True when spawn point exists in world
		// and when our faction owns it
		PR_ESpawnPointStateFlags flags = spawnPoint.GetStateFlags();
		
		return spawnPoint && spawnPoint.GetOwnerFactionId() == myFactionId && (flags & PR_ESpawnPointStateFlags.ACTIVE);
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	// Called from menu update, but only map is already initialized
	void UpdateSpawnPointUi(float tDelta)
	{
		FactionManager fm = GetGame().GetFactionManager();
		Faction myFaction = SCR_RespawnSystemComponent.GetLocalPlayerFaction();
		int myFactionId = fm.GetFactionIndex(myFaction);
		
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
		spawnPointsRemove.Clear(); // Unref the points we deleted, so that weak references are set to null too
		
		//---------------------------------------------------------------------------------
		// Add spawn points which are not in UI yet
		PR_GameMode gm = PR_GameMode.Cast(GetGame().GetGameMode());
		array<PR_BaseSpawnPoint> spawnPoints = PR_BaseSpawnPoint.GetAll();
		foreach (PR_BaseSpawnPoint spawnPoint : spawnPoints)
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
		
		//---------------------------------------------------------------------------------
		// Visualize state of selected spawn point
		UpdateSpawnPointPanel();
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	// Called from OnMapOpen
	void InitSpawnPointUi()
	{
		widgets.m_DeployButtonComponent.m_OnClicked.Insert(OnDeployButton);
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	void UpdateSpawnPointPanel()
	{
		// Spawn point name text
		string spawnPointNameText;
		
		if (!m_SelectedSpawnPoint)
			spawnPointNameText = "-";
		else
		{
			string name = m_SelectedSpawnPoint.m_SpawnPoint.GetName();
			float timeLeft_ms = m_SelectedSpawnPoint.m_SpawnPoint.GetNextRespawnWaveTime() - Replication.Time();
			timeLeft_ms = Math.Clamp(timeLeft_ms, 0, timeLeft_ms);
			spawnPointNameText = string.Format("%1 (%2 s)", name, Math.Floor(timeLeft_ms/1000.0));
		}
		widgets.m_SpawnPointNameText.SetText(spawnPointNameText);
		
		// Update elements
		
		//------------------------------------------
		// Warning text 0 and deployment timer
		bool deploymentBlocked = true;
		bool buttonToggled = false;
		string warningText = string.Empty;
		float deploymentTimeLeft = 0;
		bool showDeploymentCountdown = false;
		
		if (!m_SelectedSpawnPoint || !m_SelectedSpawnPoint.m_SpawnPoint)
		{
			// Nothing is selected
		}
		else
		{
			PR_ESpawnCondition condition = PR_BaseSpawnPoint.CanPlayerSpawn(GetGame().GetPlayerController().GetPlayerId());
			switch (condition)
			{
				case PR_ESpawnCondition.SPAWN_AVAILABLE:
				{
					deploymentBlocked = false;
					
					if (m_SelectedSpawnPoint.m_SpawnPoint.IsPlayerEnqueued(GetGame().GetPlayerController().GetPlayerId()) )
					{
						buttonToggled = true; // Button is toggled only when we are enqueued at selected spawn point
						deploymentTimeLeft = m_SelectedSpawnPoint.m_SpawnPoint.GetNextRespawnWaveTime() - Replication.Time();
						showDeploymentCountdown = true;
					}
					
					break;
				}
				case PR_ESpawnCondition.NO_GROUP:
					warningText = "You must join a group!";
					break;
				case PR_ESpawnCondition.NO_ROLE:
					warningText = "You must select a role!";
					break;
			}
		}		
		
		SetWarningText0(warningText);
		//SetCountdownText(deploymentTimeLeft, showDeploymentCountdown);
		SetCountdownText(0, false);
		
		
		//------------------------------------------
		// Warning text 1
		// This warning text is related to spawn point alone, rergardless of player state
		
		string warningText1;
		if (m_SelectedSpawnPoint && m_SelectedSpawnPoint.m_SpawnPoint)
		{
			PR_ESpawnPointStateFlags spFlags = m_SelectedSpawnPoint.m_SpawnPoint.GetStateFlags();
			if (spFlags & PR_ESpawnPointStateFlags.OCCUPIED_BY_ENEMY)
			{
				warningText1 = "Blocked by enemy!";
				deploymentBlocked = true;
			}
		}
		SetWarningText1(warningText1);
		
		// In this case the button is only enabled when toggled
		// So we can cancel deployment, but can't request it
		bool buttonEnabled;
		if (deploymentBlocked)
			buttonEnabled = buttonToggled;
		else
			buttonEnabled = true;
		
		// Deployment button
		widgets.m_DeployButtonComponent.SetEnabled(buttonEnabled);
		widgets.m_DeployButtonComponent.SetToggled(buttonToggled);
	}
	
	bool DeployConditionGroup()
	{
		SCR_GroupsManagerComponent groupsMgr = SCR_GroupsManagerComponent.GetInstance();
		bool result = groupsMgr.GetPlayerGroup(GetGame().GetPlayerController().GetPlayerId());
		return result;
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	void OnSpawnPointClick(SCR_ModularButtonComponent comp)
	{
		PR_DeploymentMenu_SpawnPointData spdata = FindSpawnPointData(comp);
		if (!spdata)
			return;
		
		m_SelectedSpawnPoint = spdata;
		
		// Toggle this button, untoggle others
		foreach (PR_DeploymentMenu_SpawnPointData s : m_aSpawnPointsData)
		{
			s.m_MapUiButton.SetToggled(spdata == s);
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	void AddSpawnPoint(notnull PR_BaseSpawnPoint spawnPoint)
	{
		Print(string.Format("AddSpawnPoint: %1 %2", spawnPoint, spawnPoint.GetName()));
		
		PR_DeploymentMenu_SpawnPointData spData = new PR_DeploymentMenu_SpawnPointData();
		
		spData.m_SpawnPoint = spawnPoint;
		//spData.m_sSpawnPointName = spawnPoint.GetName();
		
		// Add to map
		PR_MapUiElementComponent mapUiElement = m_MapUiElementsModule.CreateUiElement("{F2689BBD0CAFEB7B}UI/DeploymentMenu/TestMapUiElement.layout", spawnPoint.GetOwner().GetOrigin());
		spData.m_MapUiElement = mapUiElement;
		spData.m_MapUiButton = SCR_ModularButtonComponent.Cast(mapUiElement.GetRootWidget().FindHandler(SCR_ModularButtonComponent));
		
		spData.m_MapUiButton.m_OnClicked.Insert(OnSpawnPointClick);
		
		m_aSpawnPointsData.Insert(spData);
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	void RemoveSpawnPoint(PR_DeploymentMenu_SpawnPointData spData)
	{
		// Remove Map UI element
		m_MapUiElementsModule.RemoveUiElement(spData.m_MapUiElement);
		
		m_aSpawnPointsData.RemoveItem(spData);
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	void OnDeployButton()
	{
		if (!m_SelectedSpawnPoint || !m_SelectedSpawnPoint.m_SpawnPoint)
			return;
		
		PR_PlayerControllerDeploymentComponent deploymentComp = PR_PlayerControllerDeploymentComponent.GetLocalInstance();
		
		// If button is toggled, we request dequeue ourselves from spawn point,
		// Otherwise we ask to enqueue ourselves
		
		if (m_SelectedSpawnPoint.m_SpawnPoint.IsPlayerEnqueued(GetGame().GetPlayerController().GetPlayerId()) )
			deploymentComp.AskDequeueAtSpawnPoint(m_SelectedSpawnPoint.m_SpawnPoint);
		else
			deploymentComp.AskEnqueueAtSpawnPoint(m_SelectedSpawnPoint.m_SpawnPoint);
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	// There are two 'warning texts', they can be shown at same time
	void SetWarningText0(string s)
	{
		widgets.m_Warning0.SetVisible(!s.IsEmpty());
		if (!s.IsEmpty())
		{
			widgets.m_WarningText0.SetText(s);
		}
	}
	void SetWarningText1(string s)
	{
		widgets.m_Warning1.SetVisible(!s.IsEmpty());
		if (!s.IsEmpty())
		{
			widgets.m_WarningText1.SetText(s);
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	void SetCountdownText(float timeLeft_ms, bool showText)
	{
		widgets.m_DeploymentCountdownText.SetVisible(showText);
		
		if(showText)
		{
			timeLeft_ms = Math.Clamp(timeLeft_ms, 0, timeLeft_ms);
						
			string countdownText;
			if (timeLeft_ms == 0.0)
				countdownText = "Deploying ...";
			else
				countdownText = string.Format("Deploying in %1...", Math.Ceil(timeLeft_ms/1000.0));
			widgets.m_DeploymentCountdownText.SetText(countdownText);
		}
	}
}

