modded class ScriptedDamageManagerComponent
{
	//------------------------------------------------------------------------------------------------
	ScriptInvoker GetOnDestroyed()
	{
		return GetScriptedDamageManagerData().GetOnDestroyed();
	}
	
	
	protected override void OnDamageStateChanged(EDamageState state)
	{
		super.OnDamageStateChanged(state);
		
		if (m_iDamageManagerDataIndex == -1)
			return;
		
		SCR_ScriptedDamageManagerData d = s_aScriptedDamageManagerData[m_iDamageManagerDataIndex];
		
		ScriptInvoker invoker = d.GetOnDamageStateChanged(false);
		if (invoker)
			invoker.Invoke(state);
		
		if (state == EDamageState.DESTROYED)
		{
			invoker = d.GetOnDestroyed(false);
			if (invoker)
				invoker.Invoke(GetOwner());
		}
	}
	
}