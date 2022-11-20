modded class SCR_MapMenuUI: ChimeraMenuBase
{	
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