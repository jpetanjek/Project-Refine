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
		
		widgets.Init(m_wRoot);
		
		m_GroupManager = SCR_GroupsManagerComponent.GetInstance();
		
		// Event subscription
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.GetLocalPlayerControllerGroupComponent();
		if(!groupController)
			return;
		
		groupController.m_OnLocalPlayerChangedGroup.Insert(OnLocalPlayerChangedGroup);
		
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
	
	void OnLocalPlayerChangedGroup(int groupID)
	{
		if(m_Group)
		{
			// Unsubscribe from old event
			PR_GroupRoleManagerComponent.Cast(m_Group.FindComponent(PR_GroupRoleManagerComponent)).m_OnAvailabilityChanged.Remove(OnAvailabilityChanged);
		}
		
		// Find new group
		m_Group = m_GroupManager.FindGroup(groupID);
		if (!m_Group)
		{
			// No group - destroy everything drawn?
			return;	
		}
		
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
		// Draw some debug role panel
		Widget wEntry = GetGame().GetWorkspace().CreateWidgets(PR_RoleEntryWidgets.s_sLayout, widgets.m_RoleListLayout);
		
		PR_RoleEntryComponent comp = PR_RoleEntryComponent.Cast(wEntry.FindHandler(PR_RoleEntryComponent));
		
		comp.Init();
	}
}