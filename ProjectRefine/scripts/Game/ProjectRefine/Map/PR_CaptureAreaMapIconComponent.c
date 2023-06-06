[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class PR_CaptureAreaMapIconComponentClass : ScriptComponentClass
{
	// prefab properties here
};

//------------------------------------------------------------------------------------------------
/*!
	Component to be attached to Capture Area prefab, which will update associated map icon.
*/
class PR_CaptureAreaMapIconComponent : ScriptComponent
{
	[Attribute()]
	protected ref PR_MapIconStyleBase m_Style;
	
	protected MapDescriptorComponent m_MapDescriptor;
	protected PR_CaptureArea m_CaptureArea;

	//------------------------------------------------------------------------------------------------
	void UpdateMapIcon()
	{
		MapItem mapItem = m_MapDescriptor.Item();
		if (!mapItem)
			return;
		MapDescriptorProps props = mapItem.GetProps();
		
		// Сolor
		Color frontColor = null;
		Faction ownerFaction = GetGame().GetFactionManager().GetFactionByIndex(m_CaptureArea.GetOwnerFactionId());
		if (ownerFaction)
			frontColor = ownerFaction.GetFactionColor();
		else
			frontColor = Color.White;
		
		props.SetFrontColor(frontColor);
		
		// Name
		Faction playerFaction = SCR_RespawnSystemComponent.GetLocalPlayerFaction();
		int playerFactionId = GetGame().GetFactionManager().GetFactionIndex(playerFaction);
		int areaOwnerFactionId = m_CaptureArea.GetOwnerFactionId();
		bool showCaptureProgress = areaOwnerFactionId != -1 && playerFactionId == areaOwnerFactionId;
		int captureProgressPercent = Math.Floor(m_CaptureArea.GetPointsRelative() * 100.0);
		
		string strLocked = string.Empty;
		if (!m_CaptureArea.GetCaptureable())
			strLocked = "LOCKED ";
		
		string iconName;
		if (showCaptureProgress)
			iconName = string.Format("[%1%2%%] %3", strLocked, captureProgressPercent, m_CaptureArea.GetName());
		else
			iconName = m_CaptureArea.GetName();
		
		mapItem.SetDisplayName(iconName);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		m_MapDescriptor = MapDescriptorComponent.Cast(owner.FindComponent(MapDescriptorComponent));
		m_CaptureArea = PR_CaptureArea.Cast(owner.FindComponent(PR_CaptureArea));
		
		if (m_Style)
			m_Style.Apply(m_MapDescriptor);
		
		m_CaptureArea.m_OnAnyPropertyChanged.Insert(OnCaptureAreaAnyPropertyChanged);
		m_CaptureArea.m_OnLinkAdded.Insert(OnCaptureAreaLinkAdded);
		
		SCR_MapEntity.GetOnMapOpen().Insert(OnMapOpen);
		
		UpdateMapIcon();
	}
	
	//------------------------------------------------------------------------------------------------
	void OnCaptureAreaAnyPropertyChanged(PR_CaptureArea captureArea)
	{
		UpdateMapIcon();
	}
	
	//------------------------------------------------------------------------------------------------
	void OnCaptureAreaLinkAdded(PR_CaptureArea thisArea, PR_CaptureArea otherArea)
	{
		// Visualize links on the map
		
		MapItem mapItem = m_MapDescriptor.Item();
		
		MapDescriptorComponent otherDescriptor = MapDescriptorComponent.Cast(otherArea.GetOwner().FindComponent(MapDescriptorComponent));
		MapItem otherMapItem = otherDescriptor.Item();
		array<MapLink> otherMapLinks = {};
		otherMapItem.GetLinks(otherMapLinks);
		
		// Check if we have already visualized this link in opposite direction
		bool linkAlreadyAdded = false;
		foreach (MapLink link : otherMapLinks)
		{
			if (link.Owner() == mapItem || link.Target() == mapItem)
			{
				linkAlreadyAdded = true;
				break;
			}
		}
		
		if (!linkAlreadyAdded)
		{
			Color lineColor = new Color(1, 1, 1, 0.7);
			Color outlineColor = new Color(0, 0, 0, 0.3);
			
			MapLink mapLink = mapItem.LinkTo(otherMapItem);
			MapLinkProps props = mapLink.GetMapLinkProps();
			
			props.SetLineWidth(4.0);
			props.SetOutlineWidth(1.0);
			props.SetLineColor(lineColor);
			props.SetOutlineColor(outlineColor);
			props.SetLineType(EMapLineType.LT_FULL_LINE);
			props.SetVisible(true);
			
			//PrintFormat("Added link: %1 -> %2", thisArea.GetName(), otherArea.GetName());
		}
		
	}
	
	//------------------------------------------------------------------------------------------------
	void OnMapOpen()
	{
		// When map is opened, refresh the icon too, in case player's faction has changed
		UpdateMapIcon();
	}
};
