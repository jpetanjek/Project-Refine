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
			groupRoleManagerComponent.m_OnAvailabilityChanged.Insert(ReDrawCurrentAvailability);
			groupRoleManagerComponent.m_OnRoleClaimsChanged.Insert(ReDrawCurrentAvailability);
			
			// Draw current availability
			ReDrawCurrentAvailability(groupRoleManagerComponent);
		}
	}
	
	override void HandlerDeattached(Widget w)
	{
		bool what = 1;
	}
	
	//! Function meant to detect when current player joins any group
	void OnPlayerAdded(SCR_AIGroup group, int playerID)
	{
		if(playerID != GetGame().GetPlayerController().GetPlayerId())
			return;
				
		if(m_Group)
		{
			// Unsubscribe from old event
			PR_GroupRoleManagerComponent.Cast(m_Group.FindComponent(PR_GroupRoleManagerComponent)).m_OnAvailabilityChanged.Remove(ReDrawCurrentAvailability);
			PR_GroupRoleManagerComponent.Cast(m_Group.FindComponent(PR_GroupRoleManagerComponent)).m_OnRoleClaimsChanged.Remove(ReDrawCurrentAvailability);
		}
		
		// Find new group
		m_Group = group;
		if (!group.IsPlayerInGroup(GetGame().GetPlayerController().GetPlayerId()))
		{
			// No group - destroy everything drawn
			DestroyContentOfList();
			return;	
		}
		
		// Find component
		PR_GroupRoleManagerComponent groupRoleManagerComponent = PR_GroupRoleManagerComponent.Cast(m_Group.FindComponent(PR_GroupRoleManagerComponent));
		// Subscribe to its event
		groupRoleManagerComponent.m_OnAvailabilityChanged.Insert(ReDrawCurrentAvailability);
		groupRoleManagerComponent.m_OnRoleClaimsChanged.Insert(ReDrawCurrentAvailability);
		
		// Draw current availability
		ReDrawCurrentAvailability(groupRoleManagerComponent);
	}
	
	void ReDrawCurrentAvailability(PR_GroupRoleManagerComponent groupRoleManagerComponent)
	{	
		// Get our own loadout
		int myRoleId = groupRoleManagerComponent.GetPlayerRoleIndex(GetGame().GetPlayerController().GetPlayerId());
		
		for(int i = 0; i < groupRoleManagerComponent.m_aRoleAvailabilityCount.Count(); i++)
		{
			bool claimedByMe = i == myRoleId;
			
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
					selection.Redraw(groupRoleManagerComponent.GetRoleClaimableCount(i),
						groupRoleManagerComponent.GetRoleAvailableCount(i),
						claimedByMe);
					
					// Redraw claim button
					DrawClaimButton(groupRoleManagerComponent, selection, i);
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
					
					PR_Role role = groupRoleManagerComponent.GetRole(i);
					
					if(role.m_eRoleLimitation == PR_ERoleLimitation.SQUAD_LEAD_ONLY)
						wEntry.SetZOrder(1);
					else if(role.m_eRoleLimitation == PR_ERoleLimitation.FIRE_TEAM_LEAD_ONLY)
						wEntry.SetZOrder(2);
					else
						wEntry.SetZOrder(3);
					
					PR_RoleEntryComponent comp = PR_RoleEntryComponent.Cast(wEntry.FindHandler(PR_RoleEntryComponent));
					
					comp.Init(i, role);
					comp.Redraw(groupRoleManagerComponent.GetRoleClaimableCount(i),
						groupRoleManagerComponent.GetRoleAvailableCount(i),
						claimedByMe);
					DrawClaimButton(groupRoleManagerComponent, comp, i);
				}
			}
		}		
	}
	
	void DrawClaimButton(PR_GroupRoleManagerComponent groupRoleManagerComponent, PR_RoleEntryComponent selection, int index)
	{
		if(CanPlayerDrawClaimRoleButton(groupRoleManagerComponent, index, GetGame().GetPlayerController().GetPlayerId()))
		{
			// Enable button etc.
			selection.EnableClaimButton(true);
		}
		else
		{
			// Disable button etc.
			selection.EnableClaimButton(false);
		}
	}
	
	bool CanPlayerDrawClaimRoleButton(PR_GroupRoleManagerComponent groupRoleManagerComponent, int index, int playerID)
	{
		if(groupRoleManagerComponent.CanPlayerClaimRole(index, playerID))
		{
			// If he already has it claimed, he cannot claim again
			if(groupRoleManagerComponent.GetPlayerRoleIndex(playerID) == index)
			{
				return false;
			}
			else
			{
				// If he doesn't he can
				return true;
			}
		}
		else
		{
			return false;
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