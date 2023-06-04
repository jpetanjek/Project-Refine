[EntityEditorProps(category: "GameScripted/GameMode")]
class PR_PreparationPhaseColliderClass : GenericEntityClass
{
};

class PR_PreparationPhaseCollider : GenericEntity
{
	// Preparation phase collider
	[RplProp(onRplName: "OnCollisionChanged")]
	bool m_bColliderDisabled = false;
	
	PR_CaptureArea m_MainBase;
	
	//-------------------------------------------------------------------------------------------------------------------------------
	void PR_GameMode(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		IEntity parent = owner.GetParent();
		while(parent.GetParent())
			parent = parent.GetParent();
		
		PR_CaptureArea area = PR_CaptureArea.Cast(parent.FindComponent(PR_CaptureArea));
		if(area)
			m_MainBase = area;

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
		PR_GameMode gm = PR_GameMode.Cast(GetGame().GetGameMode());
		if(!m_bColliderDisabled && (stage == PR_EGameModeStage.LIVE || ( gm.GetArchetype() == PR_EGameModeArchetype.INVASION && m_MainBase && m_MainBase.GetInitialOwnerFactionId() == gm.GetDefendingFaction())))
		{
			m_bColliderDisabled = true;
			Replication.BumpMe();
			GetPhysics().Destroy();
		}
	}
}