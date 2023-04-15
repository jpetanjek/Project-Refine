/*
This map module lets us add UI widgets to the map which are attached to some world position.
*/

[BaseContainerProps()]
class PR_MapUiElementsModule : SCR_MapModuleBase
{	
	protected ref array<PR_MapUiElementComponent> m_aUiElements = {};
	protected FrameWidget m_wFrame; // The frame where all UI elements are created
	
	
	//------------------------------------------------------------------------------------------------
	// Creates a UI widget on the map
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
	// Removes the element and its UI widget from the map
	void RemoveUiElement(notnull PR_MapUiElementComponent element)
	{
		m_aUiElements.RemoveItem(element);
		element.GetRootWidget().RemoveFromHierarchy();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update method for frame operations
	override void Update(float timeSlice)
	{
		SCR_MapEntity mapEntity = SCR_MapEntity.GetMapInstance();
		WorkspaceWidget ws = GetGame().GetWorkspace();
		
		for (int i = m_aUiElements.Count() - 1; i >= 0; i--)
		{
			PR_MapUiElementComponent c = m_aUiElements[i];
			if (!c)
			{
				m_aUiElements.Remove(i);
			}
			else
			{
				// Reposition all elements according to their world position
				vector worldPos = c.GetWorldPos();
				int screenx, screeny;
				mapEntity.WorldToScreen(worldPos[0], worldPos[2], screenx, screeny, true);
				float screenxUnscaled = ws.DPIUnscale(screenx);
				float screenyUnscaled = ws.DPIUnscale(screeny);
				FrameSlot.SetPos(c.GetRootWidget(), screenxUnscaled, screenyUnscaled);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Init method for cases where all modules and components should be loaded already so constructor cannot be used, called once after creation
	override void Init()
	{
		
	}
	
	
	//------------------------------------------------------------------------------------------------
	//! SCR_MapEntity event
	override void OnMapOpen(MapConfiguration config)
	{
		MapConfiguration mapConfig = m_MapEntity.GetMapConfig();
		m_wFrame = FrameWidget.Cast(mapConfig.RootWidgetRef.FindAnyWidget("MapUiElementsFrame"));
	}
	
	/*
	//------------------------------------------------------------------------------------------------
	//! SCR_MapEntity event
	override void OnMapClose(MapConfiguration config)
	{}
	*/
};