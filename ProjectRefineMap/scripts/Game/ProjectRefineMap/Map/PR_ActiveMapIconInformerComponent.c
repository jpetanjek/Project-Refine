[ComponentEditorProps(category: "GameScripted/GameMode/Components", description: "Base for gamemode scripted component.")]
class PR_ActiveMapIconInformerComponentClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
class PR_ActiveMapIconInformerComponent : ScriptComponent
{
	[Attribute(uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_IconPrefab;
	
	protected PR_ActiveMapIconManagerComponent m_MapManager;
	
	protected PR_ActiveMapIcon m_Icon;
	
	ResourceName GetIconPrefab()
	{
		return m_IconPrefab;
	}
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		if (!GetGame().GetWorldEntity() || Replication.IsClient())
  			return;

		// Only Server registers in to manager here
		// inform game mode-> ActiveMapIconManagerComponent that we exist
		m_MapManager = PR_ActiveMapIconManagerComponent.GetInstance();
		if(m_MapManager)
		{
			m_MapManager.ServerRegisterAsync(this); // It will be registered asynchronously, it will call FinishInit
		}
	}
	
	// Called from manager to finish init
	void InitCompletion(PR_ActiveMapIcon createdIcon)
	{
		m_Icon = createdIcon;
		
		SCR_FactionAffiliationComponent m_FactionAffiliationComponent = SCR_FactionAffiliationComponent.Cast(GetOwner().FindComponent(SCR_FactionAffiliationComponent));
		if (m_FactionAffiliationComponent && m_Icon)
		{
			m_FactionAffiliationComponent.GetOnFactionChanged().Insert(OnTargetFactionChanged);
		}
	}
	
	void OnTargetFactionChanged(FactionAffiliationComponent owner, Faction previousFaction, Faction newFaction)
	{
		// Inform icon directly
		if(m_Icon)
		{
			m_Icon.OnTargetFactionChanged();
			if(m_MapManager)
				m_MapManager.OnIconTargetFactionChanged(m_Icon);
		}
	}
	
	void ~PR_ActiveMapIconInformerComponent()
	{
		if(m_MapManager && m_Icon)
		{
			m_MapManager.Unregister(m_Icon);
		}
	}	
}