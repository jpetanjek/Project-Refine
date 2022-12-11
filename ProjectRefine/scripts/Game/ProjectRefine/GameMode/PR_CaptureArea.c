[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class PR_CaptureAreaClass : ScriptComponentClass
{
	// prefab properties here
};

//------------------------------------------------------------------------------------------------
/*!
	Class generated via ScriptWizard.
*/
class PR_CaptureArea : ScriptComponent
{
	// [RplProp()]
	protected int m_iOwnerFaction;

	[Attribute("10", UIWidgets.EditBox, desc: "Radius of capture zone")]
	protected float m_fRadius;
	
	[Attribute("-1", UIWidgets.EditBox, desc: "Owner faction at game start")]
	protected int m_iInitialOwnerFaction;
	
	[Attribute("true", UIWidgets.CheckBox, desc: "True if area is capturable by soldier occupation, false if not.")]
	protected bool m_bCapturable;
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	#ifdef WORKBENCH
	override void _WB_AfterWorldUpdate(IEntity owner, float timeSlice)
	{
		Shape.CreateCylinder(Color.RED, ShapeFlags.VISIBLE | ShapeFlags.ONCE | ShapeFlags.WIREFRAME, owner.GetOrigin(), m_fRadius, 300.0);
	}
	#endif

	//------------------------------------------------------------------------------------------------
	void PR_CaptureArea(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~PR_CaptureArea()
	{
	}

};
