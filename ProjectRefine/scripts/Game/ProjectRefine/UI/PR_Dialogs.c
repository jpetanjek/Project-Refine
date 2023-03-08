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