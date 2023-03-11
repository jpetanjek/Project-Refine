/*
Component for the role entry in role panel
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
		widgets.m_RolePrimary.SetText(role.GetPrimaryWeapon());
		widgets.m_RoleSecondary.SetText(role.GetSecondaryWeapon());
		
		RedrawAvailability(claimable, available);
		
		// Claim button init
		PR_RoleClaimButtonComponent comp = PR_RoleClaimButtonComponent.Cast(widgets.m_RoleClaimButton.FindHandler(PR_RoleClaimButtonComponent));
		comp.Init(roleIndex);
	}
	
	void RedrawAvailability(int claimable, int available)
	{
		string availabilityEntry = string.Format("%1/%2", claimable, available);
		widgets.m_RoleAvailability.SetText(availabilityEntry);
	}
	
	void EnableClaimButton(bool enable)
	{
		widgets.m_RoleClaimButton.SetEnabled(enable);
		widgets.m_RoleClaimButton.SetVisible(enable);
	}
}