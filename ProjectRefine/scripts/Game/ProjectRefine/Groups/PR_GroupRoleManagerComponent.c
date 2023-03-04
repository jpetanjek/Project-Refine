[EntityEditorProps(category: "GameScripted/Groups", description: "Player groups role manager, attach to group!.")]
class PR_GroupRoleManagerComponentClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
class PR_GroupRoleManagerComponent : ScriptComponent
{
	// Network number of avialabe roles via state
	// An array the same size as all roles of a faction
	// -1 means never available for this group
	[RplProp (onRplName: "OnAvailabilityChangedClient")]
	array<int> m_aClaimableRolesCount;
	
	int m_iTotalAvailability = 0;
	
	// Initialize availability - via Game Mode
	void InitializeAvailability(array<int> roleAvailabilityCount)
	{
		m_aClaimableRolesCount = roleAvailabilityCount;
		RecalculateTotalAvailability();
	}
	
	void OnAvailabilityChangedClient()
	{
		RecalculateTotalAvailability();
		// Only show those roles that are avialable in UI to client
	}
	
	void RecalculateTotalAvailability()
	{
		m_iTotalAvailability = 0;
		for(int i = 0; i < m_aClaimableRolesCount.Count(); i++)
		{
			int val = m_aClaimableRolesCount[i];
			if(val != -1)
				m_iTotalAvailability += val;
		}
	}
	
	//------------------------------------------------------------------------------
	// UI logic START
	
	// Can I draw role increment at all?
	bool CanIncrementAvailabilityOfAnyRole()
	{
		// Check if requester is SL?
		
		if( m_iTotalAvailability == 0)
			return false;
		return true;
	}
	// UI logic END
	//------------------------------------------------------------------------------
	
	
	//------------------------------------------------------------------------------
	// Squad Lead Only logic START
	
	// -1 Dissalowed by SL
	// -2 Dissallowed by Commander
	
	bool ChangeAvailabilityOfRole(int index, bool increment)
	{
		if(increment)
		{
			if(CanIncrementAvailabilityOfRole(index))
			{
				int val = m_aClaimableRolesCount[index];
				val++;
				m_aClaimableRolesCount[index] = val;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if(CanDecrementAvailabilityOfRole(index))
			{
				int val = m_aClaimableRolesCount[index];
				val--;
				m_aClaimableRolesCount[index] = val;
				return true;
			}
			else
			{
				return false;
			}
		}		
	}
	
	// Can I draw role increment for this specific role?
	bool CanIncrementAvailabilityOfRole(int index)
	{
		if(!CanIncrementAvailabilityOfAnyRole() || !m_aClaimableRolesCount.IsIndexValid(index) || m_aClaimableRolesCount[index] == -1)
			return false;
		
		// check if its SL_ROLE or FIRETEAMLEAD- can't change those
		
		// check if SL has enough budget for claim price
			
		return true;
	}
	
	bool CanDecrementAvailabilityOfRole(int index)
	{
		if(!m_aClaimableRolesCount.IsIndexValid(index) || m_aClaimableRolesCount[index] == -1 || m_aClaimableRolesCount[index] == 0)
			return false;
		
		// check if its SL_ROLE or FIRETEAMLEAD- can't change those
		
		return true;
	}
	// Squad Lead Only logic END
	//------------------------------------------------------------------------------
	
	
	//------------------------------------------------------------------------------
	// Claim Logic START
	
	// Manage pick and availability logic (first come, first serve), from server to client
	// On Character level manage selected role (use this as indexOld) and active role (currently in use - draw this as icon)
	// Claiming can only happen in respawn menu / when there is no active role (no character possesed)
	// When player leaves group indexNew will be -1
	bool ClaimRole(int indexNew, int indexOld)
	{
		// Case 1: Claiming role
		if(m_aClaimableRolesCount.IsIndexValid(indexNew))
		{
			// Check availability
			if(m_aClaimableRolesCount[indexNew] != -1 || m_aClaimableRolesCount[indexNew] > 0)
			{
				// Available
				// Increment new
				int val = m_aClaimableRolesCount[indexNew] - 1;
				m_aClaimableRolesCount[indexNew] = val;
				
				// Decrement old
				if(m_aClaimableRolesCount.IsIndexValid(indexOld) && m_aClaimableRolesCount[indexOld] != -1)
				{
					val = m_aClaimableRolesCount[indexOld] + 1;
					m_aClaimableRolesCount[indexOld] = val;
				}
			}
			else
			{
				// Not available
				return false;
			}
		}
		
		// Case 2: Leaving group
		else if(indexNew == -1)
		{
			// Make sure old role is available for this group
			if(m_aClaimableRolesCount.IsIndexValid(indexOld) && m_aClaimableRolesCount[indexOld] != -1)
			{
				int val = m_aClaimableRolesCount[indexOld] + 1;
				m_aClaimableRolesCount[indexOld] = val;
			}
				
		}
		
		// non valid index
		return false;	
	}
	
	// Claim Logic END
	//------------------------------------------------------------------------------
	
}