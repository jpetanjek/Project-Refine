//------------------------------------------------------------------------------------------------
class PR_FactionManagerClass: SCR_FactionManagerClass
{
};

typedef func OnPlayerChangedFaction;
void OnPlayerChangedFaction(int playerID, int newFactionIdx);

typedef func OnFactionMembersChanged;
void OnFactionMembersChanged(PR_FactionManager manager);

//------------------------------------------------------------------------------------------------
class PR_FactionManager : SCR_FactionManager
{
	[RplProp(onRplName: "FactionMembersChanged")]	
	ref array<ref PR_RoleToPlayer> m_aFactionMembers = new ref array<ref PR_RoleToPlayer>();
	
	// Server and client event
	protected ref ScriptInvokerBase<OnFactionMembersChanged> m_OnFactionMembersChanged = new ScriptInvokerBase<OnFactionMembersChanged>();

	// Server only event
	protected ref ScriptInvokerBase<OnPlayerChangedFaction> m_OnPlayerChangedFaction = new ScriptInvokerBase<OnPlayerChangedFaction>();


	//------------------------------------------------------------------------------------------------
	// Set players faction
	void SetPlayerFaction(FactionKey factionKey, int playerID)
	{
		SetPlayerFaction(GetFactionByKey(factionKey), playerID);
	}
	
	void SetPlayerFaction(Faction faction, int playerID)
	{
		SetPlayerFaction(GetFactionIndex(faction), playerID);
	}
	
	// If you want to remove player from his current faction, simply set factionIdx as -1
	void SetPlayerFaction(int factionIdx, int playerID)
	{
		if(Replication.IsClient())
			return;
		
		int oldFactionIdx = GetPlayerFactionIndex(playerID);
		
		if( oldFactionIdx != -1)
		{
			int idx = m_aFactionMembers[oldFactionIdx].m_aPlayers.Find(oldFactionIdx);
			if(idx != -1)
			{
				m_aFactionMembers[oldFactionIdx].m_aPlayers.Remove(idx);
			}
		}
		
		if( factionIdx != -1)
		{
			if(m_aFactionMembers.IsIndexValid(factionIdx))
			{
				m_aFactionMembers[factionIdx].m_aPlayers.Insert(playerID);
			}
			else
			{
				PR_RoleToPlayer factionPlayerInit = new PR_RoleToPlayer();
				factionPlayerInit.m_aPlayers.Insert(playerID);
				m_aFactionMembers.Insert(factionPlayerInit);
			}
		}
		
		Replication.BumpMe();
		m_OnPlayerChangedFaction.Invoke(playerID, factionIdx);
		FactionMembersChanged();
	}
	
	//------------------------------------------------------------------------------------------------
	// Get players faction
	// If you want local players faction use SCR_PlayerController.GetLocalPlayerId()
	
	int GetPlayerFactionIndex(int playerID)
	{
		for(int i = 0; i < m_aFactionMembers.Count(); i++)
		{
			if( m_aFactionMembers[i].m_aPlayers.Contains(playerID))
			{
				return i;
			}
		}
		
		return -1;
	}
	
	Faction GetPlayerFaction(int playerID)
	{
		int factionIdx = GetPlayerFactionIndex(playerID);
		if( factionIdx != -1)
		{
			return GetFactionByIndex(factionIdx);
		}
		else
		{
			return null;
		}		
	}
	
	FactionKey GetPlayerFactionKey(int playerID)
	{
		Faction faction = GetPlayerFaction(playerID);
		if( faction )
		{
			return faction.GetFactionKey();
		}
		else
		{
			return "";
		}
	}
	
	//------------------------------------------------------------------------------------------------
	// Events
	void FactionMembersChanged()
	{
		m_OnFactionMembersChanged.Invoke(this);
	}	
	
	ScriptInvokerBase<OnFactionMembersChanged> GetOnFactionMembersChanged()
	{
		return m_OnFactionMembersChanged;
	}
	
	ScriptInvokerBase<OnPlayerChangedFaction> GetOnPlayerChangedFaction()
	{
		return m_OnPlayerChangedFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	void ~PR_FactionManager()
	{
		if(m_aFactionMembers)
		{
			m_aFactionMembers.Clear();
			m_aFactionMembers = null;
		}
	}
};