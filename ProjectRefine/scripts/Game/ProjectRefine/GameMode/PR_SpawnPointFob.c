// Spawn point which is located inside range of an FOB

class PR_SpawnPointFobClass : PR_BaseSpawnPointClass
{
}

class PR_SpawnPointFob : PR_BaseSpawnPoint
{
	protected PR_FobComponent m_Fob;
	
	[RplProp()]
	protected int m_OwnerFactionId; // We store faction when it's built

	[RplProp()]
	protected bool m_bActive;
	
	void Init(int ownerFactionId)
	{
		m_Fob = PR_FobComponent.FindFobAtPosition(ownerFactionId, GetOwner().GetOrigin());;
		
		m_OwnerFactionId = ownerFactionId;
		m_bActive = m_Fob != null;
		
		if (m_Fob)
			m_Fob.m_OnDestroyed.Insert(OnFobDestroyed);
		
		Replication.BumpMe();
	}
		
	override bool IsRespawnAllowed()
	{
		// Also check conditions of base class
		return super.IsRespawnAllowed() && m_bActive;
	}
	
	void OnFobDestroyed(PR_FobComponent fob)
	{
		fob.m_OnDestroyed.Remove(OnFobDestroyed);
		
		// Deactivate this spawn point
		m_bActive = false;
		Replication.BumpMe();
	}
	
	// Must return ID of owner faction
	override int GetOwnerFactionId()
	{
		return m_OwnerFactionId;
	}
	
	// Must return name for UI
	override string GetName()
	{
		return "FOB Name Placeholder";
	}
}