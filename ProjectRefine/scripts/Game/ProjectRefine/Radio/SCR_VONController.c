modded class SCR_VONController
{
	protected SCR_VONEntry m_ShortRangeEntry; // Entry for short range entry, if available
	
	override void AddEntry(SCR_VONEntry entry)
	{
		super.AddEntry(entry);
	
		SCR_VONEntryRadio entryRadio = SCR_VONEntryRadio.Cast(entry);
		
		if (!m_ShortRangeEntry && entryRadio && !entryRadio.m_bIsLongRange)
			m_ShortRangeEntry = entryRadio;
	}
	
	override void RemoveEntry(SCR_VONEntry entry)
	{
		super.RemoveEntry(entry);
		
		if (entry == m_ShortRangeEntry)
			m_ShortRangeEntry = null;
	}
	
	override void OnVONChannel(float value, EActionTrigger reason)
	{
		//--------------------------------------------------
		// Refresh the whole array of entries
		
		// Remove old entries
		array<ref SCR_VONEntry> allEntries = {};
		foreach (auto e : m_aEntries)
			allEntries.Insert(e);
		
		foreach (SCR_VONEntry entry : allEntries)
			RemoveEntry(entry);
		
		// Rescan everything and add entries again
		RegisterVONEntries();
		
		// Please this button should only toggle short range radio
		m_ActiveEntry = m_ShortRangeEntry;
		
		super.OnVONChannel(value, reason);
	}
	
	// Copied from SCR_GadgetManagerComponent
	protected void RegisterVONEntries()
	{
		PlayerController pc = PlayerController.Cast(GetOwner());
		IEntity controlledEntity = pc.GetControlledEntity();
		if (!controlledEntity)
			return;
		
		SCR_GadgetManagerComponent gadgetMgr = SCR_GadgetManagerComponent.Cast(controlledEntity.FindComponent(SCR_GadgetManagerComponent));
		if (!gadgetMgr)
			return;
		
		array<SCR_GadgetComponent> radiosArray = {};
		radiosArray.Copy(gadgetMgr.GetGadgetsByType(EGadgetType.RADIO)); 					// squad radios
		radiosArray.InsertAll(gadgetMgr.GetGadgetsByType(EGadgetType.RADIO_BACKPACK)); 	// backpack radio

		foreach (SCR_GadgetComponent radio : radiosArray)
		{
			BaseRadioComponent radioComp = BaseRadioComponent.Cast(radio.GetOwner().FindComponent(BaseRadioComponent));
			int count = radioComp.TransceiversCount();
			for (int i = 0 ; i < count; ++i)	// Get all individual transceivers (AKA channels) from the radio
			{
				SCR_VONEntryRadio radioEntry = new SCR_VONEntryRadio();
				radioEntry.SetRadioEntry(radioComp.GetTransceiver(i), i + 1, radio);
				
				AddEntry(radioEntry);
			}
		}
	}
	
}