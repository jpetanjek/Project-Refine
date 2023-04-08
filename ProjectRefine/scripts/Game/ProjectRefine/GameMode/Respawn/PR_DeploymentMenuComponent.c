class PR_DeploymentMenuComponentClass : PR_BaseGameModeComponentClass
{
}

class PR_DeploymentMenuComponent : PR_BaseGameModeComponent
{	
	override void OnPostInit(IEntity owner)
	{
		// Must run only on those with UI
		if (!System.IsConsoleApp())
			SetEventMask(owner, EntityEvent.FRAME);
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		SCR_RespawnSystemComponent respawnSystem = SCR_RespawnSystemComponent.GetInstance();
		PlayerController pc = GetGame().GetPlayerController();
		MenuManager menuMgr = GetGame().GetMenuManager();
		
		if (!respawnSystem.GetLocalPlayerFaction())
		{
			// We don't have faction
			if(!menuMgr.FindMenuByPreset(ChimeraMenuPreset.RefineFactionSelectionMenu))
				menuMgr.OpenMenu(ChimeraMenuPreset.RefineFactionSelectionMenu);
		}
		else
		{
			menuMgr.CloseMenuByPreset(ChimeraMenuPreset.RefineFactionSelectionMenu);
			
			if (!pc.GetControlledEntity())
			{
				// We are not spawned
				if (!menuMgr.FindMenuByPreset(ChimeraMenuPreset.RefineDeploymentMenu))
					menuMgr.OpenMenu(ChimeraMenuPreset.RefineDeploymentMenu);
			}
			else
			{
				menuMgr.CloseMenuByPreset(ChimeraMenuPreset.RefineDeploymentMenu);
			}
		}
		
		
	}
}