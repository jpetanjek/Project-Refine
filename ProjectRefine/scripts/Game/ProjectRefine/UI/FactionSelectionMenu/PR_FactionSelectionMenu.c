class PR_FactionSelecitonMenu : ChimeraMenuBase
{
	protected ref PR_FactionSelectionMenuWidgets widgets = new PR_FactionSelectionMenuWidgets();
	
	//-----------------------------------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		widgets.Init(GetRootWidget());
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	override void OnMenuClose()
	{
		super.OnMenuClose();
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------
	override void OnMenuUpdate(float tDelta)
	{
		
	}
	
	
}
