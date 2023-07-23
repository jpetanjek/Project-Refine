modded class SCR_TooltipManagerEntity : GenericEntity
{
	bool m_bCheckWidgetUnderCursor = true;
	
	void SetCheckWidgetUnderCursor(bool newValue)
	{
		m_bCheckWidgetUnderCursor = newValue;
	}
	
	static Widget CreateTooltipEx(ResourceName rsc, notnull Widget hoverWidget, bool followCursor, vector offset, SCR_ETooltipAlignmentHorizontal horizontalAlignment, SCR_ETooltipAlignmentVertical verticalAlignment, bool checkWidgetUnderCursor = true)
	{	
		SCR_TooltipManagerEntity ent = GetInstance();
		
		if (!ent)
			return null;
		
		Widget w = ent.Internal_CreateTooltip(rsc, hoverWidget, followCursor, offset, horizontalAlignment, verticalAlignment);
		ent.SetCheckWidgetUnderCursor(checkWidgetUnderCursor);
		return w;
	}
	
	override protected Widget Internal_CreateTooltip(ResourceName rsc, Widget hoverWidget, bool followCursor, vector offset, SCR_ETooltipAlignmentHorizontal horizontalAlignment, SCR_ETooltipAlignmentVertical verticalAlignment)
	{
		Widget w = super.Internal_CreateTooltip(rsc, hoverWidget, followCursor, offset, horizontalAlignment, verticalAlignment);
		SetCheckWidgetUnderCursor(true); // For all default usage we want to keep that
		
		return w;
	}
	
	// Here we disable functionality that deletes tooltip if the widget under cursor stops is not m_wHoverWidget
	override protected void EOnFrame(IEntity owner, float timeSlice)
	{	
		// Bail, deactivate, delete tooltip if hover widget is different or is gone
		bool deactivate = false;
		if (m_bCheckWidgetUnderCursor && WidgetManager.GetWidgetUnderCursor() != m_wHoverWidget)
			deactivate = true;
		else if (!m_wHoverWidget)
			deactivate = true;
		
		if (deactivate)
		{
			Internal_DeleteTooltip();
			SetFlags(EntityFlags.ACTIVE, false);			
			return;
		}
		
		// Bail, deactivate if there is no tooltip any more (smth else deleted it or we deleted it)
		if (!m_wTooltip)
		{
			SetFlags(EntityFlags.ACTIVE, false);
			return;
		}
			
		// Update current tooltip position if required
		if (m_wTooltip && m_bFollowCursor)
			UpdatePosition();
	}
}