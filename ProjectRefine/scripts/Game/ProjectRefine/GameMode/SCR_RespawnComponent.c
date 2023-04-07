modded class SCR_RespawnComponent
{
	void TempRequestSpawnPlayer()
	{
		Rpc(RpcAsk_TempRequestSpawnPlayer);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_TempRequestSpawnPlayer()
	{	
		PR_FactionMemberManager factionMemberManager = PR_FactionMemberManager.Cast(GetGame().GetGameMode().FindComponent(PR_FactionMemberManager));
		if(!factionMemberManager)
			return;
		
		int playerID = PlayerController.Cast(GetPlayerController()).GetPlayerId();
		
		factionMemberManager.SetPlayerFaction(0 , playerID);
		
		int factionIdx = factionMemberManager.GetPlayerFactionIndex(playerID);
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