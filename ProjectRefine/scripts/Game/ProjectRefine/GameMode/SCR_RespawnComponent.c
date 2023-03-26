modded class SCR_RespawnComponent
{
	void TempRequestSpawnPlayer()
	{
		Rpc(RpcAsk_TempRequestSpawnPlayer);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_TempRequestSpawnPlayer()
	{
		/*
		SCR_RespawnSystemComponent respawnSystem = SCR_RespawnSystemComponent.GetInstance();
		
		int playerId = PlayerController.Cast(GetPlayerController()).GetPlayerId();
		
		string prefab = "{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et";
		vector position = Vector(68.12 + Math.RandomFloat(-10, 10), 1, 425.008);
		respawnSystem.CustomRespawn(playerId, prefab, position);
		*/
		
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
				// Usually here we would remove from all other spawn points (/his previous one)
				return;
			}
		}
	}
}