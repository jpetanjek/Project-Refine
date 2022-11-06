[EntityEditorProps(category: "GameScripted/GameMode", description: "Universal map icon entity", visible: false)]
class SCR_ActiveMapIconClass : SCR_PositionClass
{
};

//------------------------------------------------------------------------------------------------
//! Entity which follows its target and is drawn on the map via the SCR_MapDescriptorComponent
class SCR_ActiveMapIcon : SCR_Position
{
	protected ref SCR_UIInfo m_Icon;
	
	protected IEntity m_Target;
	
	protected int m_iRefreshCounter;
	
	// some replicated vector in which we will store x,y,rot - repVec - add a OnRep
	// TODO: Make a custom struct & codec with limited precision to save traffic
	
	
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
	void SCR_ActiveMapIcon(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
		SetFlags(EntityFlags.STATIC, true);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~SCR_ActiveMapIcon()
	{
	}
	
	void SetTarget(IEntity target)
	{
		m_Target = target;
	}
	
	void SetIcon(SCR_UIInfo icon)
	{
		m_Icon = icon;
		// Find SCR_MapDescriptorComponent and set icon I guess?
	}
	
	override void EOnFixedFrame(IEntity owner, float timeSlice)
	{
		if(m_iRefreshCounter++ > 30)
		{
			// repVec.BumpMe();
		}	
	}
	
}