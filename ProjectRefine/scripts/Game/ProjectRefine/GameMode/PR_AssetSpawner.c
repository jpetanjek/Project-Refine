[EntityEditorProps(category: "GameScripted/GameMode", description: "Runtime vehicle spawner", visible: false)]
class PR_AssetSpawnerClass : GenericEntityClass
{
};

//------------------------------------------------------------------------------------------------
//! Entity which follows its target and is drawn on the map via the SCR_MapDescriptorComponent
class PR_AssetSpawner : GenericEntity
{
	[Attribute(desc: "List of entities that can be spawned", category: "Asset Spawner")]
	protected ref PR_EntitySpawnerList m_EntityAssetList;
	
	[Attribute("", UIWidgets.Flags, "", category: "Asset Spawner", enums: ParamEnumArray.FromEnum(PR_EAssetType))]
	protected PR_EAssetType m_eSupportedEntities;
	
	protected ref array<ResourceName> m_aSpawn = {};
	
	// Spawned vehicle
	protected IEntity m_Target;
	
	// Assigned Capture Area
	protected PR_CaptureArea m_CaptureArea;
	
	override void EOnInit(IEntity owner)
	{
		if (!GetGame().GetWorldEntity())
  			return;
		
		// TODO: Runs only on server
		if(Replication.IsClient())
			return;
		
		if(GetParent())		
		{
			m_CaptureArea = PR_CaptureArea.Cast(GetParent().FindComponent(PR_CaptureArea));
			SpawnActions();
		}
		else
		{	
			// Assert - cannot work unless it has assigned Caputre Area
		}
	}
	
	void OnDamageStateChanged(EDamageState state)
	{
		if (state == EDamageState.DESTROYED)
		{
			SpawnActions();
		}
	}
	
	void SpawnActions()
	{
		EntitySpawnParams spawnParams = EntitySpawnParams();	
		spawnParams.TransformMode = ETransformMode.WORLD;
		GetWorldTransform(spawnParams.Transform);
		
		// Find which entity can spawn here		
		for (int i = 0; i < m_EntityAssetList.GetEntryCount(); i++)
		{
			PR_EntitySpawnInfo entitySpawnInfo = m_EntityAssetList.GetEntryAtIndex(i);
			int wha = m_CaptureArea.GetOwnerFactionId();
			int wha2 = entitySpawnInfo.m_iFaction;
			
			if ( entitySpawnInfo && entitySpawnInfo.m_eAssetType == m_eSupportedEntities && m_CaptureArea && m_CaptureArea.GetOwnerFactionId() == entitySpawnInfo.m_iFaction)
			{		
				ResourceName toInsert = entitySpawnInfo.GetPrefab();
				m_aSpawn.Insert(toInsert);
			}
		}
		
		m_Target = IEntity.Cast(GetGame().SpawnEntityPrefab( Resource.Load(m_aSpawn.Get(m_aSpawn.GetRandomIndex())), GetGame().GetWorld(), spawnParams));
		
		ScriptedDamageManagerComponent m_pDamageManager = ScriptedDamageManagerComponent.Cast(m_Target.FindComponent(ScriptedDamageManagerComponent));
		if (m_pDamageManager)
			m_pDamageManager.GetOnDamageStateChanged().Insert(OnDamageStateChanged);
	}
	
	void PR_AssetSpawner(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
		SetFlags(EntityFlags.STATIC, true);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~PR_AssetSpawner()
	{
		
	}
}