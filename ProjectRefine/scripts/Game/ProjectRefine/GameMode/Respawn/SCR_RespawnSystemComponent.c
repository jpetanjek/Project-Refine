modded class SCR_RespawnSystemComponent
{

    override static Faction GetLocalPlayerFaction(IEntity player = null)
    {
        PR_FactionManager factionManager = PR_FactionManager.Cast(GetGame().GetFactionManager());
		if(!factionManager)
			return null;
		
		int playerID = SCR_PlayerController.GetLocalPlayerId();
		return factionManager.GetPlayerFaction(playerID);
    }
	
	override Faction GetPlayerFaction(int playerId)
	{
		PR_FactionManager factionManager = PR_FactionManager.Cast(GetGame().GetFactionManager());
		if(!factionManager)
			return null;
		
		return factionManager.GetPlayerFaction(playerId);
	}
	
	override void SetPlayerFaction(int playerId, int factionIndex)
	{
		PR_FactionManager factionManager = PR_FactionManager.Cast(GetGame().GetFactionManager());
		if(!factionManager)
			return;
		
		factionManager.SetPlayerFaction(factionIndex, playerId);
	}
	
}