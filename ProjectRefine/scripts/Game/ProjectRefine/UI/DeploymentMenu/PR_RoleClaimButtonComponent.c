/*
Component for the role claim button in role entry
*/

class PR_RoleClaimButtonComponent : ScriptedWidgetComponent
{
	int m_iRoleIndex = -1;
	
	void Init(int index)
	{
		m_iRoleIndex = index;
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		PR_PlayerControllerRoleComponent pc_roleComponent = PR_PlayerControllerRoleComponent.Cast(GetGame().GetPlayerController().FindComponent(PR_PlayerControllerRoleComponent));
		if(!pc_roleComponent)
			return false;
		
		pc_roleComponent.TryClaimRole(m_iRoleIndex);
		return true;
	}
}