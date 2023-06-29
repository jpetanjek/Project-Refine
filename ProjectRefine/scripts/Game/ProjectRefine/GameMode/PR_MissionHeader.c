class PR_MissionHeader : SCR_MissionHeader
{	
	[Attribute("0", UIWidgets.ComboBox, desc: "Archetype of Game Mode", enums: ParamEnumArray.FromEnum(PR_EGameModeArchetype))]
	PR_EGameModeArchetype m_eRefineGameModeArchetype;
	
	[Attribute("1", UIWidgets.ComboBox, desc: "When true, players without a group will be automatically kicked after some time.")]
	bool m_bRefineKickIfNoGroup;
	
	[Attribute("0", UIWidgets.CheckBox, desc: "When true, server process will be terminated when game mode ends.")]
	bool m_bRefineTerminateServerOnGameEnd;
	
	[Attribute("0", UIWidgets.CheckBox, desc: "Used for testing. When true, game mode will end 5 seconds after start")]
	bool m_bRefineTestEndGame;
	
	//[Attribute("300", UIWidgets.EditBox, "Duration of preparation phase in seconds")]
	//int m_iRefinePreparationPhaseDuration_s;
	
	[Attribute("0", UIWidgets.CheckBox)]
	bool m_bRefineRandomFactionArrays;
	
	//[Attribute("0", UIWidgets.CheckBox)]
	//bool m_bRefineFinalShuffleFactions;
	
	[Attribute()]
	ref array<string> m_aRefineFactions_0;
	
	[Attribute()]
	ref array<string> m_aRefineFactions_1;
	
	[Attribute()]
	ref array<string> m_aRefineFactions_2;
	
	[Attribute()]
	ref array<string> m_aRefineFactions_3;
}