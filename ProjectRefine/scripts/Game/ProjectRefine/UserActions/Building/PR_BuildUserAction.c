//! Action to build a managed building
class PR_BuildUserAction : ScriptedUserAction
{
	protected PR_BuildingManager m_BuildingManager;
	protected PR_BuildingPlayerControllerComponent m_BuildingPC;
	protected RplId m_TargetRplId = RplId.Invalid();

	protected int m_iBuildResult;
	protected float m_fNextConditionCheck;

	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_BuildingManager = PR_BuildingManager.Cast(pOwnerEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActionStart(IEntity pUserEntity)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		if(!m_BuildingPC)
		{
			// Find local player controller
			PlayerController playerController = GetGame().GetPlayerController();
			if (!playerController)
				return;
			
			// Find network component to send RPC to server
			m_BuildingPC = PR_BuildingPlayerControllerComponent.Cast(playerController.FindComponent(PR_BuildingPlayerControllerComponent));
		}
		
		if(!m_BuildingPC)
			return;
		
		if(!m_TargetRplId.IsValid())
		{
			m_TargetRplId = Replication.FindId(m_BuildingManager.m_RplComponent);
		}
		
		m_BuildingPC.AskBuildAction(m_TargetRplId);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		switch (m_iBuildResult)
		{			
			case PR_BuildActionFeedback.FULLY_BUILT:
			{
				SetCannotPerformReason("FULLY BUILT");
				break;
			}
		}
		
		if (m_iBuildResult == PR_BuildActionFeedback.POSSIBLE)
			return true;
		else
			return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (Replication.Time() >= m_fNextConditionCheck)
		{
			m_iBuildResult = CanBuild(user);
			m_fNextConditionCheck += 250;
		}
		
		if (m_iBuildResult == PR_BuildActionFeedback.DO_NOT_SHOW)
			return false;
		else
			return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		// Local - true; Networked - false;
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (!m_BuildingManager)
			return false;
				
		outName = "BUILD";
		
		outName += " (";
		outName += m_BuildingManager.m_iHealth.ToString();
		outName += "/";
		outName += m_BuildingManager.m_iMaxHealth.ToString();
		outName += ")";
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check the availability of this user action
	//! \param player Player trying to unload supplies
	int CanBuild(IEntity player)
	{		
		if (!player || !m_BuildingManager)
			return PR_BuildActionFeedback.DO_NOT_SHOW;
				
		if (m_BuildingManager.m_iHealth >= m_BuildingManager.m_iMaxHealth)
		{
			return PR_BuildActionFeedback.FULLY_BUILT;
		}
		
		return PR_BuildActionFeedback.POSSIBLE;
	}
	
}

enum PR_BuildActionFeedback
{
	DO_NOT_SHOW = 0,
	POSSIBLE = 1,
	FULLY_BUILT = 2
};