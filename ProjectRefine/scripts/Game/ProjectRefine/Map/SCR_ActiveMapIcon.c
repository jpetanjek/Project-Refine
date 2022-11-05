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
	
	// List of all active map icons
	protected static ref map<SCR_ActiveMapIcon, RplId> s_mActiveMapIcons = new map<SCR_MapIcon, RplId>();
	
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
			s_mActiveMapIcons.Set(this, id);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void SCR_SpawnPoint(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
		SetFlags(EntityFlags.STATIC, true);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~SCR_SpawnPoint()
	{
		if (s_mActiveMapIcons)
		{
			s_mActiveMapIcons.Remove(this);
		}
	}
	
	void SetTarget(IEntity target)
	{
		m_Target = target;
	}
	
	void SetIcon(SCR_UIInfo icon)
	{
		m_Icon = icon;
	}
	
	override void EOnFixedFrame(IEntity owner, float timeSlice)
	{
		if(m_iRefreshCounter++ > 30)
		{
			// repVec.BumpMe();
		}	
	}
	
}