modded class PauseMenuUI
{
	protected const float FACTION_CHANGE_MIN_INTERVAL_MS = 120000.0;
	static float s_fLastFactionChange_ms = -1.0; // By default -1.0 - we never changed faction
	
	protected SCR_ButtonTextComponent m_ChangeFactionButton;
	
	override void OnMenuClose()
	{
		super.OnMenuClose();
	}
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		// We make the pause menu modal, because when we open it over our menus, we don't want
		// to be able to interace with menus behind it
		// Had to disable it, since it makes interaction with other menus (settings, etc) impossible
		//GetGame().GetWorkspace().AddModal(GetRootWidget(), GetRootWidget());
		
		// Change Faction
		m_ChangeFactionButton = SCR_ButtonTextComponent.GetButtonText("ChangeFaction", m_wRoot);
		if (m_ChangeFactionButton)
		{
			m_ChangeFactionButton.m_OnClicked.Insert(OnChangeFaction);
		}
	}
	
	protected void OnChangeFaction()
	{
		float currentTime = GetGame().GetWorld().GetWorldTime();
		
		s_fLastFactionChange_ms = currentTime;
		Close();
		PR_PlayerControllerDeploymentComponent deploymentComp = PR_PlayerControllerDeploymentComponent.GetLocalInstance();
		deploymentComp.AskResetFaction();
	}
	
	override void OnMenuUpdate(float tDelta)
	{
		super.OnMenuUpdate(tDelta);
		
		UpdateFactionChangeButton();
	}
	
	protected void UpdateFactionChangeButton()
	{
		Faction currentFaction = PR_FactionMemberManager.GetLocalPlayerFaction();
		float currentTime = GetGame().GetWorld().GetWorldTime();
		float timeSinceFactionChanged = currentTime - s_fLastFactionChange_ms;
		bool factionChangeAllowed = (s_fLastFactionChange_ms == -1.0) || 							// Never changed faction
									(timeSinceFactionChanged >  FACTION_CHANGE_MIN_INTERVAL_MS) ||	// Timeout hasn't been reached yet
									(currentFaction == null);										// Don't have a faction yet
		
		string factionChangeButtonText;
		if (!factionChangeAllowed)
		{
			float timeLeft = FACTION_CHANGE_MIN_INTERVAL_MS - timeSinceFactionChanged;
			factionChangeButtonText = string.Format("Change Faction (wait %1 s)", Math.Ceil(timeLeft / 1000.0));
		}
		else
		{
			factionChangeButtonText = "Change Faction";
		}
		
		m_ChangeFactionButton.SetText(factionChangeButtonText);
		m_ChangeFactionButton.SetEnabled(factionChangeAllowed);
	}
}