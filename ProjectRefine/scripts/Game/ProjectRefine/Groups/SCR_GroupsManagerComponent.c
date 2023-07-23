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
	override void OnPlayerFactionChanged(notnull FactionAffiliationComponent owner, Faction previousFaction, Faction newFaction)
	{
		if (!newFaction)
			return;
		
		SCR_Faction scrFaction = SCR_Faction.Cast(newFaction);
		if (!scrFaction)
			return;
		/*
		if (scrFaction.GetCanCreateOnlyPredefinedGroups())
			return;

		SCR_AIGroup newPlayerGroup = GetFirstNotFullForFaction(newFaction);
		if (!newPlayerGroup)
			newPlayerGroup = CreateNewPlayableGroup(newFaction);
		
		//group creation can fail
		if (!newPlayerGroup || !owner)
			return;
		
		*/
		PlayerController controller = PlayerController.Cast(owner.GetOwner());
		if (!controller)
			return;
		
		SCR_PlayerControllerGroupComponent groupComp = SCR_PlayerControllerGroupComponent.Cast(controller.FindComponent(SCR_PlayerControllerGroupComponent));
		if (!groupComp)
			return;

		SCR_AIGroup oldGroup = FindGroup(groupComp.GetGroupID());
		if (!oldGroup)
			return;

		oldGroup.RemovePlayer(controller.GetPlayerId());
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