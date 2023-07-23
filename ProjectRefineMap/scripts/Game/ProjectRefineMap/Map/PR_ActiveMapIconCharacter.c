enum PR_ECharacterIconFlags
{
	LEADER		= 1<<0,	// True when this character is leader of its group
	UNCONSCIOUS = 1<<1	// True when unconscious
}

class PR_ActiveMapIconCharacterClass : PR_ActiveMapIconClass
{
}

class PR_ActiveMapIconCharacter : PR_ActiveMapIcon
{
	// ID of player who controls this character
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected int m_iPlayerId;
	
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected PR_ECharacterIconFlags m_Flags;
	
	// Id of parent group of this character
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected int m_iParentGroupId;
	
	protected AIAgent m_AIAgent;
	protected CharacterControllerComponent m_CharacterController;
	
	// Map descriptor which makes an icon to highlight current player
	protected MapDescriptorComponent m_MapDescriptorPlayerHighlight;
	
	[Attribute()]
	ref PR_MapIconStyleBase m_StylePlayerHighlight;
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!GetGame().GetWorldEntity())
  			return;
		
		array<Managed> components = {};
		FindComponents(MapDescriptorComponent, components);
		m_MapDescriptorPlayerHighlight = MapDescriptorComponent.Cast(components[1]);
		
		if (m_StylePlayerHighlight)
			m_StylePlayerHighlight.Apply(m_MapDescriptorPlayerHighlight);
	}
	
	override void OnTargetAssigned(IEntity target)
	{
		AIControlComponent aiControlComponent = AIControlComponent.Cast(target.FindComponent(AIControlComponent));
		m_AIAgent = aiControlComponent.GetAIAgent();
		m_CharacterController = CharacterControllerComponent.Cast(target.FindComponent(CharacterControllerComponent));
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
			SCR_AIGroup aiGroup = SCR_AIGroup.Cast(m_AIAgent.GetParentGroup());
			if (aiGroup)
			{
				AIAgent leader = aiGroup.GetLeaderAgent();
				isLeader = leader == m_AIAgent;
				m_iParentGroupId = aiGroup.GetGroupID();
			}
		}
		
		if (isLeader)
			m_Flags |= PR_ECharacterIconFlags.LEADER;
		else
			m_Flags &= ~(PR_ECharacterIconFlags.UNCONSCIOUS);
		
		// Check if we are unconscious
		if (m_CharacterController)
		{
			if (m_CharacterController.IsUnconscious())
				m_Flags |= PR_ECharacterIconFlags.UNCONSCIOUS;
			else
				m_Flags &= ~(PR_ECharacterIconFlags.UNCONSCIOUS);
		}
	}
	
	override protected void UpdateFromReplicatedState()
	{
		super.UpdateFromReplicatedState();
		
		MapItem mapItem = m_MapDescriptor.Item();
		MapDescriptorProps props = mapItem.GetProps();
		
		SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		bool controlledByMe = m_iPlayerId != 0 && pc && pc.GetPlayerId() == m_iPlayerId;
			 
		
		// Display name
		if (m_iPlayerId != 0 && !controlledByMe) // 0 is invalid ID of player, not -1
			mapItem.SetDisplayName(GetGame().GetPlayerManager().GetPlayerName(m_iPlayerId));
		else
			mapItem.SetDisplayName(string.Empty);
		
		// Icon color
		Color color;
		Faction faction = GetGame().GetFactionManager().GetFactionByIndex(m_iFactionId);
		if (faction)
			color = faction.GetFactionColor();
		else
			color = Color.White;
		props.SetFrontColor(color);
		
		// Check our group ID
		int myGroupId = -1;
		
		if (pc && pc.m_GroupComponent)
			myGroupId = pc.m_GroupComponent.GetGroupID();
		
		// Icon image
		string imageDef;
		if (m_Flags & PR_ECharacterIconFlags.UNCONSCIOUS)
		{
			imageDef = "unconscious";
			mapItem.SetAngle(0); // OVerride angle, set it back to 0
		}
		else if (myGroupId == m_iParentGroupId && myGroupId != -1)
		{
			if (m_Flags & PR_ECharacterIconFlags.LEADER)
				imageDef = "character_leader_same_group";
			else
				imageDef = "character_same_group";
		}
		else
		{
			if (m_Flags & PR_ECharacterIconFlags.LEADER)
				imageDef = "character_leader";
			else
				imageDef = "character";
		}
		
		mapItem.SetImageDef(imageDef);
		
		// Font color
		// This is already set in style, however for some reason text is rendered grey sometimes, let's see if it helps if we also set it here
		// todo remove this
		props.SetTextColor(Color.Black);
		
		//------------------------------------
		// Player highlight
		int myPlayerId = 0;
		if (pc)
			myPlayerId = pc.GetPlayerId();
		
		MapItem mapItemPlayerHighlight = m_MapDescriptorPlayerHighlight.Item();
		MapDescriptorProps propsPlayerHighlight = mapItemPlayerHighlight.GetProps();
		propsPlayerHighlight.SetVisible(controlledByMe);
		if (controlledByMe)
		{
			mapItemPlayerHighlight.SetAngle(Math.RAD2DEG * m_vPosAndDir[1]); 
		}
	}
}