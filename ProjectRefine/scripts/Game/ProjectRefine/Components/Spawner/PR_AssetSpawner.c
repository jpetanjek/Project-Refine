typedef func OnAssetSpawnedDelegate;
void OnAssetSpawnedDelegate(PR_AssetSpawner spawner, IEntity asset, int factionId);

[EntityEditorProps(category: "GameScripted/GameMode", description: "Runtime vehicle spawner", visible: false, dynamicBox: true)]
class PR_AssetSpawnerClass : GenericEntityClass
{
};
	

//------------------------------------------------------------------------------------------------
//! Entity which follows its target and is drawn on the map via the SCR_MapDescriptorComponent
class PR_AssetSpawner : GenericEntity
{
	// Low frequency update interval of asset spawner
	protected const float UPDATE_PERIOD_S = 1.0;
	
	[Attribute("", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(PR_EAssetType))]
	protected PR_EAssetType m_AssetType;
	
	[Attribute("false", UIWidgets.CheckBox, desc: "Spawn at round start?")]
	protected bool m_bSpawnAtStart;
	
	[Attribute("30", UIWidgets.EditBox, desc: "Respawn time in seconds")]
	protected float m_fRespawnTime_s;
	
	protected float m_fDeltaTime = 0;		// Timer for periodic updates
	
	protected PR_CaptureArea m_CaptureArea;	// Assigned Capture Area
	protected bool m_bDestroyed = false;	// True when asset was destroyed
	protected IEntity m_Target;				// Spawned asset
	protected float m_fTimer_s = 0;			// Timer for respawning
	
	// Events
	ref ScriptInvokerBase<OnAssetSpawnedDelegate> m_OnAssetSpawned = new ScriptInvokerBase<OnAssetSpawnedDelegate>();
	
	//------------------------------------------------------------------------------------------------	
	void PR_AssetSpawner(IEntitySource src, IEntity parent)
	{
		SetFlags(EntityFlags.ACTIVE, true);
	
		EntityEvent mask = 0; //EntityEvent.DIAG;
		if(!Replication.IsClient())
		{
			mask |= EntityEvent.FRAME;
		}
		
		SetEventMask(mask);
	}
	
	//------------------------------------------------------------------------------------------------	
	// Called by Capture Area which owns this
	void Init(PR_CaptureArea captureArea)
	{
		m_CaptureArea = captureArea;
	}

	//------------------------------------------------------------------------------------------------	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		// 1s timer
		m_fDeltaTime += timeSlice;
		if (m_fDeltaTime < UPDATE_PERIOD_S)
			return;
		
		// Bail if parent capture area is not assigned. Probably not initialized yet.
		if (!m_CaptureArea)
			return;
		
		if(!m_Target && m_bSpawnAtStart)
		{
			if (TrySpawnAsset())
				m_bDestroyed = false;
		}
			
		if((m_bDestroyed || !m_Target) && m_fTimer_s >= m_fRespawnTime_s)
		{
			if (TrySpawnAsset())
				m_bDestroyed = false;
		}
		
		if(m_bDestroyed || !m_Target)
		{
			m_fTimer_s += m_fDeltaTime;
		}
		
