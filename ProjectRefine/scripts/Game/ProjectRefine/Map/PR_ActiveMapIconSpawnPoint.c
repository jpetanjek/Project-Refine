class PR_ActiveMapIconSpawnPointClass : PR_ActiveMapIconClass
{
}

class PR_ActiveMapIconSpawnPoint : PR_ActiveMapIcon
{	
	override protected void UpdatePosAndDirPropFromTarget()
	{
		// Same as base class, but we don't copy direction
		vector worldTransform[4];
		m_Target.GetWorldTransform(worldTransform);
		m_vPosAndDir = Vector(worldTransform[3][0], 0, worldTransform[3][2]); // X, Dir, Z
	}
	
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