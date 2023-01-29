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