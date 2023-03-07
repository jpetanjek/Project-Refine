modded class SCR_CursorCustom
{
	override void SetCursorVisual(SCR_CursorVisualState stateCfg)
	{
		super.SetCursorVisual(stateCfg);
		m_wRoot.SetZOrder(999999);
	}
}