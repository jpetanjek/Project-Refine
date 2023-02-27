/*
Component attached to widget which will be rendered on the map according to its world pos.
*/

class PR_MapUiElementComponent : ScriptedWidgetComponent
{
	protected vector m_vWorldPos;
	protected Widget m_wRoot;
	
	void SetWorldPos(vector worldPos) { m_vWorldPos = worldPos; }
	vector GetWorldPos() { return m_vWorldPos; }
	
	Widget GetRootWidget() { return m_wRoot; }
	
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		
		// The widget must be center-aligned
		FrameSlot.SetAlignment(w, 0.5, 0.5);
	}
}