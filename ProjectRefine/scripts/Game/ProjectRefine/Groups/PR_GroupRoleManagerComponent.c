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
	[RplProp (onRplName: "OnAvailabilityChangedClient")]
	array<int> m_aClaimableRolesCount;
	
	// Defines how many of this role there can be max
	[RplProp (onRplName: "OnAvailabilityChangedClient")]
	array<int> m_RoleAvailabilityCount;
	
	// Events
	ref ScriptInvokerBase<OnAvailabilityChangedDelegate> m_OnAvailabilityChanged = new ScriptInvokerBase<OnAvailabilityChangedDelegate>();
	
	// Initialize availability - via Game Mode
	void InitializeAvailability(array<int> roleAvailabilityCount)
	{
		m_aClaimableRolesCount = roleAvailabilityCount;
		m_RoleAvailabilityCount = roleAvailabilityCount;
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
		if(!m_RoleAvailabilityCount.IsIndexValid(indexNew) || m_RoleAvailabilityCount[indexNew] <= 0)
			return false;
		
		// Check if there is any of it left
		if(!m_aClaimableRolesCount.IsIndexValid(indexNew) || m_aClaimableRolesCount[indexNew] <= 0)
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
		if(!m_RoleAvailabilityCount.IsIndexValid(indexOld) || m_RoleAvailabilityCount[indexOld] <= 0)
			return false;
		
		// Check if it is below maximum
		if(!m_aClaimableRolesCount.IsIndexValid(indexOld) || m_aClaimableRolesCount[indexOld] >= m_RoleAvailabilityCount[indexOld])
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
		m_OnAvailabilityChanged.Invoke(this);
		
		// Get local group
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.GetLocalPlayerControllerGroupComponent();
		if (!groupController)
			return;
		
		// Get this groups id
		int thisGroupId = SCR_AIGroup.Cast(GetOwner()).GetGroupID();
		
		if(thisGroupId != groupController.GetGroupID())
			return;
		
	}
	
	
	//-----------------------------------------------
	// GROUP MEMBER UI LOGIC START
	
	// Skip this one for your own role
	bool CanGroupMemberDrawRole(int index)
	{
		// Check if it is at all available
		if(!m_RoleAvailabilityCount.IsIndexValid(index) || m_RoleAvailabilityCount[index] <= 0)
			return false;
		
		// Check if there is any of it left
		if(!m_aClaimableRolesCount.IsIndexValid(index) || m_aClaimableRolesCount[index] <= 0)
			return false;
		
		return true;
	}
	
	// GROUP MEMBER UI LOGIC END
	//-----------------------------------------------
	
	//-----------------------------------------------
	// GROUP LEADER UI LOGIC START
	
	bool CanGroupLeaderDrawRole(int index)
	{
		// Check if it is at all available
		if(!m_RoleAvailabilityCount.IsIndexValid(index) || m_RoleAvailabilityCount[index] < 0)
			return false;
		
		return true;
	}
	
	// GROUP LEADER UI LOGIC END
	//-----------------------------------------------
	
	
	// UI LOGIC END
	//-----------------------------------------------
}