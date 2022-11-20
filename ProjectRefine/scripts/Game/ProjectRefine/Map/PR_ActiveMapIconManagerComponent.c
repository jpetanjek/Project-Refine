[ComponentEditorProps(category: "GameScripted/GameMode", description: "")]
class PR_ActiveMapIconManagerComponentClass: SCR_BaseGameModeComponentClass
{
};

//------------------------------------------------------------------------------------------------
//! Manages streaming logic of all icons - for now stream everything registered to everyone
class PR_ActiveMapIconManagerComponent: SCR_BaseGameModeComponent
{	
	private ref map<ScriptComponent, PR_ActiveMapIcon> m_AllRegistered = new map<ScriptComponent, PR_ActiveMapIcon>;
	private ref map<ScriptComponent, PR_ActiveMapIcon> m_NewlyRegistered = new map<ScriptComponent, PR_ActiveMapIcon>;
	
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
	
	void Register(ScriptComponent target,ResourceName m_ActiveMapIconPrefab)
	{
		if(target != null)
		{
			Print(m_ActiveMapIconPrefab);
			//PR_ActiveMapIcon activeMapIcon = PR_ActiveMapIcon.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_ActiveMapIconPrefab), GetOwner().GetWorld()));
			PR_ActiveMapIcon activeMapIcon = PR_ActiveMapIcon.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_ActiveMapIconPrefab)));
			if(activeMapIcon != null)
			{
				m_NewlyRegistered.Insert(target, activeMapIcon);
				m_AllRegistered.Insert(target, activeMapIcon);
				
				activeMapIcon.Init(target.GetOwner());
			}
		}
	}
	
	void Unregister(ScriptComponent target)
	{
		ref PR_ActiveMapIcon activeMapIcon = m_AllRegistered.Get(target);
		m_AllRegistered.Remove(target);
		m_NewlyRegistered.Remove(target);
		
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
		if(m_NewlyRegistered != null)
			m_NewlyRegistered.Clear();
	}
	
	void StreamingLogic()
	{
		if(m_NewlyRegistered == null)
			return;
		
		// Stream new ones in to everyone
		array<int> indiciesToRemove = {};
		
		for(int i = 0; i < m_NewlyRegistered.Count(); i++)
		{
			if(m_NewlyRegistered.GetElement(i) != null)
			{
				// Order its stream in to every player
				RplComponent rpl = RplComponent.Cast(m_NewlyRegistered.GetElement(i).FindComponent(RplComponent));
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
			m_NewlyRegistered.RemoveElement(indiciesToRemove[j]);
		}
	}
	
	
	override void OnPlayerConnected(int playerId)
	{
		if(m_AllRegistered == null)
			return;
		
		array<int> indiciesToRemove = {};
		
		for(int i = 0; i < m_AllRegistered.Count(); i++)
		{
			if(m_AllRegistered.GetElement(i) != null)
			{
				// Order its stream in to this player
				RplComponent rpl = RplComponent.Cast(m_AllRegistered.GetElement(i).FindComponent(RplComponent));
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
			m_AllRegistered.RemoveElement(indiciesToRemove[j]);
		}
	}
	
	// Public inteface to add a map marker
	void AddMapMarker(vector markerPosWorld, string markerText, int fromPlayerId)
	{
		PrintFormat("Player %1 requests to add marker: %2 %3", fromPlayerId, markerPosWorld, markerText);
		
		// Spawn a prefab with map marker
		EntitySpawnParams p = new EntitySpawnParams();
		p.Transform[3] = markerPosWorld;
		Resource rsc = Resource.Load("{6EF387F31DB53667}Prefabs/Map/MapMarkerBase.et");
		PR_ActiveMapIcon mapIconEntity = PR_ActiveMapIcon.Cast(GetGame().SpawnEntityPrefab(rsc));
		mapIconEntity.Init(null, pos: markerPosWorld);
	}
};
