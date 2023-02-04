// This adds an OnDestroyed event which passes the pointer to Entity.
modded class SCR_ScriptedDamageManagerData
{
	protected ref ScriptInvoker m_OnDestroyed; // (IEntity entity)

	//------------------------------------------------------------------------------------------------
	override void Reset()
	{
		super.Reset();
		delete m_OnDestroyed;
	}

	//------------------------------------------------------------------------------------------------
	ScriptInvoker GetOnDestroyed(bool createNew = true)
	{
		if (!m_OnDestroyed && createNew)
			m_OnDestroyed = new ScriptInvoker();
		return m_OnDestroyed;
	}
};