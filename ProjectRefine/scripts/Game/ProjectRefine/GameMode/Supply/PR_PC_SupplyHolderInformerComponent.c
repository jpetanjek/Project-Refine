[EntityEditorProps(category: "GameScripted", description: "Relays supply holder requests from client to server.", color: "0 0 255 255")]
class PR_PC_SupplyHolderInformerComponentClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
//! Relays supply holder requests from client to server.
class PR_PC_SupplyHolderInformerComponent : ScriptComponent
{	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
	}

	//------------------------------------------------------------------------------------------------
	void PR_PC_SupplyHolderInformerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~PR_PC_SupplyHolderInformerComponent()
	{
		
	}
	
	//------------------------------------------------------------------------------------------------
	static PR_PC_SupplyHolderInformerComponent GetLocalPlayerControllerSupplyHolderInformerComponent()
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return null;
		
		PR_PC_SupplyHolderInformerComponent informer = PR_PC_SupplyHolderInformerComponent.Cast(playerController.FindComponent(PR_PC_SupplyHolderInformerComponent));
		
		return informer;
	}
	
	//------------------------------------------------------------------------------------------------
	void RequestSupplyAction(RplId source, RplId target, int amount, bool take)
	{
		Rpc(SupplyAction, source, target, amount, take);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void SupplyAction(RplId source, RplId target, int amount, bool take) // If the target will take or give supplies
	{		
		RplComponent rplComponentTarget = RplComponent.Cast(Replication.FindItem(target));
		RplComponent rplComponentSource = RplComponent.Cast(Replication.FindItem(source));
		if(!rplComponentTarget || !rplComponentSource)
			return;
		
		GenericEntity entityTarget = GenericEntity.Cast(rplComponentTarget.GetEntity());
		GenericEntity entitySource = GenericEntity.Cast(rplComponentSource.GetEntity());
		if(!entityTarget || !entitySource)
			return;
		
		PR_SupplyHolderComponent componentTarget = PR_SupplyHolderComponent.Cast(entityTarget.FindComponent(PR_SupplyHolderComponent));
		PR_SupplyHolderComponent componentSource = PR_SupplyHolderComponent.Cast(entitySource.FindComponent(PR_SupplyHolderComponent));
		if(!componentTarget || !componentSource)
			return;
		
		vector transformTarget[4];
		vector transformSource[4];
		entityTarget.GetWorldTransform(transformTarget);
		entitySource.GetWorldTransform(transformSource);
		float distance = vector.Distance(transformTarget, transformSource);
		
		if(distance > componentSource.m_fRange)
			return;
		
		if(take)
		{
			componentSource.TakeSupplies(componentTarget, amount); 
		}
		else
		{
			componentSource.GiveSupplies(componentTarget, amount);
		}
	}
}