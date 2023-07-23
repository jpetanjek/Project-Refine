modded class SCR_ListBoxComponent
{
	[Attribute("", UIWidgets.EditBox)]
	protected string m_sVerticalLayoutWidgetName;
	
	override protected void _SetItemSelected(int item, bool selected, bool invokeOnChanged)
	{
		if (!m_bMultiSelection)
		{
			if (selected)
				m_iCurrentItem = item;
		}
		
		super._SetItemSelected(item, selected, invokeOnChanged);
	}
	
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		
		if (m_sVerticalLayoutWidgetName != string.Empty)
			m_wList = VerticalLayoutWidget.Cast(w.FindAnyWidget(m_sVerticalLayoutWidgetName));
	}
}