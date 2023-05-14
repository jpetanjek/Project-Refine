class PR_BuildingEntryNameComponent : ScriptedWidgetComponent
{
	protected TextWidget m_wNameText;
	protected TextWidget m_wCostText;
	
	override event void HandlerAttached(Widget w)
	{
		m_wNameText = TextWidget.Cast(w.FindAnyWidget("NameText"));
		m_wCostText = TextWidget.Cast(w.FindAnyWidget("CostText"));
	}
	
	void Init(string name, int cost = 0, bool costVisible = false)
	{
		m_wNameText.SetText(name);
		
		m_wCostText.SetVisible(costVisible);
		if (costVisible)
		{
			m_wCostText.SetText(cost.ToString());
		}
	}
}