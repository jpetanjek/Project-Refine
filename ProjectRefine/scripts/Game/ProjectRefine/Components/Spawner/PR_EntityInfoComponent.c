[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class PR_EntityInfoComponentClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
/*!
	Component storing some data about asset.
*/
class PR_EntityInfoComponent : ScriptComponent
{
	protected PR_EAssetType m_AssetType = -1;
	
	protected int m_iInitialFactionId = -1;

	void Init(PR_EAssetType assetType, int factionId)
	{
		m_AssetType = assetType;
		m_iInitialFactionId = factionId;
	}
	
	PR_EAssetType GetAssetType() { return m_AssetType; }
	
	int GetInitialFactionId() { return m_iInitialFactionId; }
		
	/*
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
	void PR_AssetInfoComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~PR_AssetInfoComponent()
	{
	}
	*/
};
