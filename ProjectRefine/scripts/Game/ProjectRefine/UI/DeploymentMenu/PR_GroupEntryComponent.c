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
	}
	
	override void HandlerDeattached(Widget w)
	{
		SCR_AIGroup.GetOnPlayerAdded().Remove(Event_OnPlayerAdded);
		SCR_AIGroup.GetOnPlayerRemoved().Remove(Event_OnPlayerRemoved);
	}
	
	void Init(SCR_AIGroup group)
	{
		SCR_AIGroup.GetOnPlayerAdded().Insert(Event_OnPlayerAdded);
		SCR_AIGroup.GetOnPlayerRemoved().Insert(Event_OnPlayerRemoved);
	}
	
	void CreateGroupMemberLine(int playerId)
	{
		Widget w = GetGame().GetWorkspace().CreateWidgets(PR_GroupMemberLineWidgets.s_sLayout, widgets.m_GroupMemberList);
		PR_GroupMemberLineComponent grpMemberComp = PR_GroupMemberLineComponent.Cast(w.FindHandler(PR_GroupMemberLineComponent));
		grpMemberComp.Init(playerId);
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
	
	//--------------------------------------------------------------------------------
	// Events
	
	void Event_OnPlayerAdded(SCR_AIGroup group, int playerId)
	{
		// Bail if not our group
		if (group != m_Group)
			return;
		
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