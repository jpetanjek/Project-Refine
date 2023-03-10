/*
Component for the panel with list of roles
*/

class PR_RoleEntryComponent : ScriptedWidgetComponent
{
	protected ref PR_RoleEntryWidgets widgets = new PR_RoleEntryWidgets();
	
	protected Widget m_wRoot;
	
	int m_iRoleIndex = -1;
	
	override void HandlerAttached(Widget w)
	{
		widgets.Init(w);
	}
	
	override void HandlerDeattached(Widget w)
	{
		bool what = 1;
	}
	
	void Init(int roleIndex, PR_Role role, int claimable, int available)
	{
		m_iRoleIndex = roleIndex;
		widgets.m_RoleName.SetText(role.GetRoleName());
	}
}