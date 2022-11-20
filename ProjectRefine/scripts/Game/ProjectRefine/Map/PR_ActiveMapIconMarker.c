class PR_ActiveMapIconMarkerClass : PR_ActiveMapIconClass
{
}

class PR_ActiveMapIconMarker : PR_ActiveMapIcon
{
	// ID of player who controls this character
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected string m_sIconName;
	
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected string m_sMarkerText;
	
	override protected void UpdateFromReplicatedState()
	{
		super.UpdateFromReplicatedState();
		
		MapItem mapItem = m_MapDescriptor.Item();
		MapDescriptorProps props = mapItem.GetProps();
		
		mapItem.SetImageDef(m_sIconName);
		//props.SetImageDef(m_sIconName);
		mapItem.SetDisplayName(m_sMarkerText);
	}
	
	void InitMarkerProps(string markerText, string iconName)
	{
		m_sIconName = iconName;
		m_sMarkerText = markerText;
		Replication.BumpMe();
		UpdateFromReplicatedState();
	}
}