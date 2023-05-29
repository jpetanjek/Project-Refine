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
	
	protected PR_SupplyHolderComponent m_SupplyHolder;
	protected SCR_FactionAffiliationComponent m_FactionComp;
	
	// Array of all things built here
	protected ref array<PR_BuildingManager> m_aBuildingManagers = {};
	
	//------------------------------------------------------------------------------------------------
	void Init(int ownerFactionId)
	{
		SetOwnerFaction(ownerFactionId);
		
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
	static PR_FobComponent FindFobAtPosition(int ownerFactionId, vector posWorld, bool doubleRange = false)
	{
		float rangeMult = 1.0;
		if (doubleRange)
			rangeMult = 2.0;
		
		Faction faction = GetGame().GetFactionManager().GetFactionByIndex(ownerFactionId);
		if (!faction)
			return null;
		
		foreach (PR_FobComponent fob : s_aAll)
		{
			if (fob.m_FactionComp.GetAffiliatedFaction() != faction)
				continue;
			
			if (vector.DistanceXZ(fob.GetOwner().GetOrigin(), posWorld) < rangeMult * fob.GetRange())
				return fob;
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
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
	protected void SetOwnerFaction(int factionId)
	{
		Faction faction = GetGame().GetFactionManager().GetFactionByIndex(factionId);
		m_FactionComp.SetAffiliatedFaction(faction);
	}
	
	//------------------------------------------------------------------------------------------------
	int GetOwnerFactionId()
	{
		return GetGame().GetFactionManager().GetFactionIndex(m_FactionComp.GetAffiliatedFaction());
	}
	
	//------------------------------------------------------------------------------------------------
	Faction GetOwnerFaction()
	{
		return m_FactionComp.GetAffiliatedFaction();
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
		
		m_FactionComp = SCR_FactionAffiliationComponent.Cast(owner.FindComponent(SCR_FactionAffiliationComponent));
		if (!m_FactionComp)
			Print("No SCR_FactionAffiliationComponent found on FOB!", LogLevel.ERROR);
		
		m_SupplyHolder = PR_SupplyHolderComponent.Cast(owner.FindComponent(PR_SupplyHolderComponent));
		if (!m_SupplyHolder)
			Print("No PR_SupplyHolderComponent found on FOB!", LogLevel.ERROR);
	}

	//------------------------------------------------------------------------------------------------
	//override void EOnInit(IEntity owner)
	//{
	//}

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
