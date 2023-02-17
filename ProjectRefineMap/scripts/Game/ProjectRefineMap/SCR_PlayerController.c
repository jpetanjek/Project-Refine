void OnControlledEntityChangedDelegate(IEntity oldEntity, IEntity newEntity);
typedef func OnControlledEntityChangedDelegate;

modded class SCR_PlayerController
{
	SCR_PlayerControllerGroupComponent m_GroupComponent;
	
	protected IEntity m_PrevControlledEntity;
	
	// Works on server and client
	ref ScriptInvokerBase<OnControlledEntityChangedDelegate> m_OnControlledEntityChanged2 = new ScriptInvokerBase<OnControlledEntityChangedDelegate>(); // old, new
	
	override void EOnInit(IEntity owner)
	{
		m_GroupComponent = SCR_PlayerControllerGroupComponent.Cast(owner.FindComponent(SCR_PlayerControllerGroupComponent));
		super.EOnInit(owner);
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		
		// This is really silly since there is already OnControlledEntityChanged event,
		// but it doesn't work on server for client-owned player controllers
		IEntity controlledEntity = GetControlledEntity();
		if (controlledEntity != m_PrevControlledEntity)
			m_OnControlledEntityChanged2.Invoke(m_PrevControlledEntity, controlledEntity);
		
		m_PrevControlledEntity = controlledEntity;
	}
}