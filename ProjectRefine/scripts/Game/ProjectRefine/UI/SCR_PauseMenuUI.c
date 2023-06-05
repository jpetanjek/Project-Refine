modded class PauseMenuUI
{
	// We make the pause menu modal, because when we open it over our menus, we don't want
	// to be able to interace with menus behind it
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		GetGame().GetWorkspace().AddModal(GetRootWidget(), GetRootWidget());
	}
	
	override void OnMenuClose()
	{
		super.OnMenuClose();
	}
}