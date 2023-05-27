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
	
	void AskBuild(PR_BuildingEntryAsset asset, PR_BuildingProviderBaseComponent buildingProvider, vector transform[4])
	{
		IEntity buildingProviderEntity = buildingProvider.GetOwner();
		RplId rplId = RplComponent.Cast(buildingProvider.GetOwner().FindComponent(RplComponent)).Id();
		Rpc(RpcAsk_Build, asset.m_sBuildingManagerPrefab, rplId, asset.m_eFlags, asset.m_iCost, transform[0], transform[1], transform[2], transform[3]);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_Build(ResourceName buildingMgrPrefab, RplId rplId, PR_EAssetBuildingFlags assetFlags, int cost, vector vAside, vector vUp, vector vDir, vector vPos)
	{
		RplComponent rplComp = RplComponent.Cast(Replication.FindItem(rplId));
		if (!rplComp)
			return;
		
		//-------------------------------------------------------------------
		// Player data
		PlayerController pc = PlayerController.Cast(GetOwner());
		int playerId = pc.GetPlayerId();
		SCR_Faction faction = SCR_Faction.Cast(PR_FactionMemberManager.GetInstance().GetPlayerFaction(playerId));
		if (!faction)
			return;
		int factionId = faction.GetId();
		IEntity playerEntity = pc.GetControlledEntity();
		if (!playerEntity)
			return;
		
		
		PR_BuildingProviderBaseComponent buildingProvider = PR_BuildingProviderBaseComponent.Cast(rplComp.GetEntity().FindComponent(PR_BuildingProviderBaseComponent));
		
		if (!buildingProvider)
			return;
		
		// Verify supply count
		if (buildingProvider.GetSupply() < cost)
			return;
		
		// Find FOB here
		PR_FobComponent fob = PR_FobComponent.Cast(buildingProvider.GetOwner().FindComponent(PR_FobComponent));
		
		// Verify that FOB exists, if the asset requires it
		if ((assetFlags & PR_EAssetBuildingFlags.REQUIRES_FOB) && !fob)
			return;
		
		// todo verify that position is not obstructed, not sure how to do it now
		
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
			buildingMgr.Init(factionId, assetFlags);
			
			if (fob)
				fob.RegisterBuildingManager(buildingMgr);
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
