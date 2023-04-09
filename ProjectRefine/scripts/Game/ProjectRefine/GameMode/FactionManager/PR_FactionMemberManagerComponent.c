class  PR_FactionMemberManagerClass : PR_BaseGameModeComponentClass
{
}

typedef func OnPlayerChangedFaction;
void OnPlayerChangedFaction(int playerID, int newFactionIdx);

typedef func OnFactionMembersChanged;
void OnFactionMembersChanged(PR_FactionManager manager);

class PR_FactionMemberManager : PR_BaseGameModeComponent
{
	[RplProp(onRplName: "FactionMembersChanged")]	
	ref array<ref PR_RoleToPlayer> m_aFactionMembers = new ref array<ref PR_RoleToPlayer>();
	
	// Client only - to deduce what new members were added to a faction
	ref array<ref PR_RoleToPlayer> m_aFactionMembersOld = new ref array<ref PR_RoleToPlayer>();
	
	FactionManager m_FactionManager;
	
	//------------------------------------------------------------------------------------------------
	static PR_FactionMemberManager GetInstance()
	{
		BaseGameMode gm = GetGame().GetGameMode();
		if (!gm)
			return null;
		
		return PR_FactionMemberManager.Cast(gm.FindComponent(PR_FactionMemberManager));
	}
	
	//------------------------------------------------------------------------------------------------
	// Events
	// Server and client event
	protected ref ScriptInvokerBase<OnFactionMembersChanged> m_OnFactionMembersChanged = new ScriptInvokerBase<OnFactionMembersChanged>();

	// Server only event
	protected ref ScriptInvokerBase<OnPlayerChangedFaction> m_OnPlayerChangedFaction = new ScriptInvokerBase<OnPlayerChangedFaction>();
	
	ScriptInvokerBase<OnFactionMembersChanged> GetOnFactionMembersChanged()
	{
		return m_OnFactionMembersChanged;
	}
	
	ScriptInvokerBase<OnPlayerChangedFaction> GetOnPlayerChangedFaction()
	{
		return m_OnPlayerChangedFaction;
	}	
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	override protected void EOnInit(IEntity owner)
	{
		m_FactionManager = GetGame().GetFactionManager();
		if(!m_FactionManager)
			return;
		
		for(int i = 0; i < m_FactionManager.GetFactionsCount(); i++)
		{
			PR_RoleToPlayer instance = new PR_RoleToPlayer;
			m_aFactionMembers.Insert(instance);
		}
	}
	
	void ~PR_FactionMemberManager()
	{
		if(m_aFactionMembers)
		{
			m_aFactionMembers.Clear();
			m_aFactionMembers = null;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void FactionMembersChanged()
	{
		m_OnFactionMembersChanged.Invoke(this);
		
		if(Replication.IsClient())
		{
			// Calculate changes
			if(m_aFactionMembersOld.Count())
			{
				for(int i = 0; i < m_aFactionMembersOld.Count(); i++)
				{
					for(int j = 0; j < m_aFactionMembersOld[i].m_aPlayers.Count(); j++)
					{
						int playerID = m_aFactionMembersOld[i].m_aPlayers[j];
						int oldFactionIdx = i;
						int newFactionIdx = -1;
						
						for(int k = 0; k < m_aFactionMembers.Count(); k++)
						{
							if(m_aFactionMembers[k].m_aPlayers.Contains(playerID))
								newFactionIdx = k;
						}
						
						if(newFactionIdx != oldFactionIdx)
						{
							m_OnPlayerChangedFaction.Invoke(playerID, newFactionIdx);
							SCR_BaseGameMode.Cast(GetGame().GetGameMode()).HandleOnFactionAssigned(playerID, m_FactionManager.GetFactionByIndex(newFactionIdx));
						}
					}
				}
			}
			else
			{
				for(int i = 0; i < m_aFactionMembers.Count(); i++)
				{
					for(int j = 0; j < m_aFactionMembers[i].m_aPlayers.Count(); j++)
					{
						int playerID = m_aFactionMembers[i].m_aPlayers[j];
						m_OnPlayerChangedFaction.Invoke(playerID, i);
						SCR_BaseGameMode.Cast(GetGame().GetGameMode()).HandleOnFactionAssigned(playerID, m_FactionManager.GetFactionByIndex(i));
					}
				}
			}
		}
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
			return m_FactionManager.GetFactionByIndex(factionIdx);
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
	
	PR_RoleToPlayer GetFactionMembers(Faction faction)
	{
		int idx = m_FactionManager.GetFactionIndex(faction);
		if(m_aFactionMembers.IsIndexValid(idx))
			return m_aFactionMembers[idx];
		else
			return null;
	}
	
	//------------------------------------------------------------------------------------------------
	// Set players faction
	void SetPlayerFaction(FactionKey factionKey, int playerID)
	{
		SetPlayerFaction(m_FactionManager.GetFactionByKey(factionKey), playerID);
	}
	
	void SetPlayerFaction(Faction faction, int playerID)
	{
		SetPlayerFaction(m_FactionManager.GetFactionIndex(faction), playerID);
	}
	
	// If you want to remove player from his current faction, simply set factionIdx as -1
	void SetPlayerFaction(int factionIdx, int playerID)
	{
		if(Replication.IsClient())
			return;

		int oldFactionIdx = GetPlayerFactionIndex(playerID);
		
		if( oldFactionIdx == factionIdx)
			return;
		
		if( oldFactionIdx != -1)
		{
			int idx = m_aFactionMembers[oldFactionIdx].m_aPlayers.Find(playerID);
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
		}
		
		Replication.BumpMe();
		FactionMembersChanged();
		m_OnPlayerChangedFaction.Invoke(playerID, factionIdx);
	}
}