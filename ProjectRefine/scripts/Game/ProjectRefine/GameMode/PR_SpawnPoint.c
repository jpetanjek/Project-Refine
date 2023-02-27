[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class PR_SpawnPointClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
/*!
Component which represents spawn point functionality.
*/
class PR_SpawnPoint : ScriptComponent
{
	protected PR_CaptureArea m_CaptureArea;

	protected static ref array<PR_SpawnPoint> s_aAll = {};
	
	//------------------------------------------------------------------------------------------------
	static array<PR_SpawnPoint> GetAll()
	{
		array<PR_SpawnPoint> a = {};
		a.Copy(s_aAll);
		return a;
	}
	
	//------------------------------------------------------------------------------------------------
	// Returns ID of faction which is allowed to spawn here
	int GetFactionId()
	{
		return m_CaptureArea.GetOwnerFactionId(); // For now we delegate it to spawn point
	}
	
	
	
	//------------------------------------------------------------------------------------------------
	void PR_SpawnPoint(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		s_aAll.Insert(this);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~PR_SpawnPoint()
	{
		s_aAll.RemoveItem(this);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT); // | EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		m_CaptureArea = PR_CaptureArea.Cast(owner.FindComponent(PR_CaptureArea));
	}
	
	
	/*
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
	}

	
	*/
};
