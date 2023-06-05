// Spawn point which is located inside range of an FOB

class PR_SpawnPointFobClass : PR_BaseSpawnPointClass
{
}

class PR_SpawnPointFob : PR_BaseSpawnPoint
{
	protected SCR_FactionAffiliationComponent m_FactionComp;
	
	void Init(int ownerFactionId)
	{
		m_FactionComp.SetAffiliatedFaction(GetGame().GetFactionManager().GetFactionByIndex(ownerFactionId));
		
		m_eStateFlags |= PR_ESpawnPointStateFlags.ACTIVE;
		
		Replication.BumpMe();
	}
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		m_FactionComp = SCR_FactionAffiliationComponent.Cast(owner.FindComponent(SCR_FactionAffiliationComponent));
		if (!m_FactionComp)
			Print("No SCR_FactionAffiliationComponent found on PR_SpawnPointFob!", LogLevel.ERROR);
	}
	
	// Must return ID of owner faction
	override int GetOwnerFactionId()
	{
		return GetGame().GetFactionManager().GetFactionIndex(m_FactionComp.GetAffiliatedFaction());
	}
	
	// Must return name for UI
	override string GetName()
	{
		return "FOB";
	}
}