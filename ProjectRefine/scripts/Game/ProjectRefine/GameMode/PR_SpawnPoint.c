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
	protected static ref array<PR_SpawnPoint> s_aAll = {};
	
	// Spawn positions for characters
	protected ref array<PR_CharacterSpawnPosition> m_aSpawnPositions = {};

	protected PR_CaptureArea m_CaptureArea; // Temporary, might get removed later
		
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
	// Even if faction owns a spawn point, respawn there might be blocked for various reasons.
	bool GetRespawnAllowed()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	// Returns spawn point name
	string GetName()
	{
		return m_CaptureArea.GetName();
	}
	
	//------------------------------------------------------------------------------------------------
	// Returns world space coordinate of a random spawn position
	vector GetRandomSpawnPosition()
	{
		if (m_aSpawnPositions.IsEmpty())
			return GetOwner().GetOrigin();
		
		int id = Math.RandomInt(0, m_aSpawnPositions.Count());
		return m_aSpawnPositions[id].GetOrigin();
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
		
		// Search for PR_CharacterSpawnPosition entities and pass them to spawn point
		IEntity childEntity = GetOwner().GetChildren();
		int nCharSpawnPosAdded = 0;
		while (childEntity)
		{
			PR_CharacterSpawnPosition spawnPos = PR_CharacterSpawnPosition.Cast(childEntity);
			if (spawnPos)
			{
				m_aSpawnPositions.Insert(spawnPos);
				nCharSpawnPosAdded++;
			}
			childEntity = childEntity.GetSibling();
		}
		
		if (nCharSpawnPosAdded == 0)
			_print("No character spawn positions found!", LogLevel.ERROR);
	}
	
	
	/*
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
	}

	
	*/

	//-------------------------------------------------------------------------------------------------------------------------------
	protected void _print(string str, LogLevel logLevel = LogLevel.NORMAL)
	{
		Print(string.Format("[PR_SpawnPoint] %1: %2", GetName(), str), logLevel);
	}
};
