// Classes for building asset list configurations

[BaseContainerProps(), BaseContainerCustomTitleField("m_sDisplayName")]
class PR_BuildingEntry
{
	[Attribute(desc: "Name to be shown in UI")]
	string m_sDisplayName;
	
	[Attribute(desc: "Description to be shown in UI")]
	string m_sDescription;
}

[BaseContainerProps(configRoot: true), BaseContainerCustomTitleField("m_sDisplayName")]
class PR_BuildingEntryCategory : PR_BuildingEntry
{
	[Attribute()]
	ref array<ref PR_BuildingEntry> m_aEntries;
}

enum PR_EAssetBuildingRules
{
	GENERIC,
	BASE_ONLY
}

[BaseContainerProps(), PR_BuildingEntryAssetCustomTitleAttribute()]
class PR_BuildingEntryAsset : PR_BuildingEntry
{
	[Attribute("", UIWidgets.ResourceNamePicker, "Prefab of asset, used only for preview.", "et")]
	ResourceName m_sPrefab;
	
	[Attribute("", UIWidgets.ResourceNamePicker, "Prefab of building manager for this asset, used for actual building.", "et")]
	ResourceName m_sBuildingManagerPrefab;
	
	[Attribute("0", UIWidgets.EditBox, "Cost of asset")]
	int m_iCost;
	
	[Attribute("1", UIWidgets.CheckBox, "Orient asset to surface normal")]
	bool m_bOrientToSurface;
	
	[Attribute("0", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(PR_EAssetBuildingRules))]
	PR_EAssetBuildingRules m_eAssetBuildingRules;
}

class PR_BuildingEntryAssetCustomTitleAttribute : BaseContainerCustomTitle
{	
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		string displayName;
		int cost;
		source.Get("m_sDisplayName", displayName);
		source.Get("m_iCost", cost);
		
		title = string.Format("%1 - %2", cost.ToString(), displayName);
		
		return true;
	}
} 