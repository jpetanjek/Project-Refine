[ComponentEditorProps(category: "GameScripted/GameMode", description: "")]
class PR_ActiveMapIconManagerComponentClass: SCR_BaseGameModeComponentClass
{
};

//------------------------------------------------------------------------------------------------
//! Manages streaming logic of all icons - for now stream everything registered to everyone
class PR_ActiveMapIconManagerComponent: SCR_BaseGameModeComponent
{	
	ref array<PR_ActiveMapIcon> m_AllMarkers = new array<PR_ActiveMapIcon>;
	ref array<PR_ActiveMapIcon> m_NewMarkers = new array<PR_ActiveMapIcon>;
	
	protected static PR_ActiveMapIconManagerComponent s_Instance;
	
	void PR_ActiveMapIconManagerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		s_Instance = this;
	}
	
	void ~PR_ActiveMapIconManagerComponent()
	{
	}
	
	static PR_ActiveMapIconManagerComponent GetInstance()
	{
		return s_Instance;
	}
	
	array<PR_ActiveMapIcon> GetAllMapIcons()
	{
		array<PR_ActiveMapIcon> a = {};
		a.Copy(m_AllMarkers);
		return a;
	}
	
	// Server -> Player changed faction, execute streaming logic - stream in things he needs, stream out things he doesn't
	override void HandleOnFactionAssigned(int playerID, Faction assignedFaction)
	{
		PlayerController localPC =  GetGame().GetPlayerManager().GetPlayerController(playerID);
		if(localPC == null)
			return;
		
		RplIdentity identity = localPC.GetRplIdentity();
		if(!identity.IsValid())
			return;
		
		SCR_RespawnSystemComponent respawnSystem = SCR_RespawnSystemComponent.GetInstance();
		if (respawnSystem == null)
			return;
		
		SCR_PlayerRespawnInfo playerRespawnInfo = respawnSystem.FindPlayerRespawnInfo(localPC.GetPlayerId());
		if(playerRespawnInfo == null)
			return;
		
		if(Replication.IsServer())
		{	
			for(int i = 0; i < m_AllMarkers.Count(); i++)
			{
				PR_ActiveMapIcon localMarker = m_AllMarkers.Get(i);
				
				if(localMarker != null)
				{
					// Order its stream in to this player
					RplComponent rpl = RplComponent.Cast(localMarker.FindComponent(RplComponent));
					if(rpl != null)
					{
						if (playerRespawnInfo.GetPlayerFactionIndex() == localMarker.m_iFactionId)
						{
							rpl.EnableStreamingConNode(identity, false);
						}
						else
						{
							rpl.EnableStreamingConNode(identity, true);
						}
					}
				}
			}
		}
		else 
		{
			// Client
			
			for(int i = 0; i < m_AllMarkers.Count(); i++)
			{
				PR_ActiveMapIcon localMarker = m_AllMarkers.Get(i);
				
				if(localMarker != null)
				{
					localMarker.OnPlayerFactionChanged(playerID, playerRespawnInfo.GetPlayerFactionIndex());
				}
			}
		}
	}
	
	// Called on server when target of the icon changes its faction
	void OnIconTargetFactionChanged(PR_ActiveMapIcon activeMapIcon)
	{
		int index = m_AllMarkers.Find(activeMapIcon);
		if(index == -1)
			return;
		
		SCR_RespawnSystemComponent respawnSystem = SCR_RespawnSystemComponent.GetInstance();
		if(respawnSystem == null)
			return;
		
		array<int> players = {};
		GetGame().GetPlayerManager().GetPlayers(players);
		
		// Order its stream in to every player - based on faction
		RplComponent rpl = RplComponent.Cast(activeMapIcon.FindComponent(RplComponent));
		if(rpl != null)
		{
			for(int j = 0; j < players.Count(); j++)
			{
				PlayerController localPC =  GetGame().GetPlayerManager().GetPlayerController(players[j]);
				if(localPC)
				{
					RplIdentity identity = localPC.GetRplIdentity();
					if(identity.IsValid())
					{
						SCR_PlayerRespawnInfo playerRespawnInfo = respawnSystem.FindPlayerRespawnInfo(localPC.GetPlayerId());
						if (playerRespawnInfo && playerRespawnInfo.GetPlayerFactionIndex() == activeMapIcon.m_iFactionId)
						{
							rpl.EnableStreamingConNode(identity, false);
						}
						else
						{
							rpl.EnableStreamingConNode(identity, true);
						}
					}
				}
			}
		}
	}
	
	PR_ActiveMapIcon ServerRegister(ScriptComponent target,ResourceName m_ActiveMapIconPrefab)
	{	
		PR_ActiveMapIcon activeMapIcon = PR_ActiveMapIcon.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_ActiveMapIconPrefab)));
		if(activeMapIcon != null)
		{
			m_AllMarkers.Insert(activeMapIcon);
			m_NewMarkers.Insert(activeMapIcon);
			
			activeMapIcon.Init(target.GetOwner());

			return activeMapIcon;
		}
		return null;
	}
	
	void ClientRegister(PR_ActiveMapIcon activeMapIcon)
	{
		m_AllMarkers.Insert(activeMapIcon);
		m_NewMarkers.Insert(activeMapIcon);
	}
	
	void Unregister(PR_ActiveMapIcon activeMapIcon)
	{
		int index = m_AllMarkers.Find(activeMapIcon);
		if(index != -1)
			m_AllMarkers.Remove(index);
		
		index = m_NewMarkers.Find(activeMapIcon);
		if(index != -1)
			m_NewMarkers.Remove(index);
		
		if(Replication.IsServer())
		{
			delete activeMapIcon;
		}
	}
	
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(GetOwner(), EntityEvent.FIXEDFRAME);
	}
	
	override void EOnFixedFrame(IEntity owner, float timeSlice)
	{
		// TODO: Only runs on master/server
		StreamingLogic();
		if(m_NewMarkers != null)
			m_NewMarkers.Clear();
	}
	
	void StreamingLogic()
	{
		SCR_RespawnSystemComponent respawnSystem = SCR_RespawnSystemComponent.GetInstance();
		if(respawnSystem == null)
			return;
		
		if(m_NewMarkers.IsEmpty())
			return;
		
		// Stream new ones in to everyone
		array<int> indiciesToRemove = {};
		
		array<int> players = {};
		GetGame().GetPlayerManager().GetPlayers(players);
		
		for(int i = 0; i < m_NewMarkers.Count(); i++)
		{
			if(m_NewMarkers.Get(i) != null)
			{
				PR_ActiveMapIcon localMarker = m_NewMarkers.Get(i);
				
				// Order its stream in to every player - based on faction
				RplComponent rpl = RplComponent.Cast(m_NewMarkers.Get(i).FindComponent(RplComponent));
				if(rpl != null)
				{
					for(int j = 0; j < players.Count(); j++)
					{
						PlayerController localPC =  GetGame().GetPlayerManager().GetPlayerController(players[j]);
						if(localPC)
						{
							RplIdentity identity = localPC.GetRplIdentity();
							if(identity.IsValid())
							{
								SCR_PlayerRespawnInfo playerRespawnInfo = respawnSystem.FindPlayerRespawnInfo(localPC.GetPlayerId());
								if (playerRespawnInfo && playerRespawnInfo.GetPlayerFactionIndex() == localMarker.m_iFactionId)
								{
									rpl.EnableStreamingConNode(identity, false);
								}
							}
						}
					}
				}
			}
			else
			{
				// TODO cleanup this logic
				indiciesToRemove.Insert(i);
			}
		}
		
		for(int j = 0; j < indiciesToRemove.Count(); j++)
		{
			m_NewMarkers.Remove(indiciesToRemove[j]);
		}
	}
	
	
	override void OnPlayerConnected(int playerId)
	{
		SCR_RespawnSystemComponent respawnSystem = SCR_RespawnSystemComponent.GetInstance();
		if(respawnSystem == null)
			return;
		
		if(m_AllMarkers == null)
			return;
		
		array<int> indiciesToRemove = {};
		
		for(int i = 0; i < m_AllMarkers.Count(); i++)
		{
			if(m_AllMarkers.Get(i) != null)
			{
				PR_ActiveMapIcon localMarker = m_AllMarkers.Get(i);
				
				// Order its stream in to this player
				RplComponent rpl = RplComponent.Cast(m_AllMarkers.Get(i).FindComponent(RplComponent));
				if(rpl != null)
				{
					PlayerController localPC =  GetGame().GetPlayerManager().GetPlayerController(playerId);
					if(localPC)
					{
						RplIdentity identity = localPC.GetRplIdentity();
						if(identity.IsValid())
						{
							SCR_PlayerRespawnInfo playerRespawnInfo = respawnSystem.FindPlayerRespawnInfo(localPC.GetPlayerId());
							if (playerRespawnInfo && playerRespawnInfo.GetPlayerFactionIndex() == localMarker.m_iFactionId)
							{
								rpl.EnableStreamingConNode(identity, false);
							}
						}
					}
				}
			}
			else
			{
				indiciesToRemove.Insert(i);
			}
		}
		
		for(int j = 0; j < indiciesToRemove.Count(); j++)
		{
			m_AllMarkers.Remove(indiciesToRemove[j]);
		}
	}
	
	// Public inteface to add a map marker
	PR_ActiveMapIconMarker AddMapMarker(int fromPlayerId, int fromPlayerFactionId, vector markerPosWorld, string markerText, string markerIconName, int markerColor)
	{
		//PrintFormat("Player %1 requests to add marker: %2 %3", fromPlayerId, markerPosWorld, markerText);
		
		// Spawn a prefab with map marker
		EntitySpawnParams p = new EntitySpawnParams();
		p.Transform[3] = markerPosWorld;
		Resource rsc = Resource.Load("{6EF387F31DB53667}Prefabs/Map/MapMarkerBase.et");
		PR_ActiveMapIconMarker marker = PR_ActiveMapIconMarker.Cast(GetGame().SpawnEntityPrefab(rsc));
		marker.Init(null, pos: markerPosWorld, factionId : fromPlayerFactionId);
		
		PlayerManager pm = GetGame().GetPlayerManager();
		string playerName = pm.GetPlayerName(fromPlayerId);
		
		marker.InitMarkerProps(markerText, markerIconName, markerColor, playerName);
		
		m_NewMarkers.Insert(marker);
		m_AllMarkers.Insert(marker);
		
		return marker;
	}
	
	// Public interface to delete a map marker
	void DeleteMapMarker(PR_ActiveMapIconMarker marker)
	{
		if (!marker)
			return;
		
		int index = m_NewMarkers.Find(marker);
		if(index != -1)
			m_NewMarkers.Remove(index);
		
		index = m_AllMarkers.Find(marker);
		if(index != -1)
			m_AllMarkers.Remove(index);
		
		RplComponent.DeleteRplEntity(marker, false);
	}
};
