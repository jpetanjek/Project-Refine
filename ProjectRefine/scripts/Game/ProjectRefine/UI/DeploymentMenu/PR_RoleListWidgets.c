// Autogenerated by the Generate Class from Layout plugin, version 0.3.0
// Layout file: UI/DeploymentMenu/RoleList.layout

class PR_RoleListWidgets
{
	static const ResourceName s_sLayout = "{38EF0B55AA92A5C4}UI/DeploymentMenu/RoleList.layout";
	ResourceName GetLayout() { return s_sLayout; }

	VerticalLayoutWidget m_RoleListLayout;

	bool Init(Widget root)
	{
		m_RoleListLayout = VerticalLayoutWidget.Cast(root.FindWidget("VerticalLayout0.m_RoleListLayout"));

		return true;
	}
}
