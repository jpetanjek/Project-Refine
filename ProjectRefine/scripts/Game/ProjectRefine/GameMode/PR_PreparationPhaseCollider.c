[EntityEditorProps(category: "GameScripted/GameMode")]
class PR_PreparationPhaseColliderClass : GenericEntityClass
{
};

class PR_PreparationPhaseCollider : GenericEntity
{
	// Preparation phase collider
	[RplProp(onRplName: "OnCollisionChanged")]
	bool m_bColliderDisabled = false;
	
	//-------------------------------------------------------------------------------------------------------------------------------
	void PR_GameMode(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);

		PR_GameMode gameMode = PR_GameMode.Cast(GetGame().GetGameMode());
		
		if(gameMode)
			gameMode.m_OnGameModeStageChanged.Insert(OnGameModeStageChanged);		
	}
	
	//-------------------------------------------------------------------------------------------------------------------------------
	void OnCollisionChanged()
	{
		if(m_bColliderDisabled)
			GetPhysics().Destroy();
	}
	
	void OnGameModeStageChanged(PR_EGameModeStage stage)
	{
		if(!m_bColliderDisabled && stage == PR_EGameModeStage.LIVE)
		{
			m_bColliderDisabled = true;
			GetPhysics().Destroy();
		}
	}
}