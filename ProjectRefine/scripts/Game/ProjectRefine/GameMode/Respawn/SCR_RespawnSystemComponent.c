modded class SCR_RespawnSystemComponent
{
	
	override int GetFactionPlayerCount(Faction faction)
	{
		PR_FactionMemberManager factionMemberManager = PR_FactionMemberManager.GetInstance();
		if(!factionMemberManager)
			return null;
		
		PR_RoleToPlayer members = factionMemberManager.GetFactionMembers(faction);
		if(members)
			return members.m_aPlayers.Count();
		else
			return 0;
	}

    override static Faction GetLocalPlayerFaction(IEntity player = null)
    {
		PR_FactionMemberManager factionMemberManager = PR_FactionMemberManager.GetInstance();
		if(!factionMemberManager)
			return null;
		
		int playerID = SCR_PlayerController.GetLocalPlayerId();
		return factionMemberManager.GetPlayerFaction(playerID);
    }
	
	override Faction GetPlayerFaction(int playerId)
	{
		PR_FactionMemberManager factionMemberManager = PR_FactionMemberManager.GetInstance();
		if(!factionMemberManager)
			return null;
		
		return factionMemberManager.GetPlayerFaction(playerId);
	}
	
	override void SetPlayerFaction(int playerId, int factionIndex)
	{
		PR_FactionMemberManager factionMemberManager = PR_FactionMemberManager.GetInstance();
		if(!factionMemberManager)
			return;
		
		factionMemberManager.SetPlayerFaction(factionIndex, playerId);
		
		if (m_pGameMode)
			m_pGameMode.HandleOnFactionAssigned(playerId, factionMemberManager.GetPlayerFaction(playerId));
	}
	
}