// Autogenerated by the Generate Class from Layout plugin, version 0.3.0
// Layout file: UI/FactionSelectionMenu/FactionSelectionMenu.layout

class PR_FactionSelectionMenuWidgets
{
	static const ResourceName s_sLayout = "{FAF119E75E0B9EBD}UI/FactionSelectionMenu/FactionSelectionMenu.layout";
	ResourceName GetLayout() { return s_sLayout; }

	ButtonWidget m_FactionButton0;
	SCR_ModularButtonComponent m_FactionButton0Component0;
	PR_FactionSelectionButtonComponent m_FactionButton0Component1;

	ButtonWidget m_FactionButton1;
	SCR_ModularButtonComponent m_FactionButton1Component0;
	PR_FactionSelectionButtonComponent m_FactionButton1Component1;

	TextWidget m_GameModeName;

	bool Init(Widget root)
	{
		m_FactionButton0 = ButtonWidget.Cast(root.FindWidget("SizeBase.VerticalLayout0.ContentOverlay.HorizontalLayout.FactionSelection.Buttons.m_FactionButton0"));
		m_FactionButton0Component0 = SCR_ModularButtonComponent.Cast(m_FactionButton0.FindHandler(SCR_ModularButtonComponent));
		m_FactionButton0Component1 = PR_FactionSelectionButtonComponent.Cast(m_FactionButton0.FindHandler(PR_FactionSelectionButtonComponent));

		m_FactionButton1 = ButtonWidget.Cast(root.FindWidget("SizeBase.VerticalLayout0.ContentOverlay.HorizontalLayout.FactionSelection.Buttons.m_FactionButton1"));
		m_FactionButton1Component0 = SCR_ModularButtonComponent.Cast(m_FactionButton1.FindHandler(SCR_ModularButtonComponent));
		m_FactionButton1Component1 = PR_FactionSelectionButtonComponent.Cast(m_FactionButton1.FindHandler(PR_FactionSelectionButtonComponent));

		m_GameModeName = TextWidget.Cast(root.FindWidget("SizeBase.VerticalLayout0.ContentOverlay.HorizontalLayout.Overlay0.Tutorial.m_GameModeName"));

		return true;
	}
}
