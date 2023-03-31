class PR_FactionSelectionButtonComponent : ScriptedWidgetComponent
{
	protected Widget m_wRoot;
	
	protected ref PR_FactionSelectionButtonWidgets widgets = new PR_FactionSelectionButtonWidgets();
	
	protected SCR_Faction m_Faction;
	
	void Init(notnull SCR_Faction faction)
	{
		m_Faction = faction;
		
		// Faction name
		widgets.m_FactionNameText.SetText(faction.GetFactionName());
		
		// Faction flag
		ResourceName flagRscName = faction.GetFactionFlag();
		widgets.m_FlagImage.LoadImageTexture(0, flagRscName);
		
		GetGame().GetCallqueue().CallLater(OnFrame, 0, true);
	}
	
	SCR_Faction GetFaction()
	{
		return m_Faction;
	}
	
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		widgets.Init(w);
	}
	
	override void HandlerDeattached(Widget w)
	{
		GetGame().GetCallqueue().Remove(OnFrame);
	}
	
	Widget GetRootWidget()
	{
		return m_wRoot;
	}
	
	protected void OnFrame()
	{
		// Update player count text
		int playerCount = SCR_RespawnSystemComponent.GetInstance().GetFactionPlayerCount(m_Faction);
		widgets.m_PlayerCountText.SetTextFormat("Players: %1", playerCount);
	}
}