		// At the end of logic reset the low frequency update timer
		m_fDeltaTime -= UPDATE_PERIOD_S;
	}
	
	//------------------------------------------------------------------------------------------------
	void OnDamageStateChanged(EDamageState state)
	{
		if (state == EDamageState.DESTROYED)
		{
			m_fTimer_s = 0;
			m_bDestroyed = true;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool TrySpawnAsset()
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
				PR_AssetList fullAssetList = faction.GetAssetList();
				if(!fullAssetList)
					return false;
				
				array<PR_Asset> assetList = {};
				fullAssetList.GetAssetList(assetList);
				if(assetList.Count() > 0)
				{
					// todo check if area is obstructed
					// if (areaObstructed) return false;
					
					for(int i = assetList.Count() - 1; i >= 0 ; i--)
					{
						PR_Asset asset = assetList.Get(i);
						if(!asset || (asset && asset.GetAssetType() != m_AssetType))
						{
							assetList.Remove(i);
						}
					}
					
					ResourceName resourceName;
					if(assetList.Count() > 0)
					{
						resourceName = assetList.Get(assetList.GetRandomIndex()).GetPrefab();
						m_Target = IEntity.Cast(GetGame().SpawnEntityPrefab(
							Resource.Load(resourceName),
					 		GetGame().GetWorld(),
							spawnParams));
					}
					
					if(m_Target)
					{
						ScriptedDamageManagerComponent damageManager = ScriptedDamageManagerComponent.Cast(m_Target.FindComponent(ScriptedDamageManagerComponent));
						if (damageManager)
							damageManager.GetOnDamageStateChanged().Insert(OnDamageStateChanged);
						else
							_print(string.Format("Didn't find ScriptedDamageManagerComponent on entity: %1", resourceName), LogLevel.ERROR);
						
						PR_AssetInformerComponent entityInfoComp = PR_AssetInformerComponent.Cast(m_Target.FindComponent(PR_AssetInformerComponent));
						if (entityInfoComp)
							entityInfoComp.Init(m_AssetType, m_CaptureArea.GetOwnerFactionId());
						else
							_print(string.Format("Didn't find PR_AssetInformerComponent on entity: %1", resourceName), LogLevel.ERROR);
						
						m_OnAssetSpawned.Invoke(this, m_Target, m_CaptureArea.GetOwnerFactionId());
						
						return true;
					}
					else
						return false;					
				}
			}
		}
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnDiag(IEntity owner, float timeSlice)
	{
		if (DiagMenu.GetBool(SCR_DebugMenuID.REFINE_SHOW_ASSET_SPAWNER_STATE))
		{
			// Draw debug text
			const int COLOR_TEXT = Color.WHITE;
		 	const int COLOR_BACKGROUND = Color.BLACK;
			
			string s;
			s = s + string.Format("Type:        %1\n", typename.EnumToString(PR_EAssetType, m_AssetType));
			s = s + string.Format("Destroyed:   %1\n", m_bDestroyed);
			s = s + string.Format("Target:		%1\n", m_Target);
			s = s + string.Format("Timer:       %1 / %2\n", m_fTimer_s.ToString(5, 1), m_fRespawnTime_s);
			
			vector pos = owner.GetOrigin() + Vector(0, 3, 0);
			DebugTextWorldSpace.Create(GetGame().GetWorld(), s, DebugTextFlags.ONCE, pos[0], pos[1], pos[2], size: 13.0, color: COLOR_TEXT, bgColor: COLOR_BACKGROUND);
			
			DrawDebugLines();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void _WB_AfterWorldUpdate(float timeSlice)
	{
		// Draw debug text
		const int COLOR_TEXT = Color.WHITE;
	 	const int COLOR_BACKGROUND = Color.BLACK;
		
		string s;
		s = s + string.Format("Type:         %1\n", typename.EnumToString(PR_EAssetType, m_AssetType));
		s = s + string.Format("Respawn Time: %1", m_fRespawnTime_s);
		if (m_bSpawnAtStart)
		s = s + string.Format("\nSpawn at start");
		
		vector pos = GetOrigin() + Vector(0, 3, 0);
		DebugTextWorldSpace.Create(GetGame().GetWorld(), s, DebugTextFlags.ONCE, pos[0], pos[1], pos[2], size: 13.0, color: COLOR_TEXT, bgColor: COLOR_BACKGROUND);
		
		DrawDebugLines();
	}
	
	//------------------------------------------------------------------------------------------------s
	void DrawDebugLines()
	{
		// Box size
		const float halfWidth = 3.0 / 2; // Size of our biggest asset - BTR
		const float halfLength = 8.0 / 2;
		const float halfHeight = 3.0 / 2;
		
		vector t[4];
		GetWorldTransform(t);
		vector vside = t[0];
		vector vdir = t[2];
		
		// Draw arrow
		vector arrowOrigin = t[3] + Vector(0, 0.1, 0);
		Shape.CreateArrow(arrowOrigin, arrowOrigin + 3*vdir, 0.8, Color.RED, ShapeFlags.ONCE);
		
		// Draw box
		Shape box = Shape.Create(ShapeType.BBOX, 0x70FF0000, ShapeFlags.ONCE | ShapeFlags.TRANSP, Vector(-halfWidth, -halfHeight, -halfLength), Vector(halfWidth, halfHeight, halfLength));
		vector tbox[4];
		tbox[0] = t[0];
		tbox[1] = t[1];
		tbox[2] = t[2];
		tbox[3] = t[3] + Vector(0, halfHeight, 0);
		box.SetMatrix(tbox);
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	protected static void _print(string str, LogLevel logLevel = LogLevel.NORMAL)
	{
		Print(string.Format("[PR_AssetSpawner] %1", str), logLevel);
	}
}