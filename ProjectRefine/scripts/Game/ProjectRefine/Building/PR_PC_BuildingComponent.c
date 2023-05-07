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
	
	void AskBuild(ResourceName buildingMgrPrefab, vector transform[4])
	{
		Rpc(RpcAsk_Build, buildingMgrPrefab, transform[0], transform[1], transform[2], transform[3]);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_Build(ResourceName buildingMgrPrefab, vector vAside, vector vUp, vector vDir, vector vPos)
	{
		// todo verity position
		
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
		PR_BuildingManager entityTarget = PR_BuildingManager.Cast(rplComponentTarget.GetEntity());
		
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
		PR_BuildingManager entityTarget = PR_BuildingManager.Cast(rplComponentTarget.GetEntity());
		
		// TODO: Some classes can build faster than others
		entityTarget.Build(-5);
	}
};
