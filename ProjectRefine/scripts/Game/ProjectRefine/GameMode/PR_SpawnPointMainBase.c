// Spawn point which is attached to main base

class PR_SpawnPointMainBaseClass : PR_BaseSpawnPointClass
{
}

class PR_SpawnPointMainBase : PR_BaseSpawnPoint
{
	protected PR_CaptureArea m_CaptureArea;
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		m_fRespawnWaveInterval_ms = 7000.0;
		
		m_CaptureArea = PR_CaptureArea.Cast(owner.FindComponent(PR_CaptureArea));
	}
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		RplComponent rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
		
		if (rpl.IsMaster())
		{
			m_eStateFlags |= PR_ESpawnPointStateFlags.ACTIVE; // Always active
			Replication.BumpMe();
		}
	}
	
	//! Must return ID of owner faction
	override int GetOwnerFactionId()
	{
		return m_CaptureArea.GetOwnerFactionId();
	}
	
	override string GetName()
	{
		return "Main Base";
	}
}