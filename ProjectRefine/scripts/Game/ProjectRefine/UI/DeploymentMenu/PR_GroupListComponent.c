/*
Component for the panel with list of groups
*/

class PR_GroupListComponent : ScriptedWidgetComponent
{
	protected ref PR_GroupListWidgets widgets = new PR_GroupListWidgets();
	
	SCR_GroupsManagerComponent m_GroupsMgr;
	
	override void HandlerAttached(Widget w)
	{
		m_GroupsMgr = SCR_GroupsManagerComponent.GetInstance();
		
		widgets.Init(w);
		
		if (m_GroupsMgr)
			m_GroupsMgr.GetOnPlayableGroupCreated().Insert(Event_OnPlayableGroupCreated);
		
		widgets.m_CreateGroupButtonComponent.m_OnClicked.Insert(OnCreateGroupButton);
		
		GetGame().GetCallqueue().CallLater(OnFrame, 0, true);
		
		InitGroupList();
	}
	
	override void HandlerDeattached(Widget w)
	{
		GetGame().GetCallqueue().Remove(OnFrame);
		
		if (m_GroupsMgr)
			m_GroupsMgr.GetOnPlayableGroupCreated().Remove(Event_OnPlayableGroupCreated);
	}
	
	// Creates group entries for all existing groups
	void InitGroupList()
	{
		Faction myFaction = SCR_RespawnSystemComponent.GetLocalPlayerFaction();
		
		SCR_GroupsManagerComponent groupsMgr = SCR_GroupsManagerComponent.GetInstance();
		
		if (!groupsMgr)
			return;
		
		array<SCR_AIGroup> groups = groupsMgr.GetPlayableGroupsByFaction(myFaction);
		
		if (groups)
		{
			foreach (SCR_AIGroup group : groups)
				CreateGroupEntry(group);
		}
	}
	
	// Creates a group entry widget for specified group
	void CreateGroupEntry(notnull SCR_AIGroup group)
	{
		Widget wEntry = GetGame().GetWorkspace().CreateWidgets(PR_GroupEntryWidgets.s_sLayout, widgets.m_GroupListLayout);
		
		PR_GroupEntryComponent comp = PR_GroupEntryComponent.Cast(wEntry.FindHandler(PR_GroupEntryComponent));
		comp.Init(group);
	}
	
	// Called every frame
	void OnFrame()
	{
		// Find out which groups were deleted
		// There is an event for that in SCR_GroupsManagerComponent but it doesn't ever get called.
		// I think that it's more reliable if we delete entries ourselves anyway and don't rely on event.
		array<PR_GroupEntryComponent> groupEntriesToRemove = {};
		Widget wChild = widgets.m_GroupListLayout.GetChildren();
		while (wChild)
		{
			PR_GroupEntryComponent groupEntryComp = PR_GroupEntryComponent.Cast(wChild.FindHandler(PR_GroupEntryComponent));
			
			if (!groupEntryComp.GetGroup()) // Group does not exist any more?
				groupEntriesToRemove.Insert(groupEntryComp);
			
			wChild = wChild.GetSibling();
		}
		
		foreach (PR_GroupEntryComponent groupEntry : groupEntriesToRemove)
		{
			groupEntry.RemoveFromUi();
		}
		
		UpdateGroupsOrder();
	}
	
	//-----------------------------------------------------------------------------------------------
	void UpdateGroupsOrder()
	{
		// Our own group should be always on top
		SCR_PlayerControllerGroupComponent groupComp = SCR_PlayerControllerGroupComponent.GetLocalInstance();
		int myGroupId = groupComp.GetGroupID();
		
		Widget wChild = widgets.m_GroupListLayout.GetChildren();
		while (wChild)
		{
			PR_GroupEntryComponent groupEntryComp = PR_GroupEntryComponent.Cast(wChild.FindHandler(PR_GroupEntryComponent));
			
			SCR_AIGroup group = groupEntryComp.GetGroup();
			if (group)
			{
				int groupId = group.GetGroupID();
				if (groupId == myGroupId)
					wChild.SetZOrder(-1);
				else
					wChild.SetZOrder(0);
			}
			
			wChild = wChild.GetSibling();
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Our own UI events
	
	void OnCreateGroupButton()
	{
		string groupName = widgets.m_GroupNameEditBoxComponent.GetValue();
		
		// If group name was not provided, bail and show a notification
		if (groupName.IsEmpty())
		{
			ShowNotification(ENotification.PR_GROUP_MUST_HAVE_NAME);
			return;
		}
		
		bool privateGroup = widgets.m_LockButtonComponent.GetToggled();
		
		SCR_PlayerControllerGroupComponent groupComponent = SCR_PlayerControllerGroupComponent.GetLocalPlayerControllerGroupComponent();
		groupComponent.RequestCreateGroupEx(groupName, privateGroup);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Events
	
	void Event_OnPlayableGroupCreated(SCR_AIGroup group)
	{
		if (group)
		{
			//for (int i = 0; i < 20; i++)
			CreateGroupEntry(group);
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Other
	void ShowNotification(ENotification notificationId)
	{
		SCR_NotificationsComponent.SendLocal(notificationId);
	}
}