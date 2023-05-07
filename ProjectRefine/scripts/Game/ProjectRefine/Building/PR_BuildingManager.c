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
	
	//---------------------
	// Data
	RplComponent m_RplComponent;
	
	IEntity m_Foundation;
	
	IEntity m_Final;
	
	[RplProp()]
	int m_iHealth = 1;
	
	bool m_bEnabled = false;
	
	bool m_bIsBuilt = false;
	
	bool m_bHealthChanged = false;
		
	//---------------------	
	override void EOnInit(IEntity owner)
	{
		m_iHealth = m_iPlacedHealth;
		
		if(m_bPlacedBuilt)
			Built();
		else
			m_Foundation = CreateStage( m_sFoundationPrefab );
		
		m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		
		if(m_RplComponent && !m_RplComponent.IsProxy())
		{
			SetEventMask(EntityEvent.FRAME);
			SetFlags(EntityFlags.ACTIVE, true);
		}
	}
	
	void ~PR_BuildingManager()
	{
		if( m_Foundation )
			delete m_Foundation;
		
		if( m_Final )
			delete m_Final;
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		RuntimeLogic();
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
		Math.ClampInt(m_iHealth, 0, m_iMaxHealth);
		if(m_iHealth != iHealthBefore)
			m_bHealthChanged = true;
	}
	
	protected void RuntimeLogic()
	{
		if(!m_bHealthChanged)
			return;
		
		if(m_iHealth == 0)
		{
			delete this;
		}
		else
		{
			if(!m_bIsBuilt)
			{
				if(m_iHealth >= m_iFinalStageHealth)
				{
					Built();
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
		m_bHealthChanged = false;
		
		Replication.BumpMe();
	}
	
	protected void Enable(bool value)
	{
		m_bEnabled = value;
		// TODO Enable Spawn point
	}
	
	protected void Built()
	{
		Enable(true);
		m_bIsBuilt = true;
		
		// Delete foundation
		if( m_Foundation )
			delete m_Foundation;
		
		// Create final
		m_Final = CreateStage(m_sFinalPrefab);
	}
}