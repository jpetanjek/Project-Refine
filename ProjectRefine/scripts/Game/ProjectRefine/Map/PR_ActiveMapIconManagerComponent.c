[ComponentEditorProps(category: "GameScripted/GameMode", description: "")]
class PR_ActiveMapIconManagerComponentClass: SCR_BaseGameModeComponentClass
{
};

//------------------------------------------------------------------------------------------------
//! Manages streaming logic of all icons - for now stream everything registered to everyone
class PR_ActiveMapIconManagerComponent: SCR_BaseGameModeComponent
{	
	static ref array<PR_ActiveMapIcon> m_AllMarkers = new array<PR_ActiveMapIcon>;
	static ref array<PR_ActiveMapIcon> m_NewMarkers = new array<PR_ActiveMapIcon>;
	
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
	
	override void HandleOnFactionAssigned(int playerID, Faction assignedFaction)
	{
		// TODO remove this, its not needed
		
		if(Replication.IsClient())
		{
			Print("Client PR_ActiveMapIconManagerComponent::HandleOnFactionAssigned"); 
			SCR_CampaignFaction faction = SCR_CampaignFaction.Cast(SCR_RespawnSystemComponent.GetLocalPlayerFaction());
			if (!faction)
				return;
		}
	}
	
	PR_ActiveMapIcon Register(ScriptComponent target,ResourceName m_ActiveMapIconPrefab)
	{
		if(target != null)
		{
			Print(m_ActiveMapIconPrefab);
			//PR_ActiveMapIcon activeMapIcon = PR_ActiveMapIcon.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_ActiveMapIconPrefab), GetOwner().GetWorld()));
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
		return null;
	}
	
	void Unregister(PR_ActiveMapIcon activeMapIcon)
	{
		int index = m_AllMarkers.Find(activeMapIcon);
		if(index != -1)
			m_AllMarkers.Remove(index);
		
		index = m_NewMarkers.Find(activeMapIcon);
		if(index != -1)
			m_NewMarkers.Remove(index);
		
		delete activeMapIcon;
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
		if(m_NewMarkers.IsEmpty())
			return;
		
		// Stream new ones in to everyone
		array<int> indiciesToRemove = {};
		
		for(int i = 0; i < m_NewMarkers.Count(); i++)
		{
			if(m_NewMarkers.Get(i) != null)
			{
				// Order its stream in to every player
				RplComponent rpl = RplComponent.Cast(m_NewMarkers.Get(i).FindComponent(RplComponent));
				if(rpl != null)
				{
					array<int> players = {};
					GetGame().GetPlayerManager().GetPlayers(players);
					
					for(int j = 0; j < players.Count(); j++)
					{
						RplIdentity identity = GetGame().GetPlayerManager().GetPlayerController(players[j]).GetRplIdentity();
						if(identity.IsValid())
						{
							rpl.EnableStreamingConNode(identity, false);
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
			m_NewMarkers.Remove(indiciesToRemove[j]);
		}
	}
	
	
	override void OnPlayerConnected(int playerId)
	{
		if(m_AllMarkers == null)
			return;
		
		array<int> indiciesToRemove = {};
		
		for(int i = 0; i < m_AllMarkers.Count(); i++)
		{
			if(m_AllMarkers.Get(i) != null)
			{
				// Order its stream in to this player
				RplComponent rpl = RplComponent.Cast(m_AllMarkers.Get(i).FindComponent(RplComponent));
				if(rpl != null)
				{
					RplIdentity identity = GetGame().GetPlayerManager().GetPlayerController(playerId).GetRplIdentity();
					if(identity.IsValid())
					{
						rpl.EnableStreamingConNode(identity, false);
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
	PR_ActiveMapIconMarker AddMapMarker(int fromPlayerId, vector markerPosWorld, string markerText, string markerIconName, int markerColor)
	{
		//PrintFormat("Player %1 requests to add marker: %2 %3", fromPlayerId, markerPosWorld, markerText);
		
		// Spawn a prefab with map marker
		EntitySpawnParams p = new EntitySpawnParams();
		p.Transform[3] = markerPosWorld;
		Resource rsc = Resource.Load("{6EF387F31DB53667}Prefabs/Map/MapMarkerBase.et");
		PR_ActiveMapIconMarker marker = PR_ActiveMapIconMarker.Cast(GetGame().SpawnEntityPrefab(rsc));
		marker.Init(null, pos: markerPosWorld);
		
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
