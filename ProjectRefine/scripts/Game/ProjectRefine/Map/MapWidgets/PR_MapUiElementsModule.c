/*
This map module lets us add UI widgets to the map which are attached to some world position.
*/

[BaseContainerProps()]
class PR_MapUiElementsModule : SCR_MapModuleBase
{	
	protected ref array<PR_MapUiElementComponent> m_aUiElements = {};
	protected FrameWidget m_wFrame; // The frame where all UI elements are created
	
	
	//------------------------------------------------------------------------------------------------
	PR_MapUiElementComponent CreateUiElement(ResourceName widgetLayout, vector worldPos)
	{
		Widget w = GetGame().GetWorkspace().CreateWidgets(widgetLayout, m_wFrame);
		PR_MapUiElementComponent uiElementComp = PR_MapUiElementComponent.Cast(w.FindHandler(PR_MapUiElementComponent));
		
		if (!uiElementComp)
			return null;
		
		m_aUiElements.Insert(uiElementComp);
		uiElementComp.SetWorldPos(worldPos);
		
		return uiElementComp;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update method for frame operations
	override void Update(float timeSlice)
	{
		SCR_MapEntity mapEntity = SCR_MapEntity.GetMapInstance();
		WorkspaceWidget ws = GetGame().GetWorkspace();
		
		// Reposition all elements according to their world position
		foreach (PR_MapUiElementComponent c : m_aUiElements)
		{
			vector worldPos = c.GetWorldPos();
			int screenx, screeny;
			mapEntity.WorldToScreen(worldPos[0], worldPos[2], screenx, screeny, true);
			float screenxUnscaled = ws.DPIUnscale(screenx);
			float screenyUnscaled = ws.DPIUnscale(screeny);
			FrameSlot.SetPos(c.GetRootWidget(), screenxUnscaled, screenyUnscaled);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Init method for cases where all modules and components should be loaded already so constructor cannot be used, called once after creation
	override void Init()
	{
		MapConfiguration mapConfig = m_MapEntity.GetMapConfig();
		m_wFrame = FrameWidget.Cast(mapConfig.RootWidgetRef.FindAnyWidget("MapUiElementsFrame"));
	}
	
	/*
	//------------------------------------------------------------------------------------------------
	//! SCR_MapEntity event
	override void OnMapOpen(MapConfiguration config)
	{}
	
	//------------------------------------------------------------------------------------------------
	//! SCR_MapEntity event
	override void OnMapClose(MapConfiguration config)
	{}
	*/
};