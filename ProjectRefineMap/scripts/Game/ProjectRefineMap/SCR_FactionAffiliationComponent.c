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
	override protected void OnFactionChanged()
	{	
		super.OnFactionChanged(); // Here the events are invoked
		
		m_PrevFaction = GetAffiliatedFaction();
	}
}