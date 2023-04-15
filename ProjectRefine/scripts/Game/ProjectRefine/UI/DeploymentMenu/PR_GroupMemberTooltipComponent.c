// Tooltip which is shown when we press the action button on group member entry
class PR_GroupMemberTooltipComponent : PR_ButtonsTooltipComponent
{
	protected SCR_AIGroup m_Group;
	protected int m_iPlayerId;
	
	SCR_ModularButtonComponent m_KickButton;
	SCR_ModularButtonComponent m_PromoteButton;
	
	void Init(SCR_AIGroup group, int playerId)
	{
		if (!group)
			return;
		
		m_Group = group;
		m_iPlayerId = playerId;
		
		m_PromoteButton = CreateButton("Promote");
		m_PromoteButton.m_OnClicked.Insert(OnPromoteButton);
		
		m_KickButton = CreateButton("Kick");
		m_KickButton.m_OnClicked.Insert(OnKickButton);
	}
	
	void OnKickButton()
	{
		if (!IsActionAllowed())
			return;
		
		PR_KickPlayerDialog dlg = new PR_KickPlayerDialog(m_iPlayerId);
		
		Close();
	}
	
	void OnPromoteButton()
	{
		if (!IsActionAllowed())
			return;
		
		PR_PromotePlayerDialog dlg = new PR_PromotePlayerDialog(m_iPlayerId);
		
		Close();
	}
	
	// True when we are the group leader and the player is still in our group
	bool IsActionAllowed()
	{
		if (!m_Group)
			return false;
		
		// These actions are allowed when we are the group leader
		if (m_Group.GetLeaderID() != GetGame().GetPlayerController().GetPlayerId())
			return false;
		
		// The targeted player must be still in our group
		if (!m_Group.IsPlayerInGroup(m_iPlayerId))
			return false;
		
		return true;
	}
}