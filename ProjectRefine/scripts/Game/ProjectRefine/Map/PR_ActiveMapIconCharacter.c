class PR_ActiveMapIconCharacterClass : PR_ActiveMapIconClass
{
}

class PR_ActiveMapIconCharacter : PR_ActiveMapIcon
{
	// ID of player who controls this character
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected int m_iPlayerId;
	
	// True when this character is leader of its group
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected bool m_bIsLeader;
	
	protected AIControlComponent m_AIControlComponent;
	protected AIAgent m_AIAgent;
	
	
	override void OnTargetAssigned(IEntity target)
	{
		m_AIControlComponent = AIControlComponent.Cast(target.FindComponent(AIControlComponent));
		m_AIAgent = m_AIControlComponent.GetAIAgent();
	}
	
	override protected void UpdatePropsFromTarget()
	{
		super.UpdatePropsFromTarget();
		
		// Check which player controls this
		m_iPlayerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_Target);
		Replication.BumpMe();
		
		// Check if we are a leader of our group
		bool isLeader = false;
		if (m_AIAgent)
		{
			AIGroup aiGroup = m_AIAgent.GetParentGroup();
			if (aiGroup)
			{
				AIAgent leader = aiGroup.GetLeaderAgent();
				isLeader = leader == m_AIAgent;
			}
		}
		m_bIsLeader = isLeader;
	}
	
	override protected void UpdateFromReplicatedState()
	{
		super.UpdateFromReplicatedState();
		
		MapItem mapItem = m_MapDescriptor.Item();
		MapDescriptorProps props = mapItem.GetProps();
		
		// Display name
		if (m_iPlayerId != -1)
			mapItem.SetDisplayName(GetGame().GetPlayerManager().GetPlayerName(m_iPlayerId));
		else
			mapItem.SetDisplayName(string.Empty);
		
		// Icon image
		string imageDef;
		if (m_bIsLeader)
			imageDef = "character_leader";
		else
			imageDef = "character";
		
		mapItem.SetImageDef(imageDef);
		
		// Icon color
		Color color;
		Faction faction = GetGame().GetFactionManager().GetFactionByIndex(m_iFactionId);
		if (faction)
			color = faction.GetFactionColor();
		else
			color = Color.White;
		props.SetFrontColor(color);
	}
}