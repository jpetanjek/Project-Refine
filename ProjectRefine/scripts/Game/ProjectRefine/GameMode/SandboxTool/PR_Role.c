enum PR_ERoleLimitation
{
	NONE,
	SQUAD_LEAD_ONLY,
	FIRE_TEAM_LEAD_ONLY,
}

[BaseContainerProps(configRoot: true)]
class PR_Role : PR_SandboxTool
{
	[Attribute(desc: "Name of Role", category: "Role Info")]
	protected string m_sRoleName;
	
	[Attribute("", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(PR_ERoleLimitation))]
	PR_ERoleLimitation m_eRoleLimitation;
	
	[Attribute(desc: "Default available count", category: "Role Info")]
	int m_iDefaultCount;
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns role name
	*/
	string GetRoleName()
	{
		return m_sRoleName;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns role limitation
	*/
	PR_ERoleLimitation GetRoleLimitation()
	{
		return m_eRoleLimitation;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns role default availability count
	*/
	PR_ERoleLimitation GetDefaultCount()
	{
		return m_iDefaultCount;
	}
	
	
}