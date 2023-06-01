modded class SCR_PossessingManagerComponent
{
	// Same as base class method, we just don't invoke editor and don't unpossess entity
	
	override bool GetPlayerID(IEntity entity, out int playerID)
	{
		if (playerID > 0)
		{
			//--- Controlled entity
			if (entity != GetMainEntity(playerID))
			{
				//--- Switch to main entity when the possessed one dies
				SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerID));
				if (playerController)
				{
					//--- Open editor, assume it will stop possessing (ToDo: No direct editor reference here)
					SCR_EditorManagerCore core = SCR_EditorManagerCore.Cast(SCR_EditorManagerCore.GetInstance(SCR_EditorManagerCore));
					if (core)
					{
						SCR_EditorManagerEntity editorManager = core.GetEditorManager(playerID);
						if (editorManager)
						{
							//editorManager.Open();
							return false;
						}
					}
					
					//--- No editor, stop possessing right now					
					//playerController.SetPossessedEntity(null);
				}
				
				playerID = 0;
			}
		}
		else
		{
			//--- Main entity
			playerID = GetPlayerIdFromMainEntity(entity);
		}
		
		return true;
	}
}