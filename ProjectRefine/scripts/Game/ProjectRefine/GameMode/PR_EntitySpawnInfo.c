/*
Class to be used for config, used to contain info about what to spawn and under which conditions
*/

[BaseContainerProps(configRoot:true), SCR_BaseContainerCustomTitleResourceName("m_sPrefab", true)]
class PR_EntitySpawnInfo
{
	[Attribute("1", desc: "By default disabled entries are ignored.", category: "Entity Spawn Info")]
	protected bool m_bEnabled;
	
	[Attribute(desc: "The prefab spawned", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "et", category: "Entity Spawn Info")]
	ResourceName m_sPrefab;
	
	[Attribute("", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(PR_EPhase))]
	PR_EPhase m_ePhase;
	
	[Attribute("", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(PR_EAssetType))]
	PR_EAssetType m_eAssetType;
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get if entry is enabled. This is an easy way to disable certain entiries
	\return bool returns true if enabled
	*/
	bool GetEnabled()
	{
		return m_bEnabled;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get the prefab of the entity
	\return ResourceName Entity Prefab
	*/
	ResourceName GetPrefab()
	{
		return m_sPrefab;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns phase which this asset belongs to
	*/
	PR_EPhase GetPhase()
	{
		return m_ePhase;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns asset type
	*/
	PR_EAssetType GetAssetType()
	{
		return m_eAssetType;
	}
	
	//------------------------------------------------------------------------------------------------
	void PR_EntitySpawnInfo()
	{}
	
	//------------------------------------------------------------------------------------------------
	void PR_EntitySpawnInfo(ResourceName prefab, bool enabled = true)
	{
		m_sPrefab = prefab;
		m_bEnabled = enabled;
	}

}

enum PR_EPhase
{
	RECON = 0,
	LIGHT = 1,
	MEDIUM = 2,
	HEAVY = 3
}

enum PR_EAssetType
{
	TRANSPORT = 0,
	ARMORED_TRANSPORT = 1,
	TROOP_TRANSPORT = 2,
	ARMED_TRANSPORT = 3,
	FUEL = 4,
	SUPPLY = 5,
	COMMAND = 6,
	ARMORED_PERSONEL_CARRIER = 7
}