//! Action to unload supplies from a Supply truck in Campaign
class PR_LoadSuppliesUserAction : ScriptedUserAction
{	
	protected int m_iCanLoadSuppliesResult = SCR_CampaignSuppliesInteractionFeedback.DO_NOT_SHOW;
	protected float m_fNextConditionCheck;
		
	protected PR_SupplyHolderComponent m_SupplyHolder;
	protected PR_PC_SupplyHolderInformerComponent m_PcSupplyHolder;
	protected RplId m_HolderRplId = RplId.Invalid();
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_SupplyHolder = PR_SupplyHolderComponent.Cast(pOwnerEntity.FindComponent(PR_SupplyHolderComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActionStart(IEntity pUserEntity)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{
	}
	
	override void PerformContinuousAction(IEntity pOwnerEntity, IEntity pUserEntity, float timeSlice)
	{
		ActionLogic(pOwnerEntity, pUserEntity);	
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		ActionLogic(pOwnerEntity, pUserEntity);		
	}
	
	void ActionLogic(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if(!m_PcSupplyHolder)
		{
			// Find local player controller
			PlayerController playerController = GetGame().GetPlayerController();
			if (!playerController)
				return;
			
			// Find network component to send RPC to server
			m_PcSupplyHolder = PR_PC_SupplyHolderInformerComponent.Cast(playerController.FindComponent(PR_PC_SupplyHolderInformerComponent));
		}
		
		if (!m_PcSupplyHolder)
			return;
		
		if(!m_HolderRplId.IsValid())
		{
			m_HolderRplId = Replication.FindId(m_SupplyHolder.m_RplComponent);
		}
		
		// Target
		if(!m_SupplyHolder.m_ActionTarget)
			return;
		
		RplId targetRplId = Replication.FindId(m_SupplyHolder.m_ActionTarget.m_RplComponent);
		if(targetRplId.IsValid())
		{
			m_PcSupplyHolder.RequestSupplyAction(m_HolderRplId, targetRplId, true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		switch (m_iCanLoadSuppliesResult)
		{			
			case SCR_CampaignSuppliesInteractionFeedback.FULL:
			{
				SetCannotPerformReason("#AR-Campaign_Action_SupplyTruckFull-UC");
				//SetCannotPerformReason("FULL");
				break;
			}
			
			case SCR_CampaignSuppliesInteractionFeedback.BASE_ENEMY:
			{
				SetCannotPerformReason("#AR-Campaign_Action_WrongBase-UC");
				//SetCannotPerformReason("ENEMY BASE");
				break;
			}
			
			case SCR_CampaignSuppliesInteractionFeedback.BASE_EMPTY:
			{
				SetCannotPerformReason("#AR-Campaign_Action_BaseDepleted-UC");
				//SetCannotPerformReason("BASE DEPLETED");
				break;
			}
		}
		
		if (m_iCanLoadSuppliesResult == SCR_CampaignSuppliesInteractionFeedback.POSSIBLE)
			return true;
		else
			return false;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (Replication.Time() >= m_fNextConditionCheck)
		{
			m_iCanLoadSuppliesResult = CanLoadSupplies(user);
			m_fNextConditionCheck += 250;
		}
		
		if (m_iCanLoadSuppliesResult == SCR_CampaignSuppliesInteractionFeedback.DO_NOT_SHOW)
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
		if (!m_SupplyHolder || m_SupplyHolder.m_aAvailableHolders.IsEmpty())
			return false;
		
		PR_SupplyHolderComponent target = m_SupplyHolder.m_ActionTarget;
		if (!target)
			return false;
				
		outName = "LOAD SUPPLIES";
		
		outName += " (";
		outName += m_SupplyHolder.m_iSupply.ToString();
		outName += "/";
		outName += m_SupplyHolder.m_iMaxSupplies.ToString();
		outName += ")";
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check the availability of this user action
	//! \param player Player trying to unload supplies
	int CanLoadSupplies(IEntity player)
	{		
		if (!player || !m_SupplyHolder)
			return SCR_CampaignSuppliesInteractionFeedback.DO_NOT_SHOW;
		
		// Target
		if (m_SupplyHolder.m_aAvailableHolders.IsEmpty())
			return SCR_CampaignSuppliesInteractionFeedback.DO_NOT_SHOW;
		
		PR_SupplyHolderComponent target = m_SupplyHolder.m_ActionTarget;
		if (!target)
		{
			return SCR_CampaignSuppliesInteractionFeedback.DO_NOT_SHOW;
		}
		
		RplId targetRplId = Replication.FindId(target);
		if (!targetRplId.IsValid())
		{
			return SCR_CampaignSuppliesInteractionFeedback.DO_NOT_SHOW;
		}
		
		if (m_SupplyHolder.m_iSupply >= m_SupplyHolder.m_iMaxSupplies)
		{
			return SCR_CampaignSuppliesInteractionFeedback.FULL;
		}
		
		if (target.m_iSupply <= 0)
		{
			return SCR_CampaignSuppliesInteractionFeedback.BASE_EMPTY;
		}
		
		return SCR_CampaignSuppliesInteractionFeedback.POSSIBLE;
	}
};
