class PR_FactionSelecitonMenu : ChimeraMenuBase
{
	protected ref PR_FactionSelectionMenuWidgets widgets = new PR_FactionSelectionMenuWidgets();
	
	//-----------------------------------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		widgets.Init(GetRootWidget());
		
		// Back button
		SCR_NavigationButtonComponent navBack = SCR_NavigationButtonComponent.GetNavigationButtonComponent("Back", GetRootWidget());
		navBack.m_OnActivated.Insert(OnBackButton);
		
		InitFactionButtons();
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	override void OnMenuClose()
	{
		super.OnMenuClose();
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
	override void OnMenuUpdate(float tDelta)
	{
		
	}
	
	void InitFactionButtons()
	{
		// No reason to create them dynamically now
		
		array<PR_FactionSelectionButtonComponent> buttons = {	widgets.m_FactionButton0Component1,
																widgets.m_FactionButton1Component1,
																widgets.m_FactionButton2Component1};
		
		FactionManager fm = GetGame().GetFactionManager();
		int nFactions = fm.GetFactionsCount();
		
		for (int buttonId = 0; buttonId < buttons.Count(); buttonId++)
		{
			if (buttonId < nFactions)
			{
				Faction f = fm.GetFactionByIndex(buttonId);
				SCR_Faction scrFaction = SCR_Faction.Cast(f);
				buttons[buttonId].Init(scrFaction);
				SCR_ModularButtonComponent buttonComp = SCR_ModularButtonComponent.Cast(buttons[buttonId].GetRootWidget().FindHandler(SCR_ModularButtonComponent));
				buttonComp.m_OnClicked.Insert(OnFactionButton);
			}
			else
			{
				// Hide this button
				buttons[buttonId].GetRootWidget().SetVisible(false);
			}
		}
	}
	
	void OnFactionButton(SCR_ModularButtonComponent button)
	{
		PR_FactionSelectionButtonComponent comp = PR_FactionSelectionButtonComponent.Cast(button.GetRootWidget().FindHandler(PR_FactionSelectionButtonComponent));
		SCR_Faction faction = comp.GetFaction();
		
		SCR_RespawnComponent respawnComp = SCR_RespawnComponent.GetInstance();
		respawnComp.RequestPlayerFaction(faction);
	}
	
	void OnGiveFactionButton()
	{
		SCR_RespawnComponent respawnComp = SCR_RespawnComponent.GetInstance();
		FactionManager fm = GetGame().GetFactionManager();
		array<Faction> factions = {};
		fm.GetFactionsList(factions);
		respawnComp.RequestPlayerFaction(factions[0]);
	}
	
	void OnSpawnCharacterButton()
	{
		SCR_RespawnComponent respawnComp = SCR_RespawnComponent.GetInstance();
		respawnComp.TempRequestSpawnPlayer();
	}
}
