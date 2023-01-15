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
}