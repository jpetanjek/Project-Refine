typedef func OnAvailabilityChangedDelegate;
void OnAvailabilityChangedDelegate(PR_GroupRoleManagerComponent groupRoleManagerComponent);

[EntityEditorProps(category: "GameScripted/Groups", description: "Player groups role manager, attach to group!.")]
class PR_GroupRoleManagerComponentClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
class PR_GroupRoleManagerComponent : ScriptComponent
{
	// Defines how many of this role can be claimed
	[RplProp(onRplName: "OnAvailabilityChangedClient")]
	ref array<int> m_aClaimableRolesCount = {};
	
	// Defines how many of this role there can be max
	[RplProp(onRplName: "OnAvailabilityChangedClient")]
	ref array<int> m_aRoleAvailabilityCount = {};
	
	//! The fact I have to do this is beyond dumb
	[RplProp(onRplName: "OnAvailabilityChangedClient")]
	int m_iFactionId = -1;
	
	//! Local storage of role list so we don't query for it all the time which is stupid
	ref array<PR_Role> m_aRoleList = {};
	
	// Events
	ref ScriptInvokerBase<OnAvailabilityChangedDelegate> m_OnAvailabilityChanged = new ScriptInvokerBase<OnAvailabilityChangedDelegate>();
	
	// Initialize availability - via Game Mode
	void InitializeAvailability(array<int> roleAvailabilityCount)
	{
		m_aClaimableRolesCount = roleAvailabilityCount;
		m_aRoleAvailabilityCount = roleAvailabilityCount;
		
		// Dumb code start
		SCR_Faction groupFaction = SCR_Faction.Cast(SCR_AIGroup.Cast(GetOwner()).GetFaction());
		FactionManager factionManager = GetGame().GetFactionManager();		
		m_iFactionId = factionManager.GetFactionIndex(groupFaction);
		// Dumb code end
		
		PR_RoleList fullRoleList = groupFaction.GetRoleList();
		fullRoleList.GetRoleList(m_aRoleList);
		
		Replication.BumpMe();
	}
	
	int GetGroupID()
	{
		return SCR_AIGroup.Cast(GetOwner()).GetGroupID();
	}
	
	// 0 means not available to this group as defined by SL
	// -1 means not available to this group as defined by Commander
	
	//-----------------------------------------------
	// CLAIM LOGIC START
	
	bool ClaimRole(int indexNew)
	{
		// Check if it is at all available
		if(!m_aRoleAvailabilityCount || !m_aRoleAvailabilityCount.IsIndexValid(indexNew) || m_aRoleAvailabilityCount[indexNew] <= 0)
			return false;
		
		// Check if there is any of it left
		if(!m_aClaimableRolesCount || !m_aClaimableRolesCount.IsIndexValid(indexNew) || m_aClaimableRolesCount[indexNew] <= 0)
			return false;
		
		// Check if role is limited
		
		// It is available, and there is enough of it
		int val = m_aClaimableRolesCount[indexNew];
		val--;
		m_aClaimableRolesCount[indexNew] = val;
		return true;
	}
	
	bool ReturnRole(int indexOld)
	{
		// Check if it is at all available
		if(!m_aClaimableRolesCount || !m_aRoleAvailabilityCount.IsIndexValid(indexOld) || m_aRoleAvailabilityCount[indexOld] <= 0)
			return false;
		
		// Check if it is below maximum
		if(!m_aClaimableRolesCount || !m_aClaimableRolesCount.IsIndexValid(indexOld) || m_aClaimableRolesCount[indexOld] >= m_aRoleAvailabilityCount[indexOld])
			return false;
		
		// It is can be returned
		int val = m_aClaimableRolesCount[indexOld];
		val++;
		m_aClaimableRolesCount[indexOld] = val;
		return true;
	}
	
	// CLAIM LOGIC END
	//-----------------------------------------------
	
	
	//-----------------------------------------------
	// UI LOGIC START
	
	void OnAvailabilityChangedClient()
	{		
		if(m_aRoleList.IsEmpty())
		{
			// Dumb code start
			FactionManager factionManager = GetGame().GetFactionManager();		
			SCR_Faction groupFaction = SCR_Faction.Cast(factionManager.GetFactionByIndex(m_iFactionId));
			// Dumb code end
			
			if(!groupFaction)
				return;
			
			PR_RoleList fullRoleList = groupFaction.GetRoleList();
			fullRoleList.GetRoleList(m_aRoleList);
		}
		
		// Useless logic but has to be here since states are a bit broken
		// Will be called twice by availability and claimability
		if(!m_aRoleList.IsEmpty())
		{
			m_OnAvailabilityChanged.Invoke(this);
		}
	}
	
	int GetRoleClaimableCount(int index)
	{
		if(m_aClaimableRolesCount && m_aClaimableRolesCount.IsIndexValid(index))
		{
			return m_aClaimableRolesCount[index];
		}
		else
		{
			return -1;
		}
	}
	
	int GetRoleAvailableCount(int index)
	{
		if(m_aRoleAvailabilityCount && m_aRoleAvailabilityCount.IsIndexValid(index))
		{
			return m_aRoleAvailabilityCount[index];
		}
		else
		{
			return -1;
		}
	}
		
	PR_Role GetRole(int index)
	{
		if(!m_aRoleList.IsIndexValid(index))
			return null;
		
		return m_aRoleList[index];
	}
	
	//-----------------------------------------------
	// GROUP MEMBER UI LOGIC START
	
	// Skip this one for your own role
	bool CanGroupMemberDrawRole(int index, int playerID)
	{
		// Check if it is at all available
		if(!m_aRoleAvailabilityCount || !m_aRoleAvailabilityCount.IsIndexValid(index) || m_aRoleAvailabilityCount[index] <= 0)
			return false;
		
		// Check if there is any of it left
		if(!m_aClaimableRolesCount || !m_aClaimableRolesCount.IsIndexValid(index) || m_aClaimableRolesCount[index] <= 0)
			return false;
		
		// Check limitations
		// Get Role
		PR_Role role = GetRole(index);
		if(!role)
			return false;
		
		// Check if playerID is leader		
		if(role.m_eRoleLimitation == PR_ERoleLimitation.SQUAD_LEAD_ONLY)
		{
			SCR_AIGroup group = SCR_AIGroup.Cast(GetOwner());
			if(!group)
				return false;
			
			if(!group.IsPlayerLeader(playerID))
				return false;
		} 
		
		return true;
	}
	
	// GROUP MEMBER UI LOGIC END
	//-----------------------------------------------
	
	//-----------------------------------------------
	// GROUP LEADER UI LOGIC START
	
	bool CanGroupLeaderDrawRole(int index)
	{
		// Check if it is at all available
		if(!m_aRoleAvailabilityCount || !m_aRoleAvailabilityCount.IsIndexValid(index) || m_aRoleAvailabilityCount[index] < 0)
			return false;
		
		return true;
	}
	
	// GROUP LEADER UI LOGIC END
	//-----------------------------------------------
	
	
	// UI LOGIC END
	//-----------------------------------------------
}