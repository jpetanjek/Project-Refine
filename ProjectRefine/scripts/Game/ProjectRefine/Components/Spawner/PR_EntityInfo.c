/*
Class to be used for config, used to contain info about what to spawn and under which conditions
*/

[BaseContainerProps(configRoot:true), SCR_BaseContainerCustomTitleResourceName("m_sPrefab", true)]
class PR_EntityInfo
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
	void PR_EntityInfo()
	{}
	
	//------------------------------------------------------------------------------------------------
	void PR_EntityInfo(ResourceName prefab, bool enabled = true)
	{
		m_sPrefab = prefab;
		m_bEnabled = enabled;
	}

}

enum PR_EPhase
{
	RECON,
	LIGHT,
	MEDIUM,
	HEAVY
}

enum PR_EAssetType
{
	SOLDIER,
	TRANSPORT,
	ARMORED_TRANSPORT,
	TROOP_TRANSPORT,
	ARMED_TRANSPORT,
	FUEL,
	SUPPLY,
	COMMAND,
	ARMORED_PERSONEL_CARRIER
}