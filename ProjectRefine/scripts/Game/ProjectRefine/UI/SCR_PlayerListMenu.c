modded class SCR_PlayerListMenu
{
	PR_PlayerListGameModeHeaderComponent m_HeaderComponent;
	
	override override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		Widget w = GetRootWidget().FindAnyWidget("m_RefineGameModeHeader");
		if (w)
			m_HeaderComponent = PR_PlayerListGameModeHeaderComponent.Cast(w.FindHandler(PR_PlayerListGameModeHeaderComponent));
	}
	
	override void OnMenuUpdate(float tDelta)
	{
		super.OnMenuUpdate(tDelta);
		
		if (m_HeaderComponent)
			m_HeaderComponent.UpdateValues();
	}
}