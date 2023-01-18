class PR_ActiveMapIconVehicleClass : PR_ActiveMapIconClass
{
}

class PR_ActiveMapIconVehicle : PR_ActiveMapIcon
{
	override protected void UpdateFromReplicatedState()
	{
		super.UpdateFromReplicatedState();
		
		MapItem mapItem = m_MapDescriptor.Item();
		MapDescriptorProps props = mapItem.GetProps();
		
		// Icon color
		Color color;
		Faction faction = GetGame().GetFactionManager().GetFactionByIndex(m_iFactionId);
		if (faction)
			color = faction.GetFactionColor();
		else
			color = Color.White;
		props.SetFrontColor(color);
	}
	
	
	override protected void UpdateFaction(FactionAffiliationComponent factionAffiliationComponent, bool init)
	{
		SCR_FactionAffiliationComponent scrf = SCR_FactionAffiliationComponent.Cast(factionAffiliationComponent);
		
		FactionManager fm = GetGame().GetFactionManager();
		
		if (!scrf)
		{
			Faction faction = factionAffiliationComponent.GetAffiliatedFaction();
			m_iFactionId = fm.GetFactionIndex(faction);
			return;
		}
		
		Faction prevFaction = scrf.GetPreviousAffiliatedFaction();
		Faction faction = scrf.GetAffiliatedFaction();
		
		if (init)
			m_iFactionId = fm.GetFactionIndex(scrf.GetDefaultAffiliatedFaction());	// If it's just created, visible to default faction
		else if (faction)
			m_iFactionId = fm.GetFactionIndex(faction);		// If it has new faction, visible to players of the new faction
		else if (prevFaction)
			m_iFactionId = fm.GetFactionIndex(prevFaction);	// If it has no new faction (was dismounted), but has previous faction, visible to previous faction
		else
			m_iFactionId = -1;
	}
}