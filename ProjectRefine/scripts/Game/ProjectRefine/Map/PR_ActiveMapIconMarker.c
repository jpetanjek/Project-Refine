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
	
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected string m_sPlayerName = string.Empty;
	
	static ref array<PR_ActiveMapIconMarker> s_aAllMarkers = {}; // TODO get list of markers from the manager
	
	void PR_ActiveMapIconMarker(IEntitySource src, IEntity parent)
	{
		s_aAllMarkers.Insert(this);
	}
	
	void ~PR_ActiveMapIconMarker()
	{
		s_aAllMarkers.RemoveItem(this);
	}
	
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
	
	void InitMarkerProps(string markerText, string iconName, int markerColor, string playerName)
	{
		m_sIconName = iconName;
		m_sMarkerText = markerText;
		m_iMarkerColor = markerColor;
		m_sPlayerName = playerName;
		Replication.BumpMe();
		UpdateFromReplicatedState();
	}
	
	override void OnCursorHover(SCR_MapEntity mapEntity, SCR_MapCursorModule cursorModule)
	{
		string str = string.Format("Placed by %1\n[Del] - Delete marker", m_sPlayerName);
		cursorModule.ShowTooltip(true, str);
	}
}