[EntityEditorProps(category: "GameScripted/GameMode", description: "Universal map icon entity", visible: false)]
class PR_ActiveMapIconClass : SCR_PositionClass
{
};

//------------------------------------------------------------------------------------------------
//! Entity which follows its target and is drawn on the map via the SCR_MapDescriptorComponent
class PR_ActiveMapIcon : SCR_Position
{
	[Attribute(desc: "UI info for icon")]
	protected ref SCR_UIInfo m_Icon;
	
	protected IEntity m_Target;
	
	protected int m_iRefreshCounter;
	
	// some replicated vector in which we will store x,y,rot - repVec - add a OnRep
	// TODO: Make a custom struct & codec with limited precision to save traffic
	[RplProp(onRplName: "PositionChange")]
	protected vector position;
	
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!GetGame().GetWorldEntity())
  			return;

		RplComponent rpl = RplComponent.Cast(FindComponent(RplComponent));
		if (rpl)
		{
			rpl.InsertToReplication();
			RplId id = Replication.FindId(this);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void PR_ActiveMapIcon(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT | EntityEvent.FIXEDFRAME | EntityEvent.FRAME);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~PR_ActiveMapIcon()
	{
	}
	
	void SetTarget(IEntity target)
	{
		m_Target = target;
		vector worldTransform[4];
		m_Target.GetWorldTransform(worldTransform);
		this.SetWorldTransform(worldTransform);
		position = Vector( worldTransform[3][0], worldTransform[3][1], worldTransform[2][0]); 
	}
	
	void SetIcon(SCR_UIInfo icon)
	{
		m_Icon = icon;
		// Find SCR_MapDescriptorComponent and set icon I guess?
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		bool wtf = 0;
	}
	
	override void EOnFixedFrame(IEntity owner, float timeSlice)
	{
		if(m_iRefreshCounter++ > 30 && m_Target != null)
		{
			vector worldTransform[4];
			m_Target.GetWorldTransform(worldTransform);
			this.SetWorldTransform(worldTransform);
			position = Vector( worldTransform[3][0], worldTransform[3][1], worldTransform[2][0]); 
			Replication.BumpMe()
		}	
	}
	
	void PositionChange()
	{
		vector worldTransform[4];
		worldTransform[3][0] = position[0];
		worldTransform[3][1] = position[1];
		worldTransform[2][0] = position[2];
		this.SetWorldTransform(worldTransform);
	}
}