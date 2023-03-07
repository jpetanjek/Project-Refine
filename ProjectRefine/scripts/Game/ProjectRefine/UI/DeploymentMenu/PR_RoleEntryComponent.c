/*
Component for the panel with list of roles
*/

class PR_RoleEntryComponent : ScriptedWidgetComponent
{
	protected ref PR_RoleEntryWidgets widgets = new PR_RoleEntryWidgets();
	
	protected Widget m_wRoot;
	
	override void HandlerAttached(Widget w)
	{
		widgets.Init(w);
	}
	
	override void HandlerDeattached(Widget w)
	{
		bool what = 1;
	}
	
	void Init()
	{
		widgets.m_RoleName.SetText("test");
	}
}