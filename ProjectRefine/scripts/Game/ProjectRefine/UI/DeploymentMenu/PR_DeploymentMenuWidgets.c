// Autogenerated by the Generate Class from Layout plugin, version 0.3.0
// Layout file: UI/DeploymentMenu/DeploymentMenu.layout

class PR_DeploymentMenuWidgets
{
	static const ResourceName s_sLayout = "{EC80BA3CC09774C4}UI/DeploymentMenu/DeploymentMenu.layout";
	ResourceName GetLayout() { return s_sLayout; }

	FrameWidget m_MapFrame;

	OverlayWidget m_MapInteractionArea;

	TextWidget m_RespawnTimerText;

	ButtonWidget m_SpawnPointSpinbox;
	SCR_SpinBoxComponent m_SpawnPointSpinboxComponent;

	SizeLayoutWidget m_RoleList;
	PR_RoleListComponent m_RoleListComponent;

	SizeLayoutWidget m_GroupList;
	PR_GroupListComponent m_GroupListComponent;

	bool Init(Widget root)
	{
		m_MapFrame = FrameWidget.Cast(root.FindWidget("m_MapFrame"));

		m_MapInteractionArea = OverlayWidget.Cast(root.FindWidget("SizeBase.VerticalLayout0.ContentOverlay.VerticalLayout.Main.m_MapInteractionArea"));

		m_RespawnTimerText = TextWidget.Cast(root.FindWidget("SizeBase.VerticalLayout0.ContentOverlay.VerticalLayout.Main.m_MapInteractionArea.SpawnPointSelection.Overlay.VerticalLayout0.m_RespawnTimerText"));

		m_SpawnPointSpinbox = ButtonWidget.Cast(root.FindWidget("SizeBase.VerticalLayout0.ContentOverlay.VerticalLayout.Main.m_MapInteractionArea.SpawnPointSelection.Overlay.VerticalLayout0.m_SpawnPointSpinbox"));
		m_SpawnPointSpinboxComponent = SCR_SpinBoxComponent.Cast(m_SpawnPointSpinbox.FindHandler(SCR_SpinBoxComponent));

		m_RoleList = SizeLayoutWidget.Cast(root.FindWidget("SizeBase.VerticalLayout0.ContentOverlay.VerticalLayout.Main.Right.HorizontalLayout0.m_RoleList"));
		m_RoleListComponent = PR_RoleListComponent.Cast(m_RoleList.FindHandler(PR_RoleListComponent));

		m_GroupList = SizeLayoutWidget.Cast(root.FindWidget("SizeBase.VerticalLayout0.ContentOverlay.VerticalLayout.Main.Right.HorizontalLayout0.m_GroupList"));
		m_GroupListComponent = PR_GroupListComponent.Cast(m_GroupList.FindHandler(PR_GroupListComponent));

		return true;
	}
}
