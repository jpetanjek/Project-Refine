typedef func OnAvailabilityChangedDelegate;
void OnAvailabilityChangedDelegate(PR_GroupRoleManagerComponent groupRoleManagerComponent);

[EntityEditorProps(category: "GameScripted/Groups", description: "Player groups role manager, attach to group!.")]
class PR_GroupRoleManagerComponentClass : ScriptComponentClass
{
};

class PR_RoleToPlayer {
	ref array<int> m_aPlayers = {};
	
	//################################################################################################
	//! Codec methods
	//------------------------------------------------------------------------------------------------
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx hint, ScriptBitSerializer packet) 
	{
		int PlayerCount;
		snapshot.SerializeBytes(PlayerCount, 4);
		packet.Serialize(PlayerCount, 32);
		
		int tmp;
		for (int i = 0; i < PlayerCount; i++)
		{
			snapshot.SerializeBytes(tmp, 4);
			packet.Serialize(tmp, 32);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Decode(ScriptBitSerializer packet, ScriptCtx hint, SSnapSerializerBase snapshot) 
	{
		int PlayerCount;
		packet.Serialize(PlayerCount, 32);
		snapshot.SerializeBytes(PlayerCount, 4);
		
		int tmp;
		for (int i = 0; i < PlayerCount; i++)
		{
			packet.Serialize(tmp, 32);
			snapshot.SerializeBytes(tmp, 4);
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx hint) 
	{
		int lCnt;
		lhs.SerializeInt(lCnt);

		int rCnt;
		rhs.SerializeInt(rCnt);

		if (lCnt != rCnt)
			return false;

		int lTmp;
		int rTmp;
		for (int i = 0; i < lCnt; i++)
		{
			lhs.SerializeInt(lTmp);
			rhs.SerializeInt(rTmp);
			if (lTmp != rTmp)
				return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool PropCompare(PR_RoleToPlayer prop, SSnapSerializerBase snapshot, ScriptCtx hint) 
	{
		int playerCount = prop.m_aPlayers.Count();
		if (!snapshot.CompareInt(playerCount))
			return false;

		for (int i = 0; i < playerCount; i++)
		{
			if (!snapshot.CompareInt(prop.m_aPlayers[i]))
				return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Extract(PR_RoleToPlayer prop, ScriptCtx hint, SSnapSerializerBase snapshot) 
	{
		int playerCount = prop.m_aPlayers.Count();
		snapshot.SerializeInt(playerCount);
		for (int i = 0; i < playerCount; i++)
		{
			int id = prop.m_aPlayers[i]; 			
			snapshot.SerializeInt(id);
		}

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx hint, PR_RoleToPlayer prop) 
	{
		int playerCount;
		snapshot.SerializeInt(playerCount);

		int tmp;
		prop.m_aPlayers.Clear();
		for (int i = 0; i < playerCount; i++)
		{
			snapshot.SerializeInt(tmp);
			prop.m_aPlayers.Insert(tmp);
		}

		return true;
	}
	//################################################################################################
	
}

//------------------------------------------------------------------------------------------------
class PR_GroupRoleManagerComponent : ScriptComponent
{
	[RplProp(onRplName: "OnAvailabilityChangedClient")]
	ref array<ref PR_RoleToPlayer> m_aRoleToPlayer = new ref array<ref PR_RoleToPlayer>();
	
	// Defines how many of this role there can be max
	[RplProp(onRplName: "OnAvailabilityChangedClient")]
	ref array<int> m_aRoleAvailabilityCount = {};
	
	//! The fact I have to do this is beyond dumb
	[RplProp(onRplName: "OnAvailabilityChangedClient")]
	int m_iFactionId = -1;
	
	//! The fact I have to do this is beyond dumb 2
	[RplProp(onRplName: "OnAvailabilityChangedClient")]
	int m_iLeaderId = -1;
	
	
	
	//! Local storage of role list so we don't query for it all the time which is stupid
	ref array<PR_Role> m_aRoleList = {};
	
	// Events
	ref ScriptInvokerBase<OnAvailabilityChangedDelegate> m_OnAvailabilityChanged = new ScriptInvokerBase<OnAvailabilityChangedDelegate>();
	
	// Initialize availability - via Game Mode
	void InitializeAvailability(array<int> roleAvailabilityCount)
	{
		m_aRoleAvailabilityCount = roleAvailabilityCount;
		
		// Dumb code start
		SCR_AIGroup group = SCR_AIGroup.Cast(GetOwner());
		SCR_Faction groupFaction = SCR_Faction.Cast(group.GetFaction());
		FactionManager factionManager = GetGame().GetFactionManager();		
		m_iFactionId = factionManager.GetFactionIndex(groupFaction);
		m_iLeaderId = group.GetLeaderID();
		// Dumb code end
		
		// Experiment start
		for(int i=0; i < m_aRoleAvailabilityCount.Count(); i++)
		{
			PR_RoleToPlayer entry = new PR_RoleToPlayer();
			m_aRoleToPlayer.Insert(entry);
		}
		
		PR_RoleList fullRoleList = groupFaction.GetRoleList();
		fullRoleList.GetRoleList(m_aRoleList);
		
		Replication.BumpMe();
		
		OnAvailabilityChangedClient();
	}
	
	override void OnPostInit(IEntity owner)
	{
		if(Replication.IsClient())
			return;
		
		SCR_AIGroup group = SCR_AIGroup.Cast(GetOwner());
		group.GetOnPlayerLeaderChanged().Insert(OnPlayerLeaderChanged);
	}
	
	void OnPlayerLeaderChanged(int groupID, int playerID)
	{
		m_iLeaderId = playerID;
		Replication.BumpMe();
		OnAvailabilityChangedClient();
	}
	
	int GetGroupID()
	{
		return SCR_AIGroup.Cast(GetOwner()).GetGroupID();
	}
	
	// 0 means not available to this group as defined by SL
	// -1 means not available to this group as defined by Commander
	
	//-----------------------------------------------
	// CLAIM LOGIC START
	
	bool ClaimRole(int indexNew)
	{
		
		 
		return true;
	}
	
	bool ReturnRole(int indexOld)
	{
		
		return true;
	}
	
	// CLAIM LOGIC END
	//-----------------------------------------------
	
	
	//-----------------------------------------------
	// UI LOGIC START
	
	void OnAvailabilityChangedClient()
	{		
		if(m_aRoleList.IsEmpty())
		{
			// Dumb code start
			FactionManager factionManager = GetGame().GetFactionManager();		
			SCR_Faction groupFaction = SCR_Faction.Cast(factionManager.GetFactionByIndex(m_iFactionId));
			// Dumb code end
			
			if(!groupFaction)
				return;
			
			PR_RoleList fullRoleList = groupFaction.GetRoleList();
			fullRoleList.GetRoleList(m_aRoleList);
		}
		
		// Useless logic but has to be here since states are a bit broken
		// Will be called twice by availability and claimability
		if(!m_aRoleList.IsEmpty())
		{
			m_OnAvailabilityChanged.Invoke(this);
		}
	}
	
	int GetRoleClaimableCount(int index)
	{
		if(!m_aRoleToPlayer || !m_aRoleToPlayer.IsIndexValid(index))
			return -1;
		
		int availabilityCount = GetRoleAvailableCount(index);
		
		if(availabilityCount == -1)
			return -1;
		
		return availabilityCount - m_aRoleToPlayer[index].m_aPlayers.Count();
	}
	
	int GetRoleAvailableCount(int index)
	{
		if(m_aRoleAvailabilityCount && m_aRoleAvailabilityCount.IsIndexValid(index))
		{
			return m_aRoleAvailabilityCount[index];
		}
		else
		{
			return -1;
		}
	}
		
	PR_Role GetRole(int index)
	{
		if(!m_aRoleList.IsIndexValid(index))
			return null;
		
		return m_aRoleList[index];
	}
	
	//-----------------------------------------------
	// GROUP MEMBER UI LOGIC START
	
	// Skip this one for your own role
	bool CanGroupMemberDrawRole(int index, int playerID)
	{
		// Check if it is at all available
		if(!m_aRoleAvailabilityCount || !m_aRoleAvailabilityCount.IsIndexValid(index) || m_aRoleAvailabilityCount[index] <= 0)
			return false;
		
		// Check if there is any of it left
		int claimableCount = GetRoleClaimableCount(index);
		if((GetRoleClaimableCount(index) < 0 || !m_aRoleToPlayer.IsIndexValid(index)) && !m_aRoleToPlayer[index].m_aPlayers.Contains(playerID))
			return false;
		
		// Check limitations
		// Get Role
		PR_Role role = GetRole(index);
		if(!role)
			return false;
		
		// Check if playerID is leader		
		if(role.m_eRoleLimitation == PR_ERoleLimitation.SQUAD_LEAD_ONLY)
		{
			SCR_AIGroup group = SCR_AIGroup.Cast(GetOwner());
			if(!group)
				return false;
			
			if(!group.IsPlayerLeader(playerID))
				return false;
		} 
		
		return true;
	}
	
	// GROUP MEMBER UI LOGIC END
	//-----------------------------------------------
	
	
	// UI LOGIC END
	//-----------------------------------------------
}