[BaseContainerProps(configRoot:true), SCR_BaseContainerCustomTitleResourceName("m_sPrefab", true)]
class PR_SandboxTool
{
	[Attribute("1", desc: "By default disabled entries are ignored.", category: "Entity Spawn Info")]
	protected bool m_bEnabled;
	
	[Attribute(desc: "Sandbox tool prefab", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "et", category: "Entity Spawn Info")]
	ResourceName m_sPrefab;
	
	[Attribute("", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(PR_EPhase))]
	PR_EPhase m_ePhase;
	
	[Attribute("1", desc: "Claim price of tool")]
	int m_iClaimPrice;
	
	
	
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
	//void PR_SandboxTool()
	//{}
	
	//------------------------------------------------------------------------------------------------
	void PR_SandboxTool(ResourceName prefab, bool enabled = true)
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