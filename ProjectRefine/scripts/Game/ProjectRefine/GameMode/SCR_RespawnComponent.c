modded class SCR_RespawnComponent
{
	void TempRequestSpawnPlayer()
	{
		Rpc(RpcAsk_TempRequestSpawnPlayer);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_TempRequestSpawnPlayer()
	{
		SCR_RespawnSystemComponent respawnSystem = SCR_RespawnSystemComponent.GetInstance();
		
		int playerId = PlayerController.Cast(GetPlayerController()).GetPlayerId();
		
		string prefab = "{26A9756790131354}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_Rifleman.et";
		vector position = Vector(68.12 + Math.RandomFloat(-10, 10), 1, 425.008);
		respawnSystem.CustomRespawn(playerId, prefab, position);
	}
}