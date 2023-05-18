modded class SCR_GroupsManagerComponent
{
	[Attribute(desc: "Array of frequencies for commander channel. One frequency per faction.")]
	ref array<int> m_iCommanderFrequencies;
	
	[Attribute(desc: "Array of frequencies for squadless players. One frequency per faction.")]
	ref array<int> m_iSquadlessFrequencies;
	
	static SCR_AIGroup GetLocalPlayerGroup()
	{
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		if (!groupsManager)
			return null;
		
		int playerId = GetGame().GetPlayerController().GetPlayerId();
		
		SCR_AIGroup group = groupsManager.GetPlayerGroup(playerId);
		if(!group)
			return null;
		
		return group;
	}
	
	// Returns a commander radio frequency for given faction
	int GetCommanderRadioFrequency(notnull Faction faction)
	{
		FactionManager fm = GetGame().GetFactionManager();
		
		int factionId = fm.GetFactionIndex(faction);
		return m_iCommanderFrequencies[factionId];
	}
	
	int GetSquadlessRadioFrequency(notnull Faction faction)
	{
		FactionManager fm = GetGame().GetFactionManager();
		
		int factionId = fm.GetFactionIndex(faction);
		return m_iSquadlessFrequencies[factionId];
	}
	
	//------------------------------------------------------------------------
	void RemovePlayerFromGroup(int playerID)
	{
		SCR_AIGroup currentGroup = GetPlayerGroup(playerID);
		
		if (currentGroup)
			currentGroup.RemovePlayer(playerID);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Is called only on the server (authority)
	//! The original method also creates a new group for no reason, we don't need this here
	override void OnPlayerFactionChanged(int playerID, int factionIndex)
	{
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager)
			return;
		
		Faction faction = factionManager.GetFactionByIndex(factionIndex);
		if (!faction)
			return;
		//SCR_AIGroup newPlayerGroup = GetFirstNotFullForFaction(faction);
		//if (!newPlayerGroup)
		//	newPlayerGroup = CreateNewPlayableGroup(faction);
	}
	
	//------------------------------------------------------------------------------------------------
	// Deletes group and removes all players from it
	void DeleteGroupRemovePlayersDelayed(notnull SCR_AIGroup group)
	{
		array<int> players = group.GetPlayerIDs();
		array<int> playersCopy = {};
		playersCopy.Copy(players);
		foreach (int playerId : playersCopy)
		{
			group.RemovePlayer(playerId);
		}
		
		// No need to call this, it has deleted the group after it became empty
		//DeleteGroupDelayed(group);
	}
}