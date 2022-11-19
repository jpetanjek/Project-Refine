class PR_ActiveMapIconCharacterClass : PR_ActiveMapIconClass
{
}

class PR_ActiveMapIconCharacter : PR_ActiveMapIcon
{
	// ID of player who controls this character
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected int m_iPlayerId;
	
	override protected void UpdatePropsFromTarget()
	{
		super.UpdatePropsFromTarget();
		
		// Check which player controls this
		m_iPlayerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_Target);
		Replication.BumpMe();
	}
	
	override protected void UpdateFromReplicatedState()
	{
		super.UpdateFromReplicatedState();
		
		MapItem mapItem = m_MapDescriptor.Item();
		if (m_iPlayerId != -1)
			mapItem.SetDisplayName(GetGame().GetPlayerManager().GetPlayerName(m_iPlayerId));
		else
			mapItem.SetDisplayName(string.Empty);
	}
}