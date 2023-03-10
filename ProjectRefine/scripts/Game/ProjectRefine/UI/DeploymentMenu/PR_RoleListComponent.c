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
		
		SCR_AIGroup.GetOnPlayerAdded().Insert(OnPlayerAdded);
		SCR_AIGroup.GetOnPlayerRemoved().Insert(OnPlayerAdded);
		
		// Event subscription
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.GetLocalPlayerControllerGroupComponent();
		if(!groupController)
			return;
		
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
	
	void OnPlayerAdded(SCR_AIGroup group, int playerID)
	{		
		if(m_Group)
		{
			// Unsubscribe from old event
			PR_GroupRoleManagerComponent.Cast(m_Group.FindComponent(PR_GroupRoleManagerComponent)).m_OnAvailabilityChanged.Remove(OnAvailabilityChanged);
		}
		
		// Find new group
		m_Group = group;
		if (!m_Group)
		{
			// No group - destroy everything drawn
			DestroyContentOfList();
			return;	
		}
		
		if(!group.IsPlayerInGroup(playerID))
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
		for(int i = 0; i < groupRoleManagerComponent.m_aClaimableRolesCount.Count(); i++)
		{
			// Search if such a widget is already drawn
			Widget child = widgets.m_RoleListLayout.GetChildren();
			PR_RoleEntryComponent selection = null;
			Widget selectionWidget = null;
			while(child)
			{
				// Find its component
				PR_RoleEntryComponent comp = PR_RoleEntryComponent.Cast(child.FindHandler(PR_RoleEntryComponent));
				if(comp)
				{
					if (comp.m_iRoleIndex == i)
					{
						selection = comp;
						selectionWidget = child;
						break;
					}
				}
				
				child = child.GetSibling();
			}
			
			if(selection)
			{
				if(groupRoleManagerComponent.CanGroupMemberDrawRole(i, GetGame().GetPlayerController().GetPlayerId()))
				{				
					// Redraw availability count
				}
				else
				{
					selectionWidget.RemoveFromHierarchy();
				}
			}
			else
			{
				if(groupRoleManagerComponent.CanGroupMemberDrawRole(i, GetGame().GetPlayerController().GetPlayerId()))
				{				
					// Draw some debug role panel
					Widget wEntry = GetGame().GetWorkspace().CreateWidgets(PR_RoleEntryWidgets.s_sLayout, widgets.m_RoleListLayout);
					
					PR_RoleEntryComponent comp = PR_RoleEntryComponent.Cast(wEntry.FindHandler(PR_RoleEntryComponent));
					
					comp.Init(i, groupRoleManagerComponent.GetRole(i), groupRoleManagerComponent.GetRoleClaimableCount(i), groupRoleManagerComponent.GetRoleAvailableCount(i));
				}
			}
		}		
	}
	
	void DestroyContentOfList()
	{
		Widget child = widgets.m_RoleListLayout.GetChildren();
		while(child)
		{
			Widget next = child.GetSibling();
			child.RemoveFromHierarchy();
			child = next;
		}
	}
	
	// Squad leader has changed redraw!
	void OnPlayerLeaderChanged()
	{
		
	}
}