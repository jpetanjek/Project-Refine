[EntityEditorProps(category: "GameScripted/GameMode", description: "Runtime vehicle spawner", visible: false)]
class PR_AssetSpawnerClass : GenericEntityClass
{
};

//------------------------------------------------------------------------------------------------
//! Entity which follows its target and is drawn on the map via the SCR_MapDescriptorComponent
class PR_AssetSpawner : GenericEntity
{	
	[Attribute("", UIWidgets.Flags, "", category: "Asset Spawner", enums: ParamEnumArray.FromEnum(PR_EAssetType))]
	protected PR_EAssetType m_eSupportedEntities;
	
	// TODO: move this to prefab
	[Attribute("20", UIWidgets.Slider, "Wait time", "0 1200 1")]
	private float m_fWaitTime;
	
	protected ref array<ResourceName> m_aSpawn = {};
	
	protected bool m_bDestroyed = false;
	protected float m_fTimer = 0;
	
	// Spawned vehicle
	protected IEntity m_Target;
	
	// Assigned Capture Area
	protected PR_CaptureArea m_CaptureArea;
	
	//------------------------------------------------------------------------------------------------	
	void PR_AssetSpawner(IEntitySource src, IEntity parent)
	{
		SetFlags(EntityFlags.ACTIVE, true);
	
		if(Replication.IsClient())
			return;
		
		SetEventMask(EntityEvent.INIT | EntityEvent.FRAME);	
	}
	
	//------------------------------------------------------------------------------------------------
	void ~PR_AssetSpawner()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		if (!GetGame().GetWorldEntity())
  			return;
	}

	//------------------------------------------------------------------------------------------------	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		// First spawn
		if(!m_Target && !m_bDestroyed)
		{
			if(GetParent())		
			{
				m_CaptureArea = PR_CaptureArea.Cast(GetParent().FindComponent(PR_CaptureArea));
				m_bDestroyed = false;
				SpawnActions();
			}
			else
			{	
				// Assert - cannot work unless it has assigned Caputre Area
			}
		}
		// Subsequent
		if(m_fTimer >= m_fWaitTime && m_bDestroyed)
		{
			m_bDestroyed = false;
			SpawnActions();
		}
		if(m_bDestroyed)
		{
			m_fTimer += timeSlice;
		}
	}
		
	
	void OnDamageStateChanged(EDamageState state)
	{
		if (state == EDamageState.DESTROYED)
		{
			m_fTimer = 0;
			m_bDestroyed = true;
		}
	}
	
	void SpawnActions()
	{
		EntitySpawnParams spawnParams = EntitySpawnParams();	
		spawnParams.TransformMode = ETransformMode.WORLD;
		GetWorldTransform(spawnParams.Transform);
		
		FactionManager factionManager = GetGame().GetFactionManager();
		if (factionManager)
		{
			SCR_Faction faction = SCR_Faction.Cast(factionManager.GetFactionByIndex(m_CaptureArea.GetOwnerFactionId()));
			if(faction)
			{
				PR_EntityAssetList fullAssetList = faction.GetAssetList();
				if(!fullAssetList)
					return;
				
				array<PR_EntityInfo> assetList = {};
				fullAssetList.GetAssetList(assetList);
				if(!assetList.IsEmpty())
				{
					m_Target = IEntity.Cast(GetGame().SpawnEntityPrefab(
					Resource.Load(assetList.Get(assetList.GetRandomIndex()).GetPrefab()),
				 	GetGame().GetWorld(),
					spawnParams));
		
					ScriptedDamageManagerComponent m_pDamageManager = ScriptedDamageManagerComponent.Cast(m_Target.FindComponent(ScriptedDamageManagerComponent));
					if (m_pDamageManager)
						m_pDamageManager.GetOnDamageStateChanged().Insert(OnDamageStateChanged);
				}
			}
		}
	}

}