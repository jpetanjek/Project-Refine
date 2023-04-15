class PR_Dialogs
{
	static const ResourceName DIALOGS_CONFIG = "{4D2A02D8BCB520D9}Configs/Dialogs/PR_Dialogs.conf";
}

class PR_DisbandGroupDialog : SCR_ConfigurableDialogUi
{
	protected SCR_AIGroup m_Group;
	
	void PR_DisbandGroupDialog(SCR_AIGroup group)
	{
		m_Group = group;
		SCR_ConfigurableDialogUi.CreateFromPreset(PR_Dialogs.DIALOGS_CONFIG, "disband_group", this);
	}
	
	override void OnConfirm()
	{
		Close();
		SCR_PlayerControllerGroupComponent groupComp = SCR_PlayerControllerGroupComponent.GetLocalInstance();
		groupComp.RequestDeleteGroup(m_Group.GetGroupID());
	}
}

class PR_KickPlayerDialog : SCR_ConfigurableDialogUi
{
	protected int m_iPlayerId;
	
	void PR_KickPlayerDialog(int playerId)
	{
		m_iPlayerId = playerId;
		SCR_ConfigurableDialogUi.CreateFromPreset(PR_Dialogs.DIALOGS_CONFIG, "kick_player", this);
	}
	
	override void OnConfirm()
	{
		Close();
		SCR_PlayerControllerGroupComponent groupComp = SCR_PlayerControllerGroupComponent.GetLocalInstance();
		groupComp.RequestKickPlayer(m_iPlayerId);
	}
}

class PR_PromotePlayerDialog : SCR_ConfigurableDialogUi
{
	protected int m_iPlayerId;
	
	void PR_PromotePlayerDialog(int playerId)
	{
		m_iPlayerId = playerId;
		SCR_ConfigurableDialogUi.CreateFromPreset(PR_Dialogs.DIALOGS_CONFIG, "promote_player", this);
	}
	
	override void OnConfirm()
	{
		Close();
		SCR_PlayerControllerGroupComponent groupComp = SCR_PlayerControllerGroupComponent.GetLocalInstance();
		groupComp.RequestPromoteLeader(m_iPlayerId);
	}
}