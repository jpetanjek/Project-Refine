// Tooltip which is shown when we press the action button on group entry
// It has some buttons for group management
class PR_GroupTooltipComponent : PR_ButtonsTooltipComponent
{
	protected SCR_AIGroup m_Group;
	
	SCR_ModularButtonComponent m_DisbandButton;
	SCR_ModularButtonComponent m_LockUnlockButton;
	
	void Init(SCR_AIGroup group)
	{
		if (!group)
			return;
		
		m_Group = group;
		
		string lockBtnText;
		if (group.IsPrivate())
			lockBtnText = "Unlock";
		else
			lockBtnText = "Lock";
		m_LockUnlockButton = CreateButton(lockBtnText);
		m_LockUnlockButton.m_OnClicked.Insert(OnLockUnlockButton);
		
		m_DisbandButton = CreateButton("Disband");
		m_DisbandButton.m_OnClicked.Insert(OnDisbandButton);
	}
	
	void OnDisbandButton()
	{
		if (!IsActionAllowed())
			return;
		
		SCR_ConfigurableDialogUi dlg = new PR_DisbandGroupDialog(m_Group);
		
		Close();
	}
	
	void OnLockUnlockButton()
	{
		if (!IsActionAllowed())
			return;
		
		SCR_PlayerControllerGroupComponent c = SCR_PlayerControllerGroupComponent.GetLocalInstance();
		c.RequestPrivateGroupChange(GetGame().GetPlayerController().GetPlayerId() , !m_Group.IsPrivate());
		
		Close();
	}
	
	// True when we are the group leader
	bool IsActionAllowed()
	{
		if (!m_Group)
			return false;
		
		// These actions are allowed when we are the group leader
		return m_Group.GetLeaderID() == GetGame().GetPlayerController().GetPlayerId();
	}
}