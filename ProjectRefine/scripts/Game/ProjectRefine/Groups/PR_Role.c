[EntityEditorProps(category: "GameScripted/GameMode")]
class PR_RoleClass: GenericEntityClass
{
};

enum PR_ERoleLimitation
{
	NONE,
	SQUAD_LEAD_ONLY,
	FIRE_TEAM_LEAD_ONLY,
}

class PR_Role : GenericEntity
{
	[Attribute(desc: "Name of Role", category: "Role Info")]
	protected string m_sRoleName;
	
	[Attribute(desc: "Role prefab", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "et", category: "Role Info")]
	ResourceName m_sPrefab;
	
	[Attribute("", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(PR_ERoleLimitation))]
	PR_ERoleLimitation m_eRoleLimitation;
	
	[Attribute("", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(PR_EPhase))]
	PR_EPhase m_ePhase;
}