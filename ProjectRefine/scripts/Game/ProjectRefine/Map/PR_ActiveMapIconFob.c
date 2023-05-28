class PR_ActiveMapIconFobClass : PR_ActiveMapIconClass
{
}

class PR_ActiveMapIconFob : PR_ActiveMapIcon
{
	protected PR_FobComponent m_Fob;
	
	override protected void UpdateFaction(FactionAffiliationComponent factionAffiliationComponent, bool init)
	{
		m_Fob = PR_FobComponent.Cast(m_Target.FindComponent(PR_FobComponent));
		
		if (!m_Fob)
			return;
		
		int ownerFactionId = m_Fob.GetOwnerFactionId();
		
		// During initialization the GetOwnerFactionId() returns -1, since the initialization of FOB happend after its entity constructor
		// Setting m_iFactionId to -1 would show the map icon to everyone, thus we must prevent this
		if (ownerFactionId == -1)
			ownerFactionId = -2;
		
		m_iFactionId = ownerFactionId;
	}
	
	override protected void UpdatePosAndDirPropFromTarget()
	{
		// Same as base class, but we don't copy direction
		vector worldTransform[4];
		m_Target.GetWorldTransform(worldTransform);
		m_vPosAndDir = Vector(worldTransform[3][0], 0, worldTransform[3][2]); // X, Dir, Z
	}
	
	override protected void UpdatePropsFromTarget()
	{
		super.UpdatePropsFromTarget();
	}
}