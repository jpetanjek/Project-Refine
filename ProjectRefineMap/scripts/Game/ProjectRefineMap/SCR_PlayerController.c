modded class SCR_PlayerController
{
	SCR_PlayerControllerGroupComponent m_GroupComponent;
	
	override void EOnInit(IEntity owner)
	{
		m_GroupComponent = SCR_PlayerControllerGroupComponent.Cast(owner.FindComponent(SCR_PlayerControllerGroupComponent));
		super.EOnInit(owner);
	}
}