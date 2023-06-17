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
	SUPPLY,
	ARMORED_TRANSPORT,
	TROOP_TRANSPORT,
	ARMED_TRANSPORT,
	MEDIC,
	FUEL,
	COMMAND,
	ARMORED_PERSONEL_CARRIER,
	INFANTRY_FIGHTING_VEHICLE,
	LIGHT_TANK,
	MEDIUM_TANK,
	HEAVY_TANK,
	LIGHT_TRANSPORT_HELICOPTER,
	HEAVY_TRANSPORT_HELICOPTER,
	ARMED_HELICOPTER,
	BOAT,
}