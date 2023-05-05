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

[BaseContainerProps(), PR_BuildingEntryAssetCustomTitleAttribute()]
class PR_BuildingEntryAsset : PR_BuildingEntry
{
	[Attribute("", UIWidgets.ResourceNamePicker, "Prefab of asset", "et")]
	ResourceName m_sPrefab;
	
	[Attribute("0", UIWidgets.EditBox, "Cost of asset")]
	float m_fCost;
	
	[Attribute("1", UIWidgets.CheckBox, "Orient asset to surface normal")]
	bool m_bOrientToSurface;
}

class PR_BuildingEntryAssetCustomTitleAttribute : BaseContainerCustomTitle
{	
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		string displayName;
		float cost;
		source.Get("m_sDisplayName", displayName);
		source.Get("m_fCost", cost);
		
		title = string.Format("%1 - %2", cost.ToString(), displayName);
		
		return true;
	}
}