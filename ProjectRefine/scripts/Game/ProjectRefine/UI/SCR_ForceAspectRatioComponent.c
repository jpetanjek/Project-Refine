// This is supposed to be gone from the game anyway
// We just want to disable it in our deployment menu
modded class SCR_ForceAspectRatioComponent : ScriptedWidgetComponent
{	
	//------------------------------------------------------------------------------------------------
	override bool OnUpdate(Widget w)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void UpdateSize()
	{
		
	}
};