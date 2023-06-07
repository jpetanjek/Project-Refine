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
	ref array<PR_ActiveMapIconInformerComponent> m_RegistrationQueue = new array<PR_ActiveMapIconInformerComponent>();
	
	protected static PR_ActiveMapIconManagerComponent s_Instance;
	
	protected SCR_EditorManagerEntity m_EditorManager;
	
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
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if(Replication.IsServer())
			SetEventMask(GetOwner(), EntityEvent.FRAME);
		
		SCR_EditorManagerCore editorManagerCore = SCR_EditorManagerCore.Cast(SCR_EditorManagerCore.GetInstance(SCR_EditorManagerCore));
		if (editorManagerCore)
		{
			editorManagerCore.Event_OnEditorManagerInitOwner.Insert(OnEditorManagerInitOwner);
			// GameMaster logic - per client
			if(Replication.IsServer())
				editorManagerCore.Event_OnEditorManagerCreatedServer.Insert(OnEditorManagerInitOnServerForPlayer);
		}
	}
	
	protected void OnEditorManagerInitOwner(SCR_EditorManagerEntity editorManager)
	{
		if(editorManager)
		{
			m_EditorManager = editorManager;
			m_EditorManager.GetOnModeChange().Insert(OnEditorModeChangeOwner);
		}
	}
	
	protected void OnEditorModeChangeOwner(SCR_EditorModeEntity newModeEntity, SCR_EditorModeEntity oldModeEntity)
	{
		// Reconsider visibility of all icons	
		for(int i = 0; i < m_AllMarkers.Count(); i++)
		{
			PR_ActiveMapIcon localMarker = m_AllMarkers.Get(i);
			
			if(localMarker != null)
			{
				localMarker.UpdateVisibility();
			}
		}
	}
	
	protected void OnEditorManagerInitOnServerForPlayer(SCR_EditorManagerEntity editorManager)
	{
		if(editorManager)
		{
			editorManager.GetOnModeAdd().Insert(OnEditorModeChangeOnServerForPlayer);
			editorManager.GetOnModeRemove().Insert(OnEditorModeChangeOnServerForPlayer);
		}
	}
	
	protected void OnEditorModeChangeOnServerForPlayer(SCR_EditorModeEntity modeEntity)
	{
		SCR_EditorManagerEntity editorManager = SCR_EditorManagerEntity.Cast(modeEntity.GetParent());
		if(!editorManager)
			return;
		
		PlayerController localPC =  GetGame().GetPlayerManager().GetPlayerController(editorManager.GetPlayerID());
		if(localPC == null)
			return;
		
		RplIdentity identity = localPC.GetRplIdentity();
		if(!identity.IsValid())
			return;
		
		// Reconsider streamability of all icons
		for(int i = 0; i < m_AllMarkers.Count(); i++)
		{
			PR_ActiveMapIcon localMarker = m_AllMarkers.Get(i);
			
			if(localMarker != null)
			{
				RplComponent rpl = RplComponent.Cast(localMarker.FindComponent(RplComponent));
				if(rpl != null)
				{	
					if (CanStream(localPC.GetPlayerId(), localMarker))
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
	
	bool CanStream(int playerId, PR_ActiveMapIcon icon)
	{
		SCR_RespawnSystemComponent respawnSystem = SCR_RespawnSystemComponent.GetInstance();
		if (respawnSystem == null)
			return false;
		
		FactionManager factionManager = GetGame().GetFactionManager();
		if (factionManager == null)
			return false;
		
		int playerFaction = factionManager.GetFactionIndex(respawnSystem.GetPlayerFaction(playerId));
		
		return playerFaction == icon.m_iFactionId || icon.m_iFactionId == -1 || (m_EditorManager && m_EditorManager.HasMode(EEditorMode.EDIT));
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
						if (CanStream(playerID, localMarker))
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
			FactionManager factionManager = GetGame().GetFactionManager();
			if (factionManager == null)
				return;
		
			int playerFaction = factionManager.GetFactionIndex(respawnSystem.GetPlayerFaction(localPC.GetPlayerId()));
			
			for(int i = 0; i < m_AllMarkers.Count(); i++)
			{
				PR_ActiveMapIcon localMarker = m_AllMarkers.Get(i);
				
				if(localMarker != null)
				{
					localMarker.OnPlayerFactionChanged(playerID, playerFaction);
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
						if (CanStream(localPC.GetPlayerId(), activeMapIcon))
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
	
	void ServerRegisterAsync(PR_ActiveMapIconInformerComponent target)
	{
		m_RegistrationQueue.Insert(target);
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
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		// Register icons asynchronously
		for(int i = m_RegistrationQueue.Count(); i > 0; i--)
		{
			PR_ActiveMapIconInformerComponent target = m_RegistrationQueue[i-1];
			
			if (target)
			{
				ResourceName activeMapIconPrefab = target.GetIconPrefab();
			
				PR_ActiveMapIcon activeMapIcon = PR_ActiveMapIcon.Cast(GetGame().SpawnEntityPrefab(Resource.Load(activeMapIconPrefab)));
				if(activeMapIcon != null)
				{
					m_AllMarkers.Insert(activeMapIcon);
					m_NewMarkers.Insert(activeMapIcon);
					
					activeMapIcon.Init(target.GetOwner());
					
					target.InitCompletion(activeMapIcon);
				}
			}
			
			m_RegistrationQueue.RemoveOrdered(i-1);
		}
		
		if(m_NewMarkers != null && !m_NewMarkers.IsEmpty())
		{
			StreamingLogic();
			m_NewMarkers.Clear();
		}
	}
	
	void StreamingLogic()
	{
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
								if (CanStream(localPC.GetPlayerId(), localMarker))
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
		// Server side streaming calls
		if(Replication.IsClient())
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
							if (CanStream(localPC.GetPlayerId(), localMarker))
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
