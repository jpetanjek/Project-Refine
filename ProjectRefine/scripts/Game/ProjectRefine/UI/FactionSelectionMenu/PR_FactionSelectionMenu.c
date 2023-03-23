class PR_FactionSelecitonMenu : ChimeraMenuBase
{
	protected ref PR_FactionSelectionMenuWidgets widgets = new PR_FactionSelectionMenuWidgets();
	
	//-----------------------------------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		widgets.Init(GetRootWidget());
		
		SCR_NavigationButtonComponent navBack = SCR_NavigationButtonComponent.GetNavigationButtonComponent("Back", GetRootWidget());
		navBack.m_OnActivated.Insert(OnBackButton);
		
		widgets.m_Faction0ButtonComponent.m_OnClicked.Insert(OnGiveFactionButton);
		widgets.m_Faction1ButtonComponent.m_OnClicked.Insert(OnSpawnCharacterButton);
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	override void OnMenuClose()
	{
		super.OnMenuClose();
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	void OnBackButton()
	{
		Close();
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	override void OnMenuUpdate(float tDelta)
	{
		
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
