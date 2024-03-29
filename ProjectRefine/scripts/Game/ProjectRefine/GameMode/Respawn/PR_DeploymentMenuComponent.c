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
		PR_EPossessionState possessionState = m_PossessionManager.GetState();
		
		//SCR_EditorManagerCore core = SCR_EditorManagerCore.Cast(SCR_EditorManagerCore.GetInstance(SCR_EditorManagerCore));
		//SCR_EditorManagerEntity editorManager = core.GetEditorManager(m_pc.GetPlayerId());
		
		// If editor is opened, it overrides our menus
		if (possessionState == PR_EPossessionState.EDITOR) //editorManager && (editorManager.IsOpened() || editorManager.IsInTransition()))
		{
			m_MenuMgr.CloseMenuByPreset(ChimeraMenuPreset.RefineDeploymentMenu);
			m_MenuMgr.CloseMenuByPreset(ChimeraMenuPreset.RefineFactionSelectionMenu);
		}
		else
		{
			if (possessionState == PR_EPossessionState.NO_FACTION)
			{
				// We don't have faction
				if(!m_MenuMgr.FindMenuByPreset(ChimeraMenuPreset.RefineFactionSelectionMenu))
					m_MenuMgr.OpenMenu(ChimeraMenuPreset.RefineFactionSelectionMenu);
			}
			else
			{
				// We have a faction
				m_MenuMgr.CloseMenuByPreset(ChimeraMenuPreset.RefineFactionSelectionMenu);
				
				if (possessionState == PR_EPossessionState.DUMMY)
				{
					// We are not spawned
					if (!m_MenuMgr.FindMenuByPreset(ChimeraMenuPreset.RefineDeploymentMenu))
						m_MenuMgr.OpenMenu(ChimeraMenuPreset.RefineDeploymentMenu);
				}
				else if (possessionState == PR_EPossessionState.MAIN)
				{
					m_MenuMgr.CloseMenuByPreset(ChimeraMenuPreset.RefineDeploymentMenu);
				}
			}
		}
	}
}