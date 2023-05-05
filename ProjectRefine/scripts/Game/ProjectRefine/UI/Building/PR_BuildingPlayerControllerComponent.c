[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class PR_BuildingPlayerControllerComponentClass : ScriptComponentClass
{
	// prefab properties here
};

//------------------------------------------------------------------------------------------------
/*!
	Class generated via ScriptWizard.
*/
class PR_BuildingPlayerControllerComponent : ScriptComponent
{
	static PR_BuildingPlayerControllerComponent GetLocalInstance()
	{
		PlayerController pc = GetGame().GetPlayerController();
		return PR_BuildingPlayerControllerComponent.Cast(pc.FindComponent(PR_BuildingPlayerControllerComponent));
	}
	
	void AskBuild(ResourceName prefab, vector transform[4])
	{
		Rpc(RpcAsk_Build, prefab, transform[0], transform[1], transform[2], transform[3]);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_Build(ResourceName prefab, vector vAside, vector vUp, vector vDir, vector vPos)
	{
		// todo verity position
		
		EntitySpawnParams sp = new EntitySpawnParams();
		sp.TransformMode = ETransformMode.WORLD;
		sp.Transform[0] = vAside;
		sp.Transform[1] = vUp;
		sp.Transform[2] = vDir;
		sp.Transform[3] = vPos;
			
		//GetGame().SpawnEntityPrefab(Resource.Load("{41A666D90504519C}Prefabs/Building/UnbuiltAsset.et"), GetGame().GetWorld(), sp);
		GetGame().SpawnEntityPrefab(Resource.Load(prefab), GetGame().GetWorld(), sp);
	}
	
	
	/*
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
	}

	//------------------------------------------------------------------------------------------------
	void PR_BuildingPlayerControllerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~PR_BuildingPlayerControllerComponent()
	{
	}
	*/
};
