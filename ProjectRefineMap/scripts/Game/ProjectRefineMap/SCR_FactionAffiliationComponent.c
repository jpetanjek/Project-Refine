modded class SCR_FactionAffiliationComponent: FactionAffiliationComponent
{
	protected Faction m_PrevFaction = null;
	
	//--------------------------------------------------------------------------------------------------------------------------
	// Returns previous faction
	// !!! This value is only valid within OnFactionChanged event! In other cases it's same as current faction.
	Faction GetPreviousAffiliatedFaction()
	{
		return m_PrevFaction;
	}
	
	//--------------------------------------------------------------------------------------------------------------------------
	override protected void OnFactionChanged(Faction previous, Faction current)
	{
		super.OnFactionChanged(previous, current); // Here the events are invoked

		if (m_OnFactionUpdate)
			m_OnFactionUpdate.Invoke(GetAffiliatedFaction());
		
		if (m_OnFactionChangedInvoker)
			m_OnFactionChangedInvoker.Invoke(this, previous, current);
		
		previous = GetAffiliatedFaction();
	}
}