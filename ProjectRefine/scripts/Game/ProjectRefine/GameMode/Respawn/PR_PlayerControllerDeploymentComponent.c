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
	
	void AskDequeueAtSpawnPoint(notnull PR_SpawnPoint spawnPoint)
	{
		RplId rplId = Replication.FindId(spawnPoint);
		Rpc(RpcAsk_DequeueAtSpawnPoint, rplId);
	}
	
	void AskEnqueueAtSpawnPoint(notnull PR_SpawnPoint spawnPoint)
	{
		RplId rplId = Replication.FindId(spawnPoint);
		Rpc(RpcAsk_EnqueueAtSpawnPoint, rplId);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_DequeueAtSpawnPoint(RplId spawnPointId)
	{
		PR_SpawnPoint spawnPoint = PR_SpawnPoint.Cast(Replication.FindItem(spawnPointId));
		
		if (!spawnPoint)
			return;
		
		PlayerController pc = PlayerController.Cast(GetOwner());
		
		spawnPoint.DequeuePlayer(pc.GetPlayerId());
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_EnqueueAtSpawnPoint(RplId spawnPointId)
	{
		PR_SpawnPoint spawnPoint = PR_SpawnPoint.Cast(Replication.FindItem(spawnPointId));
		
		if (!spawnPoint)
			return;
		
		PlayerController pc = PlayerController.Cast(GetOwner());
		
		spawnPoint.EnqueuePlayer(pc.GetPlayerId());
	}
}