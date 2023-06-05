modded class PauseMenuUI
{
	override void OnMenuClose()
	{
		super.OnMenuClose();
	}
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		// We make the pause menu modal, because when we open it over our menus, we don't want
		// to be able to interace with menus behind it
		GetGame().GetWorkspace().AddModal(GetRootWidget(), GetRootWidget());
		
		// Change Faction
		SCR_ButtonTextComponent comp = SCR_ButtonTextComponent.GetButtonText("ChangeFaction", m_wRoot);
		if (comp)
		{
			comp.m_OnClicked.Insert(OnChangeFaction);
		}
	}
	
	protected void OnChangeFaction()
	{
		Close();
		PR_PlayerControllerDeploymentComponent deploymentComp = PR_PlayerControllerDeploymentComponent.GetLocalInstance();
		deploymentComp.AskResetFaction();
	}
}