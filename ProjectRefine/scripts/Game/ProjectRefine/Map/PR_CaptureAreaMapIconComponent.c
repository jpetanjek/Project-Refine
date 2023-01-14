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
		
		string iconName;
		if (showCaptureProgress)
			iconName = string.Format("[%1%%] %2", captureProgressPercent, m_CaptureArea.GetName());
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
		
		SCR_MapEntity.GetOnMapOpen().Insert(OnMapOpen);
		
		UpdateMapIcon();
	}
	
	//------------------------------------------------------------------------------------------------
	void OnCaptureAreaAnyPropertyChanged(PR_CaptureArea captureArea)
	{
		UpdateMapIcon();
	}
	
	//------------------------------------------------------------------------------------------------
	void OnMapOpen()
	{
		// When map is opened, refresh the icon too, in case player's faction has changed
		UpdateMapIcon();
	}
};
