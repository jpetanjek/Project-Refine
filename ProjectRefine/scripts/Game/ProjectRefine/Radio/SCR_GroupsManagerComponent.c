modded class SCR_GroupsManagerComponent
{
	[Attribute(desc: "Array of frequencies for commander channel. One frequency per faction.")]
	ref array<int> m_iCommanderFrequencies;
	
	[Attribute(desc: "Array of frequencies for squadless players. One frequency per faction.")]
	ref array<int> m_iSquadlessFrequencies;
	
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
}