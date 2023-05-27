typedef func PR_OnFobDestroyed;
void PR_OnFobDestroyed(PR_FobComponent fob);

[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class PR_FobComponentClass : ScriptComponentClass
{
	// prefab properties here
};

// Represents an FOB

class PR_FobComponent : ScriptComponent
{
	protected static ref array<PR_FobComponent> s_aAll = {};
	
	
	[RplProp()]
	protected int m_iOwnerFaction = -1;
	
	protected PR_SupplyHolderComponent m_SupplyHolder;
	
	// Array of all things built here
	protected ref array<PR_BuildingManager> m_aBuildingManagers = {};
	
	//------------------------------------------------------------------------------------------------
	void Init(int ownerFactionId)
	{
		m_iOwnerFaction = ownerFactionId;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	PR_SupplyHolderComponent GetSupplyHolder()
	{
		return m_SupplyHolder;
	}
	
	//------------------------------------------------------------------------------------------------
	array<PR_FobComponent> GetAll()
	{
		return s_aAll;
	}
	
	//------------------------------------------------------------------------------------------------
	static PR_FobComponent FindFobAtPosition(int ownerFactionId, vector posWorld)
	{
		foreach (PR_FobComponent fob : s_aAll)
		{
			if (fob.m_iOwnerFaction != ownerFactionId)
				continue;
			
			if (vector.DistanceXZ(fob.GetOwner().GetOrigin(), posWorld) < fob.GetRange())
				return fob;
		}
		
		return null;
	}
	
	void RegisterBuildingManager(notnull PR_BuildingManager mgr)
	{
		if (m_aBuildingManagers.Contains(mgr))
			return;
		
		m_aBuildingManagers.Insert(mgr);
	}
	
	//------------------------------------------------------------------------------------------------
	float GetRange()
	{
		return m_SupplyHolder.m_fRange;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetOwnerFactionId()
	{
		return m_iOwnerFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		m_SupplyHolder = PR_SupplyHolderComponent.Cast(owner.FindComponent(PR_SupplyHolderComponent));
		if (!m_SupplyHolder)
			Print("No PR_SupplyHolderComponent found on FOB!", LogLevel.ERROR);
	}

	//------------------------------------------------------------------------------------------------
	void PR_FobComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		s_aAll.Insert(this);
	}

	//------------------------------------------------------------------------------------------------
	void ~PR_FobComponent()
	{
		s_aAll.RemoveItem(this);
		
		// Destroy building managers which depend on this FOB
		foreach (PR_BuildingManager mgr : m_aBuildingManagers)
		{
			if (!mgr)
				continue;
			
			if (mgr.GetBuildingFlags() & PR_EAssetBuildingFlags.REQUIRES_FOB)
			{
				mgr.Destroy();
			}
		}
	}

};
