class PR_PlayerControllerDeploymentComponentClass : ScriptComponentClass
{
}

class PR_PlayerControllerDeploymentComponent : ScriptComponent
{
	static PR_PlayerControllerDeploymentComponent GetLocalInstance()
	{
		PlayerController pc = GetGame().GetPlayerController();
		return PR_PlayerControllerDeploymentComponent.Cast(pc.FindComponent(PR_PlayerControllerDeploymentComponent));
	}
	
	void AskDequeueAtSpawnPoint(notnull PR_BaseSpawnPoint spawnPoint)
	{
		RplId rplId = Replication.FindId(spawnPoint);
		Rpc(RpcAsk_DequeueAtSpawnPoint, rplId);
	}
	
	void AskEnqueueAtSpawnPoint(notnull PR_BaseSpawnPoint spawnPoint)
	{
		RplId rplId = Replication.FindId(spawnPoint);
		Rpc(RpcAsk_EnqueueAtSpawnPoint, rplId);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_DequeueAtSpawnPoint(RplId spawnPointId)
	{
		PR_BaseSpawnPoint spawnPoint = PR_BaseSpawnPoint.Cast(Replication.FindItem(spawnPointId));
		
		if (!spawnPoint)
			return;
		
		PlayerController pc = PlayerController.Cast(GetOwner());
		
		spawnPoint.DequeuePlayer(pc.GetPlayerId());
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_EnqueueAtSpawnPoint(RplId spawnPointId)
	{
		PR_BaseSpawnPoint spawnPoint = PR_BaseSpawnPoint.Cast(Replication.FindItem(spawnPointId));
		
		if (!spawnPoint)
			return;
		
		PlayerController pc = PlayerController.Cast(GetOwner());
		
		spawnPoint.EnqueuePlayer(pc.GetPlayerId());
	}
	
	
	
	void AskResetFaction()
	{
		Rpc(RpcAsk_ResetFaction);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ResetFaction()
	{
		PlayerController pc = PlayerController.Cast(GetOwner());
		int playerId = pc.GetPlayerId();
		
		// Destroy controlled entity
		IEntity controlledEntity = pc.GetControlledEntity();
		if (controlledEntity)
		{
			DamageManagerComponent damageMgr = DamageManagerComponent.Cast(controlledEntity.FindComponent(DamageManagerComponent));
			damageMgr.SetHealthScaled(0.0);
		}
		
		// Remove from group
		SCR_PlayerControllerGroupComponent groupComp = SCR_PlayerControllerGroupComponent.Cast(pc.FindComponent(SCR_PlayerControllerGroupComponent));
		groupComp.LeaveGroup();
		
		// Reset faction
		PR_FactionMemberManager factionMemberMgr = PR_FactionMemberManager.GetInstance();
		factionMemberMgr.SetPlayerFaction(-1, playerId);		
	}
}