modded class SCR_RespawnComponent
{
	void TempRequestSpawnPlayer()
	{
		Rpc(RpcAsk_TempRequestSpawnPlayer);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_TempRequestSpawnPlayer()
	{
		PR_FactionManager factionManager = PR_FactionManager.Cast(GetGame().GetFactionManager());
		if(!factionManager)
			return;
		
		int playerID = PlayerController.Cast(GetPlayerController()).GetPlayerId();
		
		factionManager.SetPlayerFaction(0 , playerID);
		
		int factionIdx = factionManager.GetPlayerFactionIndex(playerID);
		array<PR_SpawnPoint> allSpawnPoints = PR_SpawnPoint.GetAll();
		
		for(int i = 0; i < allSpawnPoints.Count(); i++)
		{
			if(allSpawnPoints[i].GetFactionId() == factionIdx)
			{
				allSpawnPoints[i].EnqueuePlayer(playerID);
				return;
			}
		}
	}
}