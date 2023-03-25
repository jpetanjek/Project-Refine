//------------------------------------------------------------------------------------------------
class PR_RespawnSystemComponentClass: RespawnSystemComponentClass
{
};

//! Scripted implementation that handles spawning and respawning of players.
//! Should be attached to a GameMode entity.
class PR_RespawnSystemComponent: RespawnSystemComponent
{
	//------------------------------------------------------------------------------------------------
	// Data
	
	// Instance of this component
	private static PR_RespawnSystemComponent s_Instance = null;
	
	//------------------------------------------------------------------------------------------------
	// Pointers
	
	// The parent of this entity which should be a gamemode
	protected SCR_BaseGameMode m_pGameMode;
	// Parent entity's rpl component
	protected RplComponent m_pRplComponent;
	
	//------------------------------------------------------------------------------------------------
	// Static functions
	
	//------------------------------------------------------------------------------------------------
	//! Returns an instance of RespawnSystemComponent
	static PR_RespawnSystemComponent GetInstance()
	{
		if (!s_Instance)
		{
			BaseGameMode pGameMode = GetGame().GetGameMode();
			if (pGameMode)
				s_Instance = PR_RespawnSystemComponent.Cast(pGameMode.FindComponent(PR_RespawnSystemComponent));
		}

		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
	// UI Logic
	
	// Open/close menu, is it open etc.

};