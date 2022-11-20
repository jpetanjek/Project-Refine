class PR_ActiveMapIconMarkerClass : PR_ActiveMapIconClass
{
}


// Class for map markers placeable by player

class PR_ActiveMapIconMarker : PR_ActiveMapIcon
{
	// ID of player who controls this character
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected string m_sIconName;
	
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected string m_sMarkerText;

	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected int m_iMarkerColor = Color.RED;
		
	override protected void UpdateFromReplicatedState()
	{
		super.UpdateFromReplicatedState();
		
		MapItem mapItem = m_MapDescriptor.Item();
		MapDescriptorProps props = mapItem.GetProps();
		
		mapItem.SetImageDef(m_sIconName);
		//props.SetImageDef(m_sIconName);
		mapItem.SetDisplayName(m_sMarkerText);
		
		Color color = Color.FromIntSRGB(m_iMarkerColor);
		props.SetFrontColor(color);
		props.SetOutlineColor(color);
		props.SetBackgroundColor(color);
		props.SetTextColor(color);
	}
	
	void InitMarkerProps(string markerText, string iconName, int markerColor)
	{
		m_sIconName = iconName;
		m_sMarkerText = markerText;
		m_iMarkerColor = markerColor;
		Replication.BumpMe();
		UpdateFromReplicatedState();
	}
}