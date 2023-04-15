/*
Component for one group entry with list of players
*/

class PR_GroupEntryComponent : ScriptedWidgetComponent
{
	protected ref PR_GroupEntryWidgets widgets = new PR_GroupEntryWidgets();
	
	protected Widget m_wRoot;
	
	protected SCR_AIGroup m_Group;
	
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		widgets.Init(w);
		
		widgets.m_ExpandButtonComponent.m_OnToggled.Insert(OnExpandButton);
		widgets.m_JoinLeaveButtonComponent.m_OnClicked.Insert(OnJoinLeaveButton);
		widgets.m_ActionButtonComponent.m_OnClicked.Insert(OnActionButton);
		
		ExpandMemberList(GetExpanded());
		
		UpdateJoinLeaveButton();
		
		GetGame().GetCallqueue().CallLater(OnFrame, 0, true);
	}
	
	override void HandlerDeattached(Widget w)
	{
		SCR_AIGroup.GetOnPlayerAdded().Remove(Event_OnPlayerAdded);
		SCR_AIGroup.GetOnPlayerRemoved().Remove(Event_OnPlayerRemoved);
		SCR_AIGroup.GetOnCustomNameChanged().Remove(Event_OnCustomNameChanged);
		
		GetGame().GetCallqueue().Remove(OnFrame);
	}
	
	void Init(SCR_AIGroup group)
	{
		m_Group = group;
		
		SCR_AIGroup.GetOnPlayerAdded().Insert(Event_OnPlayerAdded);
		SCR_AIGroup.GetOnPlayerRemoved().Insert(Event_OnPlayerRemoved);
		SCR_AIGroup.GetOnCustomNameChanged().Insert(Event_OnCustomNameChanged);
		
		UpdateJoinLeaveButton();
		UpdateGroupNameText();
		UpdateMemberCountText();
		UpdateLockImage();
		UpdateActionButton();
		InitGroupMemberList();
	}
	
	void OnFrame()
	{
		if (!m_Group)
			return;
		
		UpdateGroupNameText(); // The event doesn't work for some reason, no time to investigate
		UpdateMemberCountText();
		UpdateLockImage();
		UpdateActionButton();
		UpdateJoinLeaveButtonEnabled();
		UpdateJoinLeaveButton();
		UpdateMembersOrder();
	}
	
	// Creates a group member line for every group member
	void InitGroupMemberList()
	{
		array<int> groupMembers = m_Group.GetPlayerIDs();
		foreach (int playerId : groupMembers)
		{
			CreateGroupMemberLine(playerId);
		}
	}
	
	void CreateGroupMemberLine(int playerId)
	{
		Widget w = GetGame().GetWorkspace().CreateWidgets(PR_GroupMemberLineWidgets.s_sLayout, widgets.m_GroupMemberList);
		PR_GroupMemberLineComponent grpMemberComp = PR_GroupMemberLineComponent.Cast(w.FindHandler(PR_GroupMemberLineComponent));
		grpMemberComp.Init(m_Group, playerId);
	}
	
	PR_GroupMemberLineComponent FindGroupMemberLine(int playerId)
	{
		Widget wChild = widgets.m_GroupMemberList.GetChildren();
		while (wChild)
		{
			PR_GroupMemberLineComponent comp = PR_GroupMemberLineComponent.Cast(wChild.FindHandler(PR_GroupMemberLineComponent));
			if (comp.GetPlayerId() == playerId)
				return comp;
			wChild = wChild.GetSibling();
		}
		
		return null;
	}
	
	// Removes the group entry from the UI
	// It's done this way in case we want to customize the removal, via an animation or similar
	void RemoveFromUi()
	{
		m_wRoot.RemoveFromHierarchy();
	}
	
	// Updates state of join button depending on our state
	void UpdateJoinLeaveButton()
	{
		if (!m_Group)
		{
			widgets.m_JoinLeaveButtonComponent.SetEffectsWithAnyTagEnabled({"all", "not_joined"});
			return;
		}
		
		string buttonMode;
		
		if (m_Group.IsPlayerInGroup(GetGame().GetPlayerController().GetPlayerId())) // Are we in that group?
			buttonMode = "joined";
		else
			buttonMode = "not_joined";
		
		widgets.m_JoinLeaveButtonComponent.SetEffectsWithAnyTagEnabled({"all", buttonMode});
	}
	
	void UpdateJoinLeaveButtonEnabled()
	{
		bool enabled = true;
		
		if (m_Group.IsPrivate())
		{
			bool inGroup = m_Group.IsPlayerInGroup(GetGame().GetPlayerController().GetPlayerId());
			if (!inGroup)
				enabled = false;
		}
		else if (m_Group.IsFull())
			enabled = false;
		
		widgets.m_JoinLeaveButtonComponent.SetEnabled(enabled);
	}
	
	void UpdateGroupNameText()
	{
		PlayerManager pm = GetGame().GetPlayerManager();
		string leaderName = pm.GetPlayerName(m_Group.GetLeaderID());
		widgets.m_GroupNameText.SetText(m_Group.GetCustomName());
		widgets.m_LeaderNameText.SetText(leaderName);
	}
	
	void UpdateMemberCountText()
	{
		widgets.m_PlayerCountText.SetText(string.Format("%1/%2", m_Group.GetPlayerCount(), m_Group.GetMaxMembers()));
	}
	
	void UpdateLockImage()
	{
		widgets.m_LockImage.SetVisible(m_Group.IsPrivate());
	}
	
	void UpdateActionButton()
	{
		bool isLeader = m_Group.IsPlayerLeader(GetGame().GetPlayerController().GetPlayerId());
		widgets.m_ActionButton.SetVisible(isLeader);
	}
	
	void UpdateMembersOrder()
	{
		// Update Z order of entries. Leader should be on top of the list.
		int leaderId = m_Group.GetLeaderID();
		Widget wChild = widgets.m_GroupMemberList.GetChildren();
		while (wChild)
		{
			PR_GroupMemberLineComponent comp = PR_GroupMemberLineComponent.Cast(wChild.FindHandler(PR_GroupMemberLineComponent));
			if (comp.GetPlayerId() == leaderId)
				wChild.SetZOrder(-1); // It will be at the top
			else
				wChild.SetZOrder(0);
			
			wChild = wChild.GetSibling();
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Collapsing/expanding
	
	bool GetExpanded()
	{
		return widgets.m_ExpandButtonComponent.GetToggled();
	}
	
	protected void ExpandMemberList(bool expand)
	{
		widgets.m_GroupMemberList.SetVisible(expand);
	}
	
	//--------------------------------------------------------------------------------
	// Our UI events
	
	void OnJoinLeaveButton()
	{
		if (!m_Group)
			return;
		
		SCR_PlayerControllerGroupComponent groupComp = SCR_PlayerControllerGroupComponent.GetLocalPlayerControllerGroupComponent();
		
		// Are we joining or leaving?
		if (m_Group.IsPlayerInGroup(GetGame().GetPlayerController().GetPlayerId()))
		{
			// Leave
			groupComp.RequestLeaveGroup();
		}
		else
		{
			// Join
			groupComp.RequestJoinGroup(m_Group.GetGroupID());
		}
	}
	
	protected void OnExpandButton()
	{
		ExpandMemberList(GetExpanded());
	}
	
	protected void OnActionButton()
	{
		Widget wTooltip = SCR_TooltipManagerEntity.CreateTooltipEx("{9EEDAB4F4D223D45}UI/DeploymentMenu/GroupTooltip.layout",
			widgets.m_ActionButton,
			followCursor: false,
			checkWidgetUnderCursor: false,
			offset: "-5 10 0");
		
		PR_GroupTooltipComponent groupTooltipComp = PR_GroupTooltipComponent.Cast(wTooltip.FindHandler(PR_GroupTooltipComponent));
		groupTooltipComp.Init(m_Group);
	}
	
	//--------------------------------------------------------------------------------
	// External events
	
	void Event_OnPlayerAdded(SCR_AIGroup group, int playerId)
	{
		// Bail if not our group
		if (group != m_Group)
			return;
		
		//for (int i = 0; i < 5; i++)
		CreateGroupMemberLine(playerId);
	}
	
	void Event_OnPlayerRemoved(SCR_AIGroup group, int playerId)
	{
		// Bail if not our group
		if (group != m_Group)
			return;
		
		PR_GroupMemberLineComponent comp = FindGroupMemberLine(playerId);
		if (!comp)
			return; // wtf?
		
		comp.RemoveFromUi();
	}
	
	void Event_OnCustomNameChanged()
	{
		// Surprise surprise! We don't get new group name here, nor the group pointer
		// Just update our name anyway
		
		if (m_Group)
			UpdateGroupNameText();
	}
	
	//--------------------------------------------------------------------------------
	// Other minor things
	
	Widget GetRootWidget()
	{
		return m_wRoot;
	}
	
	SCR_AIGroup GetGroup()
	{
		return m_Group;
	}
}