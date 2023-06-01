class PR_DeploymentMenuComponentClass : PR_BaseGameModeComponentClass
{
}

class PR_DeploymentMenuComponent : PR_BaseGameModeComponent
{	
	protected MenuManager m_MenuMgr;
	protected SCR_RespawnSystemComponent m_RespawnSystem;
	protected PR_PC_PossessionManagerComponent m_PossessionManager;
	protected PlayerController m_pc;
	
	protected bool m_bInsertedForPossessionEvent = false;
	
	override void OnPostInit(IEntity owner)
	{
		// Must run only on those with UI
		if (!System.IsConsoleApp())
			SetEventMask(owner, EntityEvent.FRAME);
		
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	protected override void EOnInit(IEntity owner)
	{
		m_MenuMgr = GetGame().GetMenuManager();
		m_RespawnSystem = SCR_RespawnSystemComponent.GetInstance();	
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		m_pc = GetGame().GetPlayerController();
		if(!m_pc)
			return;
		
		if(!m_PossessionManager)
			m_PossessionManager = PR_PC_PossessionManagerComponent.GetLocalInstance();
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.REFINE_DISABLE_AUTO_DEPLOYMENT_MENU))
			return;
		
		// First frame only, further with events
		// We go through logic because we assume we can reconnect and have already assigned faction etc.
		MenuLogic();
	}
	
	void MenuLogic()
	{
		if (!m_RespawnSystem.GetLocalPlayerFaction())
		{
			// Edge case of no faction assigned, but has character?
			
			// We don't have faction
			if(!m_MenuMgr.FindMenuByPreset(ChimeraMenuPreset.RefineFactionSelectionMenu))
				m_MenuMgr.OpenMenu(ChimeraMenuPreset.RefineFactionSelectionMenu);
		}
		else
		{
			m_MenuMgr.CloseMenuByPreset(ChimeraMenuPreset.RefineFactionSelectionMenu);
			
			//if(m_pc.GetControlledEntity()) Print("HasControlled 1"); else Print("HasControlled 0");
			
			PR_EPossessionState possessionState = m_PossessionManager.GetState();
			
			if (possessionState != PR_EPossessionState.MAIN)
			{
				// We are not spawned
				if (!m_MenuMgr.FindMenuByPreset(ChimeraMenuPreset.RefineDeploymentMenu))
					m_MenuMgr.OpenMenu(ChimeraMenuPreset.RefineDeploymentMenu);
			}
			else
			{
				m_MenuMgr.CloseMenuByPreset(ChimeraMenuPreset.RefineDeploymentMenu);
				//if(m_MenuMgr.FindMenuByPreset(ChimeraMenuPreset.RefineDeploymentMenu))
				//	Print("Still open!");
			}
		}
	}
}