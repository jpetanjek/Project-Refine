class PR_PlayerListGameModeHeaderComponent : ScriptedWidgetComponent
{
	ref PR_PlayerListGameModeHeaderWidgets widgets = new PR_PlayerListGameModeHeaderWidgets();
	
	Widget m_wRoot;
	
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		widgets.Init(w);
	}
	
	void UpdateValues()
	{
		FactionManager fm = GetGame().GetFactionManager();
		PR_GameMode gm = PR_GameMode.Cast(GetGame().GetGameMode());
		SCR_Faction playerFaction = SCR_Faction.Cast(SCR_RespawnSystemComponent.GetLocalPlayerFaction());
		
		if (!gm || !playerFaction)
		{
			m_wRoot.SetVisible(false);
			return;
		}
		else
		{
			m_wRoot.SetVisible(true);
			
			// Faction flag
			ResourceName factionFlagTexture;
			factionFlagTexture = playerFaction.GetFactionFlag();
			widgets.m_FactionFlagImage.LoadImageTexture(0, factionFlagTexture);
			
			// Faction score
			int factionId = fm.GetFactionIndex(playerFaction);
			float factionScore = gm.GetFactionScore(factionId);
			widgets.m_FactionScoreText.SetText(Math.Ceil(factionScore).ToString());
		}
	}
}