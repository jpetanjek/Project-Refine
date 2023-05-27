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

enum PR_EAssetBuildingFlags
{
	ORIENT_TO_SURFACE		= 1<<0,	// Orient to surface normal
	PLACE_ON_TERRAIN		= 1<<1,	// NYI
	PLACE_ON_ENTITIES		= 1<<2,	// NYI
	REQUIRES_FOB			= 1<<3,	// Can be placed only at FOB
	FORBID_FOB_2X_RANGE		= 1<<4	// Can't be placed closer than FOB double range
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
	
	[Attribute("0", UIWidgets.Flags, enums: ParamEnumArray.FromEnum(PR_EAssetBuildingFlags))]
	PR_EAssetBuildingFlags m_eFlags;
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