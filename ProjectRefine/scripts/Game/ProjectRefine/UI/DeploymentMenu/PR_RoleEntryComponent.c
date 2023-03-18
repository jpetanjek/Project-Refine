/*
Component for the role entry in role panel
*/

class PR_RoleEntryComponent : ScriptedWidgetComponent
{
	protected ref PR_RoleEntryWidgets widgets = new PR_RoleEntryWidgets();
	
	protected Widget m_wRoot;
	
	SCR_ModularButtonComponent m_ButtonComp;
	
	int m_iRoleIndex = -1;
	
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		widgets.Init(w);
		
		m_ButtonComp = SCR_ModularButtonComponent.Cast(w.FindHandler(SCR_ModularButtonComponent));
		m_ButtonComp.m_OnClicked.Insert(OnTileClick);
	}
	
	override void HandlerDeattached(Widget w)
	{
		bool what = 1;
	}
	
	void Init(int roleIndex, PR_Role role)
	{
		m_iRoleIndex = roleIndex;
		
		widgets.m_RoleName.SetText(role.GetRoleName());
		widgets.m_RolePrimary.SetText(role.GetPrimaryWeapon());
		widgets.m_RoleSecondary.SetText(role.GetSecondaryWeapon());
	}
	
	void Redraw(int claimable, int available, bool claimedByMe)
	{
		string availabilityEntry = string.Format("%1 / %2", claimable, available);
		widgets.m_RoleAvailability.SetText(availabilityEntry);
		
		m_ButtonComp.SetToggled(claimedByMe);
	}
	
	void EnableClaimButton(bool enable)
	{
		m_ButtonComp.SetEnabled(enable);
	}
	
	void OnTileClick()
	{
		// The code which was previously in RoleClaimButton
		PR_PlayerControllerRoleComponent pc_roleComponent = PR_PlayerControllerRoleComponent.Cast(GetGame().GetPlayerController().FindComponent(PR_PlayerControllerRoleComponent));
		if(!pc_roleComponent)
			return;
		
		pc_roleComponent.TryClaimRole(m_iRoleIndex);
		return;
	}
}