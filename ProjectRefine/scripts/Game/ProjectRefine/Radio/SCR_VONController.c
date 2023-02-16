modded class SCR_VONController
{
	protected SCR_VONEntry m_ShortRangeEntry; // Entry for short range entry, if available
	
	override void AddEntry(SCR_VONEntry entry)
	{
		super.AddEntry(entry);
	
		SCR_VONEntryRadio entryRadio = SCR_VONEntryRadio.Cast(entry);
		if (entryRadio && !entryRadio.m_bIsLongRange)
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
		// Please this button should only toggle short range radio
		m_ActiveEntry = m_ShortRangeEntry;
		
		super.OnVONChannel(value, reason);
	}
	
}