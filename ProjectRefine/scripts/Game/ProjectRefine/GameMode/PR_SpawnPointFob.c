// Spawn point which is located inside range of an FOB

class PR_SpawnPointFobClass : PR_BaseSpawnPointClass
{
}

class PR_SpawnPointFob : PR_BaseSpawnPoint
{
	[RplProp()]
	protected int m_OwnerFactionId; // We store faction when it's built
	
	void Init(int ownerFactionId)
	{
		m_OwnerFactionId = ownerFactionId;
		
		m_eStateFlags |= PR_ESpawnPointStateFlags.ACTIVE;
		
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