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
		
		m_CaptureArea = PR_CaptureArea.Cast(owner.FindComponent(PR_CaptureArea));
	}
	
	//! Must return ID of owner faction
	override int GetOwnerFactionId()
	{
		return m_CaptureArea.GetOwnerFactionId();
	}
}