[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class PR_AssetInformerComponentClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
/*!
	Component storing some data about asset.
*/
class PR_AssetInformerComponent : ScriptComponent
{
	protected PR_EAssetType m_AssetType = -1;
	
	protected int m_iInitialFactionId = -1;
	
	// Previously assigned startup chance - so it can be assigned when game mode enters play
	protected float m_fPreviousStartupChance = 0;

	void Init(PR_EAssetType assetType, int factionId)
	{
		m_AssetType = assetType;
		m_iInitialFactionId = factionId;
		
		// Post spawn actions - 0 startup chance if we are in preparation phase
		{
			VehicleControllerComponent controller;
			controller = VehicleControllerComponent.Cast(GetOwner().FindComponent(VehicleControllerComponent));
			
			PR_GameMode gameMode = PR_GameMode.Cast(GetGame().GetGameMode());
			
			if(controller && gameMode && gameMode.GetGameModeStage() == PR_EGameModeStage.PREPARATION && m_iInitialFactionId == gameMode.GetInvadingFactionId())
			{
				gameMode.m_OnGameModeStageChanged.Insert(OnGameModeStageChanged);			
				m_fPreviousStartupChance = controller.GetEngineStartupChance();
				controller.SetEngineStartupChance(0);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void OnGameModeStageChanged(PR_EGameModeStage stage)
	{
		switch (stage)
		{
			case PR_EGameModeStage.PREPARATION:
			{
				break;
			}
			case PR_EGameModeStage.LIVE:
			{
				// Assign previous startup chance
				VehicleControllerComponent controller;
				controller = VehicleControllerComponent.Cast(GetOwner().FindComponent(VehicleControllerComponent));
				
				if(controller)
				{
					controller.SetEngineStartupChance(m_fPreviousStartupChance);
				}
				break;
			}
			case PR_EGameModeStage.DEBRIEF:
			{
				break;
			}
		}
	}
	
	PR_EAssetType GetAssetType() { return m_AssetType; }
	
	int GetInitialFactionId() { return m_iInitialFactionId; }
		
	/*
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
	}

	//------------------------------------------------------------------------------------------------
	void PR_AssetInfoComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~PR_AssetInfoComponent()
	{
	}
	*/
};
