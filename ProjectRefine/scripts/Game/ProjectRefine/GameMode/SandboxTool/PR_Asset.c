[BaseContainerProps(configRoot: true)]
class PR_Asset : PR_SandboxTool
{
	[Attribute("", UIWidgets.ComboBox, "", enums: ParamEnumArray.FromEnum(PR_EAssetType))]
	PR_EAssetType m_eAssetType;
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns asset type
	*/
	PR_EAssetType GetAssetType()
	{
		return m_eAssetType;
	}
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