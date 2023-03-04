/*
Component for one group member line
*/

class PR_GroupMemberLineComponent : ScriptedWidgetComponent
{
	protected ref PR_GroupMemberLineWidgets widgets = new PR_GroupMemberLineWidgets();
	
	protected Widget m_wRoot;
	
	protected int m_iPlayerId = -1; // ID of player for which this line is created
	
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		
		widgets.Init(w);
	}
	
	void Init(int playerId)
	{
		m_iPlayerId = playerId;
		
		UpdateAllWidgets();
	}
	
	void RemoveFromUi()
	{
		m_wRoot.RemoveFromHierarchy();
	}
	
	void UpdateAllWidgets()
	{
		PlayerManager pm = GetGame().GetPlayerManager();
		
		// Player name
		widgets.m_PlayerNameText.SetText(pm.GetPlayerName(m_iPlayerId));
		
		// Other stuff later
	}
	
	int GetPlayerId()
	{
		return m_iPlayerId;
	}
}