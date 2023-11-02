//! Action to unload supplies from a Supply truck in Campaign
class PR_SupplyTargetUserAction : ScriptedUserAction
{	
	protected int m_iCanTargetSupplyResult = SCR_CampaignSuppliesInteractionFeedback.DO_NOT_SHOW;
	protected float m_fNextConditionCheck;
		
	protected PR_SupplyHolderComponent m_SupplyHolder;
	protected PR_PC_SupplyHolderInformerComponent m_PcSupplyHolder;
	protected RplId m_HolderRplId;
	
	protected int m_iSelectionIdx = 0;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_SupplyHolder = PR_SupplyHolderComponent.Cast(pOwnerEntity.FindComponent(PR_SupplyHolderComponent));
	}
	
	protected void RefreshTarget()
	{
		// Target
		if(!m_SupplyHolder || m_SupplyHolder.m_aAvailableHolders.IsEmpty())
			return;
		
		// If we got to end, loop back to start
		if(m_SupplyHolder.m_aAvailableHolders.Count() == m_iSelectionIdx)
		{
			m_iSelectionIdx = 0;
		}
		
		// If we have corrupted info go to new end
		if(!m_SupplyHolder.m_aAvailableHolders.IsIndexValid(m_iSelectionIdx))
		{
			m_iSelectionIdx = m_SupplyHolder.m_aAvailableHolders.Count() - 1;
		}
		
		if( m_SupplyHolder.m_aAvailableHolders.IsIndexValid(m_iSelectionIdx))
		{
			m_SupplyHolder.m_ActionTarget = m_SupplyHolder.m_aAvailableHolders[m_iSelectionIdx];
		}
		else
		{
			m_SupplyHolder.m_ActionTarget = null;
		}
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
		if(!m_PcSupplyHolder)
		{
			// Find local player controller
			PlayerController playerController = GetGame().GetPlayerController();
			if (!playerController)
				return;
			
			// Find network component to send RPC to server
			m_PcSupplyHolder = PR_PC_SupplyHolderInformerComponent.Cast(playerController.FindComponent(PR_PC_SupplyHolderInformerComponent));
		}
		
		if (!m_PcSupplyHolder || !m_SupplyHolder)
			return;
		
		if(!m_HolderRplId.IsValid())
		{
			m_HolderRplId = Replication.FindId(m_SupplyHolder);
		}
		
		// Increment selection
		m_iSelectionIdx++;
		
		RefreshTarget();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		switch (m_iCanTargetSupplyResult)
		{			
			case SCR_CampaignSuppliesInteractionFeedback.EMPTY:
			{
				string outName;
				outName += "NO SUPPLY HOLDERS CLOSE ENOUGH ";
				outName += " (";
				outName +=  m_SupplyHolder.m_iSupply.ToString();
				outName += "/";
				outName +=  m_SupplyHolder.m_iMaxSupplies.ToString();
				outName += ")";
				
				SetCannotPerformReason(outName);
				break;
			}
		}
		
		if (m_iCanTargetSupplyResult == SCR_CampaignSuppliesInteractionFeedback.POSSIBLE)
			return true;
		else
			return false;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if(/*m_SupplyHolder.m_RplComponent && m_SupplyHolder.m_RplComponent.IsProxy() &&*/ m_SupplyHolder.m_bCanTransact)
		{
			m_SupplyHolder.UpdateAvailableHolders();
		}
		
		if (GetGame().GetWorld().GetWorldTime() >= m_fNextConditionCheck)
		{
			m_iCanTargetSupplyResult = CanTargetSupplies(user);
			m_fNextConditionCheck += 250;
		}
		
		if (m_iCanTargetSupplyResult == SCR_CampaignSuppliesInteractionFeedback.DO_NOT_SHOW)
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
		
		if (!m_SupplyHolder.m_ActionTarget)
			return false;
		
		outName += "[";
		outName += (m_SupplyHolder.m_aAvailableHolders.Find(m_SupplyHolder.m_ActionTarget) + 1).ToString();
		outName += "/";
		outName += m_SupplyHolder.m_aAvailableHolders.Count().ToString();
		outName += "] ";
		
		outName += GetSupplyHolderName(m_SupplyHolder.m_ActionTarget);
		
		outName += " (";
		outName +=  m_SupplyHolder.m_ActionTarget.m_iSupply.ToString();
		outName += "/";
		outName +=  m_SupplyHolder.m_ActionTarget.m_iMaxSupplies.ToString();
		outName += ")";
	
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected string GetSupplyHolderName(PR_SupplyHolderComponent supplyHolder)
	{
		// Is it on capture area?
		PR_CaptureArea ca = PR_CaptureArea.Cast(supplyHolder.GetOwner().FindComponent(PR_CaptureArea));
		if (ca)
			return ca.GetName();
		
		// Is it on FOB?
		PR_FobComponent fob = PR_FobComponent.Cast(supplyHolder.GetOwner().FindComponent(PR_FobComponent));
		if (fob)
			return "FOB";
		
		// Otherwise it's on a vehicle
		IEntity rootEntity = supplyHolder.GetOwner();
		while (rootEntity.GetParent())
			rootEntity = rootEntity.GetParent();
		
		SCR_EditableVehicleComponent editableComp = SCR_EditableVehicleComponent.Cast(rootEntity.FindComponent(SCR_EditableVehicleComponent));
		if (editableComp)
		{
			SCR_UIInfo uiInfo = editableComp.GetInfo();
			if (uiInfo)
				return uiInfo.GetName();
		}
		
		return "Unknown";
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check the availability of this user action
	//! \param player Player trying to target
	int CanTargetSupplies(IEntity player)
	{		
		RefreshTarget();
		
		if (!player || !m_SupplyHolder)
			return SCR_CampaignSuppliesInteractionFeedback.DO_NOT_SHOW;
		
		// Target
		if (m_SupplyHolder.m_aAvailableHolders.IsEmpty())
			return SCR_CampaignSuppliesInteractionFeedback.EMPTY;
		
		return SCR_CampaignSuppliesInteractionFeedback.POSSIBLE;
	}
};
