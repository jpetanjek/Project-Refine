//! Action to unbuild a managed building
class PR_UnbuildUserAction : ScriptedUserAction
{
	protected PR_BuildingManager m_BuildingManager;
	protected PR_PC_BuildingComponent m_BuildingPC;
	protected RplId m_TargetRplId = RplId.Invalid();

	protected int m_iBuildResult;
	protected float m_fNextConditionCheck;
	
	protected float m_fActionPerformTime = 0;

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
	
	void ActionLogic(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if(!m_BuildingPC)
		{
			// Find local player controller
			PlayerController playerController = GetGame().GetPlayerController();
			if (!playerController)
				return;
			
			// Find network component to send RPC to server
			m_BuildingPC = PR_PC_BuildingComponent.Cast(playerController.FindComponent(PR_PC_BuildingComponent));
		}
		
		if(!m_BuildingPC)
			return;
		
		if(!m_TargetRplId.IsValid())
		{
			m_TargetRplId = Replication.FindId(m_BuildingManager.m_RplComponent);
		}
		
		m_BuildingPC.AskUnbuildAction(m_TargetRplId);
	}
	
	override void PerformContinuousAction(IEntity pOwnerEntity, IEntity pUserEntity, float timeSlice)
	{
		m_fActionPerformTime += timeSlice;
		if(m_fActionPerformTime > 0.33)
		{
			m_fActionPerformTime = 0;
			ActionLogic(pOwnerEntity, pUserEntity);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		ActionLogic(pOwnerEntity, pUserEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{	
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
				
		outName = "UNBUILD";
		
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
		
		return PR_BuildActionFeedback.POSSIBLE;
		
		// TODO if player is SL don't allow him to build
	}
}