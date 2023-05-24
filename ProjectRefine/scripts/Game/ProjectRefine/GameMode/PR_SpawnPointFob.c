// Spawn point which is located inside range of an FOB

class PR_SpawnPointFobClass : PR_BaseSpawnPointClass
{
}

class PR_SpawnPointFob : PR_BaseSpawnPoint
{
	protected PR_FobComponent m_Fob;
	
	[RplProp()]
	protected int m_OwnerFactionId; // We store faction when it's built
	
	void Init(int ownerFactionId)
	{
		m_Fob = PR_FobComponent.FindFobAtPosition(ownerFactionId, GetOwner().GetOrigin());;
		
		m_OwnerFactionId = ownerFactionId;
		
		if (m_Fob)
		{
			m_Fob.m_OnDestroyed.Insert(OnFobDestroyed);
			m_eStateFlags |= PR_ESpawnPointStateFlags.ACTIVE;
		}
		
		Replication.BumpMe();
	}
	
	void OnFobDestroyed(PR_FobComponent fob)
	{
		fob.m_OnDestroyed.Remove(OnFobDestroyed);
		
		// Deactivate this spawn point
		m_eStateFlags &= ~(PR_ESpawnPointStateFlags.ACTIVE);
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