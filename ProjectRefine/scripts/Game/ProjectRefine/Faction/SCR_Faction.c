modded class SCR_Faction
{
	[Attribute(desc: "List of PR assets related to this faction.")]
	protected ref PR_AssetList m_aAssetList;
	
	[Attribute(desc: "List of PR roles related to this faction.")]
	protected ref PR_RoleList m_aRoleList;
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get the asset list assigned to this faction
	\return PR_AssetList
	*/
	PR_AssetList GetAssetList()
	{
		return m_aAssetList;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get the role list assigned to this faction
	\return PR_RoleList
	*/
	PR_RoleList GetRoleList()
	{
		return m_aRoleList;
	}
}