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
	array<int> m_aRoleAvailabilityCount;
	
	int m_iTotalAvailability = 0;
	
	// Initialize availability - via Game Mode
	void InitializeAvailability(array<int> roleAvailabilityCount)
	{
		m_aRoleAvailabilityCount = roleAvailabilityCount;
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
		for(int i = 0; i < m_aRoleAvailabilityCount.Count(); i++)
		{
			int val = m_aRoleAvailabilityCount[i];
			if(val != -1)
				m_iTotalAvailability += val;
		}
	}
	
	bool ChangeAvailabilityOfRole(int index, bool increment)
	{
		if(increment)
		{
			if(CanIncrementAvailabilityOfRole(index))
			{
				int val = m_aRoleAvailabilityCount[index];
				val++;
				m_aRoleAvailabilityCount[index] = val;
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
				int val = m_aRoleAvailabilityCount[index];
				val--;
				m_aRoleAvailabilityCount[index] = val;
				return true;
			}
			else
			{
				return false;
			}
		}		
	}
	
	// Can I draw role increment at all?
	bool CanIncrementAvailabilityOfAnyRole()
	{
		if( m_iTotalAvailability == 0)
			return false;
		return true;
	}
	
	// Can I draw role increment for this specific role?
	bool CanIncrementAvailabilityOfRole(int index)
	{
		if(!CanIncrementAvailabilityOfAnyRole() || !m_aRoleAvailabilityCount.IsIndexValid(index) || m_aRoleAvailabilityCount[index] == -1)
			return false;
		
		// check if its SL_ROLE or FIRETEAMLEAD- can't change those
		
		// check if SL has enough budget for claim price
			
		return true;
	}
	
	bool CanDecrementAvailabilityOfRole(int index)
	{
		if(!m_aRoleAvailabilityCount.IsIndexValid(index) || m_aRoleAvailabilityCount[index] == -1 || m_aRoleAvailabilityCount[index] == 0)
			return false;
		
		// check if its SL_ROLE or FIRETEAMLEAD- can't change those
		
		return true;
	}
	
	// Manage pick and availability logic (first come, first serve), from server to client
	// On Character/PlayerController level manage selected role (use this as indexOld) and active role (currently in use - draw this as icon)
	// Claiming can only happen in respawn menu
	bool ClaimRole(int indexNew, int indexOld)
	{
		return true;	
	}
}