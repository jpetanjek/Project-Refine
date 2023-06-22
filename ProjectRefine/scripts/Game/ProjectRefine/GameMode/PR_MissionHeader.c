class PR_MissionHeader : SCR_MissionHeader
{
	[Attribute("1", UIWidgets.CheckBox, "Factions will be shuffled with random chance")]
	bool m_bRefineRandomizeFactions;
	
	[Attribute("", UIWidgets.EditBox, "Faction key from Faction Manager entity. Attacker faction if faction randomization is disabled.")]
	string m_sRefineFaction_0;
	
	[Attribute("", UIWidgets.EditBox, "Faction key from Faction Manager entity. Defender faction if faction randomization is disabled.")]
	string m_sRefineFaction_1;
	
	[Attribute("0", UIWidgets.ComboBox, desc: "Archetype of Game Mode", enums: ParamEnumArray.FromEnum(PR_EGameModeArchetype))]
	PR_EGameModeArchetype m_eRefineGameModeArchetype;
	
	[Attribute("0", UIWidgets.CheckBox, desc: "Used for testing. When true, game mode will end 5 seconds after start")]
	bool m_bRefineTestEndGame;
	
	//[Attribute("300", UIWidgets.EditBox, "Duration of preparation phase in seconds")]
	//int m_iRefinePreparationPhaseDuration_s;
}