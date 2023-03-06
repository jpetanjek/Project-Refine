/*
Component for the panel with list of roles
*/

class PR_RoleListComponent : ScriptedWidgetComponent
{
	protected ref PR_RoleListWidgets widgets = new PR_RoleListWidgets();
	
	protected Widget m_wRoot;
	
	protected SCR_AIGroup m_Group;
	
	protected SCR_GroupsManagerComponent m_GroupManager;
	
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		
		m_GroupManager = SCR_GroupsManagerComponent.GetInstance();
		
		// Event subscription
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.GetLocalPlayerControllerGroupComponent();
		groupController.m_OnLocalPlayerJoinedGroup.Insert(OnLocalPlayerJoinedGroup);
		
		m_Group = m_GroupManager.FindGroup(groupController.GetGroupID());
		
		if(m_Group)
		{
			// Find component
			PR_GroupRoleManagerComponent groupRoleManagerComponent = PR_GroupRoleManagerComponent.Cast(m_Group.FindComponent(PR_GroupRoleManagerComponent));
			// Subscribe to its event
			groupRoleManagerComponent.m_OnAvailabilityChanged.Insert(OnAvailabilityChanged);
			
			// Draw current availability
			ReDrawCurrentAvailability(groupRoleManagerComponent);
		}
	}
	
	override void HandlerDeattached(Widget w)
	{
		bool what = 1;
	}
	
	void OnLocalPlayerJoinedGroup(int groupID)
	{
		if(m_Group)
		{
			// Unsubscribe from old event
			PR_GroupRoleManagerComponent.Cast(m_Group.FindComponent(PR_GroupRoleManagerComponent)).m_OnAvailabilityChanged.Remove(OnAvailabilityChanged);
		}
		
		if(groupID == -1)
		{
			// No group - destroy everything drawn?
		}
		
		// Find new group
		m_Group = m_GroupManager.FindGroup(groupID);
		if (!m_Group)
			return;	
		
		// Find component
		PR_GroupRoleManagerComponent groupRoleManagerComponent = PR_GroupRoleManagerComponent.Cast(m_Group.FindComponent(PR_GroupRoleManagerComponent));
		// Subscribe to its event
		groupRoleManagerComponent.m_OnAvailabilityChanged.Insert(OnAvailabilityChanged);
		
		// Draw current availability
		ReDrawCurrentAvailability(groupRoleManagerComponent);
	}
	
	void OnAvailabilityChanged(PR_GroupRoleManagerComponent groupRoleManagerComponent)
	{
		ReDrawCurrentAvailability(groupRoleManagerComponent)
	}
	
	void ReDrawCurrentAvailability(PR_GroupRoleManagerComponent groupRoleManagerComponent)
	{
	
	}
}