class PR_MapInfoBarWidgetComponent : ScriptedWidgetComponent
{
	protected ref PR_MapInfoBarWidgets widgets = new PR_MapInfoBarWidgets();
	protected Widget m_wRoot;
	
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		widgets.Init(w);
		
		GetGame().GetCallqueue().CallLater(OnFrame, 0, true);
	}
	
	override void HandlerDeattached(Widget w)
	{
		GetGame().GetCallqueue().Remove(OnFrame);
	}
	
	// Called every frame
	void OnFrame()
	{
		FactionManager fm = GetGame().GetFactionManager();
		PR_GameMode gm = PR_GameMode.Cast(GetGame().GetGameMode());
		SCR_Faction playerFaction = SCR_Faction.Cast(SCR_RespawnSystemComponent.GetLocalPlayerFaction());
		
		if (!gm || !playerFaction)
		{
			m_wRoot.SetVisible(false);
			return;
		}
		
		m_wRoot.SetVisible(true);
		
		// Faction flag
		ResourceName factionFlagTexture;
		factionFlagTexture = playerFaction.GetFactionFlag();
		widgets.m_FactionFlagImage.LoadImageTexture(0, factionFlagTexture);
		
		// Faction score
		int factionId = fm.GetFactionIndex(playerFaction);
		int factionScore = gm.GetFactionScore(factionId);
		int factionScoreIncRate = gm.GetFactionScoreIncRate(factionId);
		
		string scoreText;
		if (factionScoreIncRate == 0)
			scoreText = factionScore.ToString();
		else
			scoreText = string.Format("%1 (%2/min.)", factionScore.ToString(), factionScoreIncRate.ToString());	
			
		widgets.m_FactionPointsText.SetText(scoreText);
	}
}