modded class SCR_VONEntryRadio
{
	override void InitEntry()
	{
		// TODO FIX RADIO
		
		if (!m_RadioTransceiver)
			return;
		
		BaseRadioComponent radio = m_RadioTransceiver.GetRadio();
		if (radio)
		{
			radio.SetPower(true); // Please switch it on. By default some radios are disabled otherwise.
		}
		
		super.InitEntry();
	}
}