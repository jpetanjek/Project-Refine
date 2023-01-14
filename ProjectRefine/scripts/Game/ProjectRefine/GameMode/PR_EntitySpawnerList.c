/*!
List of entity Assets
*/
[BaseContainerProps(configRoot: true)]
class PR_EntitySpawnerList
{
	[Attribute(desc: "Asset list")]
	protected ref array<ref PR_EntitySpawnInfo> m_aAssetList;
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get count of asset list
	\return in Asset list count
	*/
	int GetEntryCount()
	{
		return m_aAssetList.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get the entry of given index
	\param index of entity
	\param[out] Entity Info
	\return bool returns false if index is invalid
	*/
	PR_EntitySpawnInfo GetEntryAtIndex(int index)
	{
		if (!m_aAssetList || index < 0 || index >= m_aAssetList.Count())
		{
			return null;
		}
		return m_aAssetList[index];
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get Prefab of Index
	\param index of entity
	\return ResourceName Entity Prefab
	*/
	ResourceName GetResourceNameAtIndex(int index)
	{
		PR_EntitySpawnInfo entityInfo = GetEntryAtIndex(index);
		if (entityInfo)
		{
			return entityInfo.GetPrefab();
		}
		return string.Empty;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get List of Entity Info
	\param[out] Entity Info List
	\bool If true only get entities if they are set to enabled
	\return int Size of list
	*/
	int GetAssetList(out notnull array<PR_EntitySpawnInfo> assetList, bool checkIfEnabled = true)
	{
		foreach (PR_EntitySpawnInfo entityInfo : m_aAssetList)
		{
			if (!entityInfo || (checkIfEnabled && !entityInfo.GetEnabled()))
				continue;
			
			assetList.Insert(entityInfo);
		}
		
		return assetList.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	void PR_EntitySpawnerList(array<ref PR_EntitySpawnInfo> assetList = null)
	{
		if (SCR_Global.IsEditMode())
			return;
		
		if (assetList != null)
		{
			m_aAssetList = assetList;
		}
	}
};