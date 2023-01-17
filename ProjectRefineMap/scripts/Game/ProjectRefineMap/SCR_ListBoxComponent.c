modded class SCR_ListBoxComponent
{
	override protected void _SetItemSelected(int item, bool selected, bool invokeOnChanged)
	{
		if (!m_bMultiSelection)
		{
			if (selected)
				m_iCurrentItem = item;
		}
		
		super._SetItemSelected(item, selected, invokeOnChanged);
	}
}