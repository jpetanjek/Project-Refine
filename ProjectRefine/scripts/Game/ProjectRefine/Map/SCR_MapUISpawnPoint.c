modded class SCR_MapUISpawnPoint : SCR_MapUIElement
{
	//------------------------------------------------------------------------------
	override void Init(SCR_SpawnPoint sp)
	{
		m_SpawnPoint = sp;
		m_sFactionKey = sp.GetFactionKey();
		UpdateIcon();
		
		// Map descriptor is important too, doesn't work without it, couldn't figure out why
		
		SCR_MapDescriptorComponent descr = SCR_MapDescriptorComponent.Cast(m_SpawnPoint.FindComponent(SCR_MapDescriptorComponent));
		m_MapItem = descr.Item();
		MapDescriptorProps props = m_MapItem.GetProps();
		props.SetIconVisible(false);
		props.SetTextVisible(false);
		props.Activate(true);
	}

	//------------------------------------------------------------------------------
	override void UpdateIcon()
	{
		string selection = "Generic_Select";
		string highlight = "Generic_Focus";
		
		m_bVisible = true;
		
		m_wHighlightImg.LoadImageFromSet(0, m_sImageSetARO, highlight);
		m_wSelectImg.LoadImageFromSet(0, m_sImageSetARO, selection);
	}
};