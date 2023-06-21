[EntityEditorProps(category: "GameScripted/GameMode")]
class PR_PreparationPhaseColliderClass : GenericEntityClass
{
};

class PR_PreparationPhaseCollider : GenericEntity
{
	// Preparation phase collider
	[RplProp(onRplName: "OnCollisionChanged")]
	bool m_bColliderDisabled = false;
	
	PR_CaptureArea m_CaptureArea;
	
	//-------------------------------------------------------------------------------------------------------------------------------
	void PR_GameMode(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		IEntity ent = owner;
		PR_CaptureArea area;
		while(ent)
		{
			area = PR_CaptureArea.Cast(ent.FindComponent(PR_CaptureArea));
			if (area)
				break;
			ent = ent.GetParent();
		}
		
		if(area)
			m_CaptureArea = area;

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
		if(!m_bColliderDisabled && (stage == PR_EGameModeStage.LIVE || ( gm.GetArchetype() == PR_EGameModeArchetype.INVASION && m_CaptureArea && m_CaptureArea.GetOwnerFactionId() == gm.GetDefendingFactionId())))
		{
			m_bColliderDisabled = true;
			Replication.BumpMe();
			GetPhysics().Destroy();
		}
	}
}