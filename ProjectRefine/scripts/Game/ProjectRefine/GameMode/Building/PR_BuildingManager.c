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
	
	[Attribute("1", desc: "Health at which we switch to final prefab and entity is enabled")]
	int m_iFinalStageHealth;
	
	[Attribute("1", desc: "Health at which entity is disabled, should be lower than FinalStageHealth")]
	int m_iDisableHealth;
	
	//---------------------
	// Data
	RplComponent m_RplComponent;
	
	IEntity m_Foundation;
	
	IEntity m_Final;
	
	int m_iHealth = 1;
	
	bool m_bEnabled = false;
	
	bool m_bIsBuilt = false;
	
	bool m_bHealthChanged = false;
	
	float m_fElapsed = 0;
	
	//---------------------	
	override void EOnInit(IEntity owner)
	{
		m_iHealth = m_iPlacedHealth;
		
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
	
	IEntity CreateStage(ResourceName prefab)
	{
		EntitySpawnParams sp;
		sp.TransformMode = ETransformMode.WORLD;
		GetTransform(sp.Transform);
		
		return GetGame().SpawnEntityPrefab(Resource.Load(prefab), GetGame().GetWorld(), sp);
	}
	
	//---------------------
	// Functionality
	
	void Build(int amount)
	{
		m_iHealth += amount;
		Math.ClampInt(m_iHealth, 0, m_iMaxHealth);
		m_bHealthChanged = true;
	}
	
	void RuntimeLogic()
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
					Enable(true);
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
	}
	
	void Enable(bool value)
	{
		m_bEnabled = value;
		// TODO Enable Spawn point
	}
	
	void Built()
	{
		m_bIsBuilt = true;
		// Delete foundation
		if( m_Foundation )
			delete m_Foundation;
		
		// Create final
		m_Final = CreateStage(m_sFinalPrefab);
	}
}