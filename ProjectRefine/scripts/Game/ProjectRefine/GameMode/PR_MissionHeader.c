class PR_MissionHeader : SCR_MissionHeader
{
	[Attribute("1", UIWidgets.CheckBox, "Factions will be shuffled with random chance")]
	bool m_bRefineRandomizeFactions;
	
	[Attribute("", UIWidgets.EditBox, "Faction key from Faction Manager entity. Attacker faction if faction randomization is disabled.")]
	string m_sRefineFaction_0;
	
	[Attribute("", UIWidgets.EditBox, "Faction key from Faction Manager entity. Defender faction if faction randomization is disabled.")]
	string m_sRefineFaction_1;
	
	//[Attribute("300", UIWidgets.EditBox, "Duration of preparation phase in seconds")]
	//int m_iRefinePreparationPhaseDuration_s;
}