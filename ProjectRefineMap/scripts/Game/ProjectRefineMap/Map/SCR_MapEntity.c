modded class SCR_MapEntity
{
	// Fix for chat opening when confirming placement of marker
	// Better would be to assign chat to another key than Enter :/
	
	bool CanOpenChat()
	{
		SCR_MapCursorModule cursorModule = SCR_MapCursorModule.Cast(GetMapModule(SCR_MapCursorModule));
		if (!cursorModule)
			return true;
		
		return cursorModule.CanOpenChat();
	}
}