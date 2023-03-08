// Base class for tooltips with buttons
class PR_ButtonsTooltipComponent : ScriptedWidgetComponent
{
	Widget m_wRoot;

	protected const ResourceName BUTTON_LAYOUT = "{1B2BB2D10DB1FC9F}UI/DeploymentMenu/Elements/Tooltip/ButtonTextTooltip.layout";
			
	protected SCR_ModularButtonComponent CreateButton(string buttonText)
	{
		VerticalLayoutWidget v = VerticalLayoutWidget.Cast(m_wRoot.FindAnyWidget("m_ButtonsLayout"));
		
		Widget wButton = GetGame().GetWorkspace().CreateWidgets(BUTTON_LAYOUT, v);
		SCR_ModularButtonComponent buttonComp = SCR_ModularButtonComponent.Cast(wButton.FindHandler(SCR_ModularButtonComponent));
		TextWidget wText = TextWidget.Cast(wButton.FindAnyWidget("Text"));
		wText.SetText(buttonText);
		
		return buttonComp;
	}
	
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		GetGame().GetCallqueue().CallLater(OnFrame, 0, true);
	}
	
	override void HandlerDeattached(Widget w)
	{
		GetGame().GetCallqueue().Remove(OnFrame);
	}
	
	void OnFrame()
	{
		// Remove the tooltip if cursor is not over it any more
		const float offset = 60;
		
		WorkspaceWidget ws = GetGame().GetWorkspace();
		
		int xc, yc;
		WidgetManager.GetMousePos(xc, yc);
		float xCursor = xc;
		float yCursor = yc;
		
		
		float xpos, ypos;
		m_wRoot.GetScreenPos(xpos, ypos);
		float width, height;
		m_wRoot.GetScreenSize(width, height);
		
		bool overWidget = (xCursor > xpos - offset) &&
			(xCursor < xpos + width + offset) &&
			(yCursor > ypos - offset) && 
			(yCursor < ypos + height + offset);
		
		if (!overWidget && width != 0 && height != 0)
			m_wRoot.RemoveFromHierarchy(); // bye
	}
	
	void Close()
	{
		m_wRoot.RemoveFromHierarchy();
	}
}