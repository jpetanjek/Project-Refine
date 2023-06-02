[EntityEditorProps(description:"Script that manages building transitions from unbilt to built")]
class PR_BuildingManagerClass: GenericEntityClass
{

}

class PR_BuildingManager: GenericEntity
{
	//---------------------
	// Atributes
	[Attribute(desc: "Final prefab", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "et", category: "Entity Spawn Info")]
	ResourceName m_sFinalPrefab;
	
	[Attribute(desc: "Foundation prefab", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "et", category: "Entity Spawn Info")]
	ResourceName m_sFoundationPrefab;
	
	[Attribute("1", desc: "Maximum health")]
	int m_iMaxHealth;
	
	[Attribute("1", desc: "Health when placed")]
	int m_iPlacedHealth;
	
	[Attribute(desc: "Place as already built")]
	bool m_bPlacedBuilt;
	
	[Attribute("1", desc: "Health at which we switch to final prefab and entity is enabled")]
	int m_iFinalStageHealth;
	
	[Attribute("1", desc: "Health at which entity is disabled, should be lower than FinalStageHealth")]
	int m_iDisableHealth;
	
	protected PR_EAssetBuildingFlags m_eBuildingFlags;
	
	//---------------------
	// Data
	RplComponent m_RplComponent;
	
	IEntity m_Foundation;
	
	IEntity m_Final;
	
	[RplProp()]
	int m_iHealth = 1;
	
	// Faction which has created this
	[RplProp()]
	int m_iOwnerFactionId = -1;
	
	bool m_bEnabled = false;
	
	bool m_bIsBuilt = false;
	
	bool m_bHealthChanged = false;
	
	bool m_bInitialized = false;
	
	//---------------------
	// Must be called after initial entity is created
	void Init(int ownerFactionId, PR_EAssetBuildingFlags buildingFlags)
	{
		m_iOwnerFactionId = ownerFactionId;
		m_eBuildingFlags = buildingFlags;
		
		if (!m_bInitialized && m_Final)
			CallFinalInitEvents();		
		
		m_bInitialized = true;
		
		Replication.BumpMe();
	}
	
	int GetOwnerFactionId()
	{
		return m_iOwnerFactionId;
	}
	
	PR_EAssetBuildingFlags GetBuildingFlags()
	{
		return m_eBuildingFlags;
	}
	
	//---------------------	
	override void EOnInit(IEntity owner)
	{		
		m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		
		if(!m_RplComponent)
			return;
		
		if(m_RplComponent && !m_RplComponent.IsProxy())
		{
			SetFlags(EntityFlags.ACTIVE, true);
		}
		else if (m_RplComponent && m_RplComponent.IsProxy())
		{
			return;
		}
		
		if(m_bPlacedBuilt)
		{
			m_iHealth = m_iMaxHealth;
			Built();
		}
		else
		{
			m_iHealth = m_iPlacedHealth;
			m_Foundation = CreateStage( m_sFoundationPrefab );
		}
			
	}
	
	void ~PR_BuildingManager()
	{
		if( m_Foundation )
			SCR_EntityHelper.DeleteEntityAndChildren(m_Foundation);
		
		if( m_Final )
			SCR_EntityHelper.DeleteEntityAndChildren(m_Final);
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (m_bHealthChanged)
		{
			m_iHealth = Math.ClampInt(m_iHealth, 0, m_iMaxHealth);
			UpdateHealthLogic();
			m_bHealthChanged = false;
			ClearEventMask(EntityEvent.FRAME);
		}
	}
	
	protected IEntity CreateStage(ResourceName prefab)
	{
		EntitySpawnParams sp = EntitySpawnParams();
		sp.TransformMode = ETransformMode.WORLD;
		GetTransform(sp.Transform);
		
		return GetGame().SpawnEntityPrefab(Resource.Load(prefab), GetGame().GetWorld(), sp);
	}
	
	//---------------------
	// Functionality
	
	void Build(int amount)
	{
		int iHealthBefore = m_iHealth;
		m_iHealth += amount;
		m_iHealth = Math.ClampInt(m_iHealth, 0, m_iMaxHealth);
		if(m_iHealth != iHealthBefore)
		{
			// Activate EOnFrame event, run the logic in EOnFrame, then reset it
			SetEventMask(EntityEvent.FRAME);
			m_bHealthChanged = true;
		}
	}
	
	// Sets health to 0
	void Destroy()
	{
		m_iHealth = 0;
		SetEventMask(EntityEvent.FRAME);
		m_bHealthChanged = true;
	}
	
	protected void UpdateHealthLogic()
	{
		if(m_iHealth <= 0)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(this);
		}
		else
		{
			if(!m_bIsBuilt)
			{
				if(m_iHealth >= m_iFinalStageHealth)
				{
					Built();
					Enable(true); // Enable after entity is created
				}
			}
			else
			{
				if(m_iHealth <= m_iDisableHealth)
					Enable(false);
				else
					Enable(true);
			}
		}
		
		Replication.BumpMe();
		
	}
	
	protected void Enable(bool value)
	{
		m_bEnabled = value;
		
		// Call events on event handlers
		array<Managed> eventHandlers = {};
		m_Final.FindComponents(PR_BuildableEventHandlerComponent, eventHandlers);
		foreach (Managed managed : eventHandlers)
		{
			PR_BuildableEventHandlerComponent buildingEventHandler = PR_BuildableEventHandlerComponent.Cast(managed);
			buildingEventHandler.OnEnabled(this, value);
		}
	}
	
	protected void Built()
	{
		m_bIsBuilt = true;
		
		// Delete foundation
		if( m_Foundation )
		{
			SCR_EntityHelper.DeleteEntityAndChildren(m_Foundation);
		}
			
		// Create final
		m_Final = CreateStage(m_sFinalPrefab);
		
		// Call events on event handlers
		
		// At this moment it might be not initialized
		if (m_bInitialized)
			CallFinalInitEvents();
	}
	
	protected void CallFinalInitEvents()
	{
		array<Managed> eventHandlers = {};
		m_Final.FindComponents(PR_BuildableEventHandlerComponent, eventHandlers);
		foreach (Managed managed : eventHandlers)
		{
			PR_BuildableEventHandlerComponent buildingEventHandler = PR_BuildableEventHandlerComponent.Cast(managed);
			buildingEventHandler.OnBuild(this);
		}
	}
}