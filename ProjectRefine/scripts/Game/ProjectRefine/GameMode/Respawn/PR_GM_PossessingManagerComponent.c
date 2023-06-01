class PR_GM_PossessingManagerComponentClass : PR_BaseGameModeComponentClass
{
}

class PR_GM_PossessingManagerComponent : PR_BaseGameModeComponent
{
	override void OnPostInit(IEntity owner)
	{
		if (Replication.IsServer())
			SetEventMask(owner, EntityEvent.FRAME);
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		array<int> playerIds = {};
		GetGame().GetPlayerManager().GetPlayers(playerIds);
		foreach (int id : playerIds)
		{
			PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(id);
			if (!pc)
				continue;
			
			PR_PC_PossessionManagerComponent possessionMgr = PR_PC_PossessionManagerComponent.Cast(pc.FindComponent(PR_PC_PossessionManagerComponent));
			
			if (possessionMgr)
				possessionMgr.EOnFrameServer(pc, timeSlice);
		}
	}
}