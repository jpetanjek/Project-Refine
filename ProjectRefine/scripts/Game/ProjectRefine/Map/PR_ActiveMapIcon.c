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
	
	protected MapDescriptorComponent m_MapDescriptor;
	
	protected int m_iRefreshCounter;
	
	
	
	//------------------------------------------------------------------------------------------------
	// RPL PROPS
	
	// Replicated vector which stores position without vertical component and direction
	// Format: [x pos, dir angle, z pos]
	// TODO: Make a custom struct & codec with limited precision to save traffic
	[RplProp(onRplName: "OnPosAndDirChange")]
	protected vector m_vPosAndDir;

	
	// Faction ID for which this icon is relevant. -1 means it's for all factions.
	[RplProp()]
	protected int m_iFactionId;
	
	// Group ID for which this icon is relevant. -1 means it's for all groups.
	[RplProp()]
	protected int m_iGroupId;
	
	//------------------------------------------------------------------------------------------------
	// ATTRIBUTES
	
	[Attribute(desc: "Style of the icon")]
	protected ref PR_ActiveMapIconStyleBase m_Style;
	
	[Attribute("0", UIWidgets.CheckBox, desc: "When true, this map icon will keep faction ID updated according to faction of target")]
	protected bool m_bTrackTargetFaction;
	
	//------------------------------------------------------------------------------------------------
	// PUBLIC
	
	void SetTarget(IEntity target)
	{
		m_Target = target;
		UpdatePosAndDirPropFromTarget();
		SetTransformFromPosAndDirProp();
	}
	
	
	
	//------------------------------------------------------------------------------------------------
	// PROTECTED
	
	
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
		
		
		// Master will track target's state
		if (rpl.IsMaster())
		{
			SetEventMask(EntityEvent.INIT | EntityEvent.FIXEDFRAME);
		}
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
		if(m_iRefreshCounter++ > POSITION_UPDATE_INTERVAL && m_Target)
		{
			UpdatePosAndDirPropFromTarget();
			Replication.BumpMe();
			if (!System.IsConsoleApp())
				SetTransformFromPosAndDirProp(); // Makes no sense for headless
			
			m_iRefreshCounter = 0;
		}	
	}
	
	// Called by replication on m_vPosAndDir change
	protected void OnPosAndDirChange()
	{
		SetTransformFromPosAndDirProp();
	}
}