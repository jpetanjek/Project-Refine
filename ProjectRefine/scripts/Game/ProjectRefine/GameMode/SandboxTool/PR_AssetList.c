/*!
List of Assets
*/
[BaseContainerProps(configRoot: true)]
class PR_AssetList
{
	[Attribute(desc: "Asset list")]
	protected ref array<ref PR_Asset> m_aAssetList;
	
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
	\param[out] Asset
	\return bool returns false if index is invalid
	*/
	PR_Asset GetEntryAtIndex(int index)
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
		PR_Asset entityInfo = GetEntryAtIndex(index);
		if (entityInfo)
		{
			return entityInfo.GetPrefab();
		}
		return string.Empty;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get List of Entity Info
	\param[out] Asset List
	\bool If true only get entities if they are set to enabled
	\return int Size of list
	*/
	int GetAssetList(out notnull array<PR_Asset> assetList, bool checkIfEnabled = true)
	{
		foreach (PR_Asset entityInfo : m_aAssetList)
		{
			if (!entityInfo || (checkIfEnabled && !entityInfo.GetEnabled()))
				continue;
			
			assetList.Insert(entityInfo);
		}
		
		return assetList.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	void PR_AssetList(array<ref PR_Asset> assetList = null)
	{
		if (SCR_Global.IsEditMode())
			return;
		
		if (assetList != null)
		{
			m_aAssetList = assetList;
		}
	}
};