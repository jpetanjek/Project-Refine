/*
Component for one group member line
*/

class PR_GroupMemberLineComponent : ScriptedWidgetComponent
{
	protected ref PR_GroupMemberLineWidgets widgets = new PR_GroupMemberLineWidgets();
	
	protected Widget m_wRoot;
	
	protected SCR_AIGroup m_Group;
	protected int m_iPlayerId = -1; // ID of player for which this line is created
	
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		
		widgets.Init(w);
		
		widgets.m_ActionButtonComponent.m_OnClicked.Insert(OnActionButton);
		
		GetGame().GetCallqueue().CallLater(OnFrame, 0, true);
	}
	
	override void HandlerDeattached(Widget w)
	{
		GetGame().GetCallqueue().Remove(OnFrame);
	}
	
	void OnFrame()
	{
		if (!m_Group)
			return;
		
		UpdateActionButton();
	}
	
	void Init(SCR_AIGroup group, int playerId)
	{
		m_Group = group;
		m_iPlayerId = playerId;
		
		UpdateAllWidgets();
	}
	
	void RemoveFromUi()
	{
		m_wRoot.RemoveFromHierarchy();
	}
	
	void UpdateActionButton()
	{
		bool visible = true;
		int myPlayerId = GetGame().GetPlayerController().GetPlayerId();
		
		if (m_iPlayerId == myPlayerId)	// Hide if this line is for ourselves
			visible = false;
		else if (!m_Group.IsPlayerLeader(myPlayerId))	// Hide if we are not leader
			visible = false;
		
		widgets.m_ActionButton.SetVisible(visible);
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
	
	void OnActionButton()
	{
		Widget wTooltip = SCR_TooltipManagerEntity.CreateTooltipEx("{8C2232C8A9444731}UI/DeploymentMenu/GroupMemberTooltip.layout",
			widgets.m_ActionButton,
			followCursor: false,
			checkWidgetUnderCursor: false,
			offset: "-5 10 0");
		
		PR_GroupMemberTooltipComponent tooltipComp = PR_GroupMemberTooltipComponent.Cast(wTooltip.FindHandler(PR_GroupMemberTooltipComponent));
		tooltipComp.Init(m_Group, m_iPlayerId);
	}
}