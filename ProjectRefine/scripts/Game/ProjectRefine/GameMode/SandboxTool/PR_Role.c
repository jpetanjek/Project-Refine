enum PR_ERoleLimitation
{
	NONE,
	SQUAD_LEAD_ONLY,
	FIRE_TEAM_LEAD_ONLY,
}

[BaseContainerProps(configRoot: true), PR_RoleCustomTitleAttribute()]
class PR_Role : PR_SandboxTool
{
	[Attribute(desc: "Name of Role", category: "Role Info")]
	protected string m_sRoleName;
	
	[Attribute("", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(PR_ERoleLimitation))]
	PR_ERoleLimitation m_eRoleLimitation;
	
	[Attribute(desc: "Default available count", category: "Role Info")]
	int m_iDefaultCount;
	
	[Attribute(desc: "Role primary weapon", category: "Role Info")]
	protected string m_sRolePrimary;
	
	[Attribute(desc: "Role secondary weapon", category: "Role Info")]
	protected string m_sRoleSecondary;
	
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
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns role primary
	*/
	string GetPrimaryWeapon()
	{
		return m_sRolePrimary;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns role secondary
	*/
	string GetSecondaryWeapon()
	{
		return m_sRoleSecondary;
	}
}

class PR_RoleCustomTitleAttribute : BaseContainerCustomTitle
{	
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		PR_EPhase phase;
		string roleName;
		ResourceName prefab;
		
		source.Get("m_ePhase", phase);
		source.Get("m_sRoleName", roleName);
		source.Get("m_sPrefab", prefab);
		
		title = string.Format("%1  -  %2  -  %3", typename.EnumToString(PR_EPhase, phase), roleName, prefab);
		
		return true;
	}
} 