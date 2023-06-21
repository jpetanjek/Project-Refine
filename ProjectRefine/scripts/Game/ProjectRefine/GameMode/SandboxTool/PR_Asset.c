[BaseContainerProps(configRoot: true), PR_AssetAssetCustomTitleAttribute()]
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

class PR_AssetAssetCustomTitleAttribute : BaseContainerCustomTitle
{	
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		PR_EPhase phase;
		PR_EAssetType assetType;
		ResourceName prefab;
		
		source.Get("m_ePhase", phase);
		source.Get("m_eAssetType", assetType);
		source.Get("m_sPrefab", prefab);
		
		title = string.Format("%1  -  %2  -  %3", typename.EnumToString(PR_EPhase, phase), typename.EnumToString(PR_EAssetType, assetType), prefab);
		
		return true;
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
	MEDICAL,
	FUEL,
	COMMAND,
	ARMORED_PERSONNEL_CARRIER,
	INFANTRY_FIGHTING_VEHICLE,
	LIGHT_TANK,
	MEDIUM_TANK,
	HEAVY_TANK,
	LIGHT_TRANSPORT_HELICOPTER,
	HEAVY_TRANSPORT_HELICOPTER,
	ARMED_HELICOPTER,
	BOAT,
}