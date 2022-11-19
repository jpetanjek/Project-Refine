[EntityEditorProps(category: "GameScripted/GameMode", description: "Universal map icon entity", visible: false)]
class PR_ActiveMapIconClass : SCR_PositionClass
{
};

//------------------------------------------------------------------------------------------------
//! Entity which follows its target and is drawn on the map via the SCR_MapDescriptorComponent
class PR_ActiveMapIcon : SCR_Position
{
	// How often to update position
	protected static const int POSITION_UPDATE_INTERVAL = 30;
	
	// Target entity which we follow
	protected IEntity m_Target;
	protected bool m_bTargetAssigned = false;
	
	protected MapDescriptorComponent m_MapDescriptor;
	
	protected int m_iRefreshCounter;
	
	
	
	//------------------------------------------------------------------------------------------------
	// RPL PROPS
	
	// Replicated vector which stores position without vertical component and direction
	// Format: [x pos, dir angle, z pos]
	// TODO: Make a custom struct & codec with limited precision to save traffic
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected vector m_vPosAndDir;

	
	// Faction ID for which this icon is relevant. -1 means it's for all factions.
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected int m_iFactionId;
	
	// Group ID for which this icon is relevant. -1 means it's for all groups.
	[RplProp(onRplName: "UpdateFromReplicatedState")]
	protected int m_iGroupId;
	
	//------------------------------------------------------------------------------------------------
	// ATTRIBUTES
	
	[Attribute(desc: "Style of the icon")]
	protected ref PR_ActiveMapIconStyleBase m_Style;
	
	[Attribute("0", UIWidgets.CheckBox, desc: "When true, this map icon will keep faction ID updated according to faction of target")]
	protected bool m_bTrackTargetFaction;
	
	//------------------------------------------------------------------------------------------------
	// PUBLIC
	
	// Initializes this icon
	// If target is not null, we start tracking the target
	// If target is null, we just set position explicitly
	void Init(IEntity target, vector pos = vector.Zero, int factionId = -1, int groupId = -1)
	{
		m_Target = target;
		
		// We get position from target or set it explicitly
		if (target)
		{
			UpdatePosAndDirPropFromTarget();
			m_bTargetAssigned = true;
		}
		else
		{
			m_vPosAndDir[0] = pos[0];
			m_vPosAndDir[1] = 0;
			m_vPosAndDir[2] = pos[2];
		}
		
		m_iFactionId = factionId;
		m_iGroupId = groupId;
		
		UpdateFromReplicatedState();
		Replication.BumpMe();
		
		// Master will track target's state
		RplComponent rpl = RplComponent.Cast(FindComponent(RplComponent));
		if (target && rpl.IsMaster())
		{
			// If we don't have a target to track then we don't need fixed frame
			SetEventMask(EntityEvent.INIT | EntityEvent.FIXEDFRAME);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	// PROTECTED
	
	// Also called by replication when replicated state changes
	// This function must implement update of local state from replicated properties
	// Override this in inherited classes for custom functionality.
	// But remember to call this method of parent class!
	protected void UpdateFromReplicatedState()
	{
		SetTransformFromPosAndDirProp();
	}
	
	// Updates RPL prop member variables from target.
	// Override in inherited classes, but remember to call parent class method!
	protected void UpdatePropsFromTarget()
	{
		UpdatePosAndDirPropFromTarget();
	}
	
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		if (!GetGame().GetWorldEntity())
  			return;

		m_MapDescriptor = MapDescriptorComponent.Cast(owner.FindComponent(MapDescriptorComponent));
		
		RplComponent rpl = RplComponent.Cast(FindComponent(RplComponent));
		if (rpl)
		{
			rpl.InsertToReplication();
			RplId id = Replication.FindId(this);
		}
		
		if (m_Style)
			m_Style.Apply(this, m_MapDescriptor);
	}
	
	//------------------------------------------------------------------------------------------------
	void PR_ActiveMapIcon(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
		SetFlags(EntityFlags.ACTIVE, true);
	}
	
	//------------------------------------------------------------------------------------------------
	//void ~PR_ActiveMapIcon()
	//{
	//}
	
	
	// Converts target's XZ position and direction into one vector
	protected void UpdatePosAndDirPropFromTarget()
	{
		vector worldTransform[4];
		m_Target.GetWorldTransform(worldTransform);
		float dirAngle = Math.Atan2(worldTransform[2][0], worldTransform[2][2]); // Radians
		m_vPosAndDir = Vector(worldTransform[3][0], dirAngle, worldTransform[3][2]); // X, Dir, Z
	}
	
	// Sets this entity transform and map icon rotation from pos and dir vector
	protected void SetTransformFromPosAndDirProp()
	{
		// Set X,Z of this entity origin.
		// Don't set entity rotation from angle, we don't care where about this entity rotation
		vector newPos = m_vPosAndDir;
		newPos[1] = 0;
		SetOrigin(newPos);
		
		// But update marker direction
		m_MapDescriptor.Item().SetAngle(Math.RAD2DEG * m_vPosAndDir[1]); 
	}
	
	override protected void EOnFixedFrame(IEntity owner, float timeSlice)
	{
		if(m_iRefreshCounter++ > POSITION_UPDATE_INTERVAL && m_bTargetAssigned)
		{
			if (m_Target)
			{
				UpdatePropsFromTarget();
				Replication.BumpMe();
				if (!System.IsConsoleApp())
					UpdateFromReplicatedState(); // Makes no sense if we have no UI
			}
			else
			{
				// Target was deleted
				// TODO: delete ourselves			
			}
			
			m_iRefreshCounter = 0;
		}
	}
}