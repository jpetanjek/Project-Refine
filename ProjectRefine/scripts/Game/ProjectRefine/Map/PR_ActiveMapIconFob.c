class PR_ActiveMapIconFobClass : PR_ActiveMapIconClass
{
}

class PR_ActiveMapIconFob : PR_ActiveMapIcon
{
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected int m_iSupplyCount;
	
	protected PR_SupplyHolderComponent m_SupplyHolder;
	
	override void Init(IEntity target, vector pos = vector.Zero, int factionId = -1, int groupId = -1)
	{
		super.Init(target, pos, factionId, groupId);
		
		m_SupplyHolder = PR_SupplyHolderComponent.Cast(target.FindComponent(PR_SupplyHolderComponent));
	}
	
	override protected void UpdatePropsFromTarget()
	{
		super.UpdatePropsFromTarget();
		
		m_iSupplyCount = m_SupplyHolder.GetSupply();
	}
	
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
		
		// Icon text
		mapItem.SetDisplayName(string.Format("FOB (Supplies: %1)", m_iSupplyCount));
	}
}