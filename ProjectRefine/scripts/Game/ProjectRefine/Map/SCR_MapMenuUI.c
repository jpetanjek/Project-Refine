modded class SCR_MapMenuUI: ChimeraMenuBase
{
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		// Otherwise game auto focuses on the invisible edit box which intercepts controls >_<
		GetGame().GetCallqueue().CallLater(GetGame().GetWorkspace().SetFocusedWidget, 0, false, null, false);
	}
	
	
	//------------------------------------------------------------------------------------------------
	override void Callback_OnChatToggleAction()
	{
		SCR_MapEntity mapEntity = SCR_MapEntity.GetMapInstance();
		
		if (!mapEntity.CanOpenChat())
			return;
		
		if (!m_ChatPanel)
			return;
		
		if (!m_ChatPanel.IsOpen())
			SCR_ChatPanelManager.GetInstance().OpenChatPanel(m_ChatPanel);
	}
};