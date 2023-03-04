/*!
List of Roles
*/
[BaseContainerProps(configRoot: true)]
class PR_RoleList
{
	[Attribute(desc: "Role list")]
	protected ref array<ref PR_Role> m_aRoleList;
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get count of asset list
	\return in Asset list count
	*/
	int GetEntryCount()
	{
		return m_aRoleList.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get the entry of given index
	\param index of entity
	\param[out] Role
	\return bool returns false if index is invalid
	*/
	PR_Role GetEntryAtIndex(int index)
	{
		if (!m_aRoleList || index < 0 || index >= m_aRoleList.Count())
		{
			return null;
		}
		return m_aRoleList[index];
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get Prefab of Index
	\param index of entity
	\return ResourceName Entity Prefab
	*/
	ResourceName GetResourceNameAtIndex(int index)
	{
		PR_Role entityInfo = GetEntryAtIndex(index);
		if (entityInfo)
		{
			return entityInfo.GetPrefab();
		}
		return string.Empty;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get List of Entity Info
	\param[out] Role List
	\bool If true only get entities if they are set to enabled
	\return int Size of list
	*/
	int GetRoleList(out notnull array<PR_Role> assetList, bool checkIfEnabled = true)
	{
		foreach (PR_Role entityInfo : m_aRoleList)
		{
			if (!entityInfo || (checkIfEnabled && !entityInfo.GetEnabled()))
				continue;
			
			assetList.Insert(entityInfo);
		}
		
		return assetList.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	void PR_RoleList(array<ref PR_Role> assetList = null)
	{
		if (SCR_Global.IsEditMode())
			return;
		
		if (assetList != null)
		{
			m_aRoleList = assetList;
		}
	}
};