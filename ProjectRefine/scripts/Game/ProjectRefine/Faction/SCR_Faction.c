modded class SCR_Faction
{
	[Attribute(desc: "List of PR assets related to this faction.")]
	protected ref PR_EntityAssetList m_aAssetList;
	
		//------------------------------------------------------------------------------------------------
	/*!
	Get the asset list assigned to this faction
	\return PR_EntityAssetList Vehicle List
	*/
	PR_EntityAssetList GetAssetList()
	{
		return m_aAssetList;
	}
}