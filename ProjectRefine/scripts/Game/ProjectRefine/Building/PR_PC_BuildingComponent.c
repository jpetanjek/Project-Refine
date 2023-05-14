[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class PR_PC_BuildingComponentClass : ScriptComponentClass
{
	// prefab properties here
};

// Component which must be attached to player controller for building to work

class PR_PC_BuildingComponent : ScriptComponent
{
	static PR_PC_BuildingComponent GetLocalInstance()
	{
		PlayerController pc = GetGame().GetPlayerController();
		return PR_PC_BuildingComponent.Cast(pc.FindComponent(PR_PC_BuildingComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	// Creation of asset
	
	void AskBuild(ResourceName buildingMgrPrefab, PR_BuildingProviderBaseComponent buildingProvider, int cost, vector transform[4])
	{
		IEntity buildingProviderEntity = buildingProvider.GetOwner();
		RplId rplId = RplComponent.Cast(buildingProvider.GetOwner().FindComponent(RplComponent)).Id();
		Rpc(RpcAsk_Build, buildingMgrPrefab, rplId, cost, transform[0], transform[1], transform[2], transform[3]);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_Build(ResourceName buildingMgrPrefab, RplId rplId, int cost, vector vAside, vector vUp, vector vDir, vector vPos)
	{
		RplComponent rplComp = RplComponent.Cast(Replication.FindItem(rplId));
		if (!rplComp)
			return;
		
		PR_BuildingProviderBaseComponent buildingProvider = PR_BuildingProviderBaseComponent.Cast(rplComp.GetEntity().FindComponent(PR_BuildingProviderBaseComponent));
		
		if (!buildingProvider)
			return;
		
		// Verity supply count
		if (buildingProvider.GetSupply() < cost)
			return;
		
		// todo verity position, not sure how to do it now
		
		EntitySpawnParams sp = new EntitySpawnParams();
		sp.TransformMode = ETransformMode.WORLD;
		sp.Transform[0] = vAside;
		sp.Transform[1] = vUp;
		sp.Transform[2] = vDir;
		sp.Transform[3] = vPos;
			
		IEntity ent = GetGame().SpawnEntityPrefab(Resource.Load(buildingMgrPrefab), GetGame().GetWorld(), sp);
		
		PR_BuildingManager buildingMgr = PR_BuildingManager.Cast(ent);
		if (!buildingMgr)
			Print(string.Format("Built asset is not PR_BuildingManager: %1", buildingMgrPrefab), LogLevel.ERROR);
		else
		{
			// Initialize the Building Manager on created entity
			
			// Find player's faction
			PlayerController pc = PlayerController.Cast(GetOwner());
			int playerId = pc.GetPlayerId();
			PR_FactionMemberManager factionMemberManager = PR_FactionMemberManager.GetInstance();
			SCR_Faction faction = SCR_Faction.Cast(factionMemberManager.GetPlayerFaction(playerId));
			
			if (faction)
			{
				int factionId = faction.GetId();
				buildingMgr.Init(factionId);
			}
		}
		
		// Consume supplies from building provider
		buildingProvider.AddSupplies(-cost);
	}
	
	//------------------------------------------------------------------------------------------------
	// Build actions
	
	void AskBuildAction(RplId target)
	{
		Rpc(Rpc_AskBuildAction, target);	
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void Rpc_AskBuildAction(RplId target)
	{
		RplComponent rplComponentTarget = RplComponent.Cast(Replication.FindItem(target));
		if(!rplComponentTarget)
			return;
		
		PR_BuildingManager entityTarget = PR_BuildingManager.Cast(rplComponentTarget.GetEntity());
		if(!entityTarget)
			return;
		
		// TODO: Some classes can build faster than others
		entityTarget.Build(5);
	}
	
	//------------------------------------------------------------------------------------------------
	// Unbuild actions
	
	void AskUnbuildAction(RplId target)
	{
		Rpc(Rpc_AskUnbuildAction, target);	
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void Rpc_AskUnbuildAction(RplId target)
	{
		RplComponent rplComponentTarget = RplComponent.Cast(Replication.FindItem(target));
		if(!rplComponentTarget)
			return;
		
		PR_BuildingManager entityTarget = PR_BuildingManager.Cast(rplComponentTarget.GetEntity());
		if(!entityTarget)
			return;
		
		// TODO: Some classes can build faster than others
		entityTarget.Build(-5);
	}
};
