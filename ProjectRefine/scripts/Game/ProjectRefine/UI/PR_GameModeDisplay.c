/*
Player's HUD, showing game mode related data.
*/

class PR_GameModeDisplay : SCR_InfoDisplay
{
	protected ref PR_GameModeDisplayWidgets widgets = new PR_GameModeDisplayWidgets();
	
	protected static const ResourceName UNOWNED_AREA_FLAG_TEXTURE = "{BD2E2F871E6A3EBF}UI/Textures/Flags/Flag_Faction_Base.edds";
	
	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);
		widgets.Init(GetRootWidget());
	}
	
	override void UpdateValues(IEntity owner, float timeSlice)
	{
		UpdateAreaOverlay();
	}
	
	void UpdateAreaOverlay()
	{
		PlayerController playerController = GetGame().GetPlayerController();
		IEntity controlledEntity;
		if (playerController)
			controlledEntity = playerController.GetControlledEntity();
		
		PR_GameMode gameMode = PR_GameMode.Cast(GetGame().GetGameMode());
		FactionManager factionManager = GetGame().GetFactionManager();
		
		if (!gameMode || !playerController || !controlledEntity || !factionManager)
		{
			ShowCurrentAreaOverlay(false);
			return;
		}
		
		PR_CaptureArea area = gameMode.GetAreaAtPos(controlledEntity.GetOrigin());
		
		if (!area)
		{
			// We are not at any area
			ShowCurrentAreaOverlay(false);
			return;
		}
		
		// We are at some area
		ShowCurrentAreaOverlay(true);
		
		int areaOwnerFactionId = area.GetOwnerFactionId();
		SCR_Faction areaOwnerFaction = SCR_Faction.Cast(factionManager.GetFactionByIndex(areaOwnerFactionId));
		
		// Flag texture
		ResourceName areaFlagTexture;
		if (areaOwnerFaction)
			areaFlagTexture = areaOwnerFaction.GetFactionFlag();
		else
			areaFlagTexture = UNOWNED_AREA_FLAG_TEXTURE;
		
		widgets.m_AreaFlagImage.LoadImageTexture(0, areaFlagTexture);
		
		// Progress bar value
		int areaPointsOwnerFactionId = area.GetPointsOwnerFactionId();
		SCR_Faction areaPointsOwnerFaction = SCR_Faction.Cast(factionManager.GetFactionByIndex(areaPointsOwnerFactionId));
		if (areaPointsOwnerFaction)
		{
			float areaPointsRel = area.GetPointsRelative();
			Color color = areaPointsOwnerFaction.GetFactionColor();
			widgets.m_AreaProgressBar.SetColor(color);
			widgets.m_AreaProgressBar.SetCurrent(areaPointsRel);
		}
		else
		{
			widgets.m_AreaProgressBar.SetCurrent(0);
		}
		
		// Area name
		string areaName = area.GetName();
		widgets.m_AreaNameText.SetText(areaName);
		
		// Lock sign
		SCR_ChimeraCharacter controlledCharacter = SCR_ChimeraCharacter.Cast(controlledEntity);
		if (controlledCharacter)
		{
			Faction myFaction = controlledCharacter.m_pFactionComponent.GetAffiliatedFaction();
			int myFactionId = factionManager.GetFactionIndex(myFaction);
			bool canCapture = area.CanBeCapturedByFaction(myFactionId); // True if we can capture this
			widgets.m_AreaLock.SetVisible(!canCapture);
		}
	}
	
	void ShowCurrentAreaOverlay(bool show)
	{
		widgets.m_CurrentAreaOverlay.SetVisible(show);
	}
}