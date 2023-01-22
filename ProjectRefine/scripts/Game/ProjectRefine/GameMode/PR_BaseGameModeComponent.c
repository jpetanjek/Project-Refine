class PR_BaseGameModeComponentClass : SCR_BaseGameModeComponentClass
{
}

class PR_BaseGameModeComponent : SCR_BaseGameModeComponent
{
	void OnCaptureAreaFactionChanged(PR_CaptureArea area, int oldFactionId, int newFactionId);	
}