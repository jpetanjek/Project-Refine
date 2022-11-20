[ComponentEditorProps(category: "GameScripted/GameMode", description: "")]
class PR_ActiveMapIconManagerComponentClass: SCR_BaseGameModeComponentClass
{
};

//------------------------------------------------------------------------------------------------
//! Manages streaming logic of all icons - for now stream everything registered to everyone
class PR_ActiveMapIconManagerComponent: SCR_BaseGameModeComponent
{	
	private ref array<PR_ActiveMapIcon> m_AllRegisteredEntities = new array<PR_ActiveMapIcon>;
	private ref array<PR_ActiveMapIcon> m_NewlyRegisteredEntities = new array<PR_ActiveMapIcon>;
	// TODO: Maybe map<IEntity, icon> and then unregister rather than nullchecking
	
	protected static PR_ActiveMapIconManagerComponent s_Instance;
	
	void PR_ActiveMapIconManagerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		s_Instance = this;
	}
	
	void ~PR_ActiveMapIconManagerComponent()
	{
		delete m_AllRegisteredEntities;
		delete m_NewlyRegisteredEntities;
	}
	
	static PR_ActiveMapIconManagerComponent GetInstance()
	{
		return s_Instance;
	}
	
	void Register(IEntity target,ResourceName m_ActiveMapIconPrefab)
	{
		if(target != null)
		{
			Print(m_ActiveMapIconPrefab);
			//PR_ActiveMapIcon activeMapIcon = PR_ActiveMapIcon.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_ActiveMapIconPrefab), GetOwner().GetWorld()));
			PR_ActiveMapIcon activeMapIcon = PR_ActiveMapIcon.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_ActiveMapIconPrefab)));
			if(activeMapIcon != null)
			{
				m_AllRegisteredEntities.Insert(activeMapIcon);
				m_NewlyRegisteredEntities.Insert(activeMapIcon);
				activeMapIcon.Init(target);
			}
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
		if(m_NewlyRegisteredEntities != null)
			m_NewlyRegisteredEntities.Clear();
	}
	
	void StreamingLogic()
	{
		if(m_NewlyRegisteredEntities == null)
			return;
		
		// Stream new ones in to everyone
		array<int> indiciesToRemove = {};
		
		for(int i = 0; i < m_NewlyRegisteredEntities.Count(); i++)
		{
			if(m_NewlyRegisteredEntities[i] != null)
			{
				// Order its stream in to every player
				RplComponent rpl = RplComponent.Cast(m_NewlyRegisteredEntities[i].FindComponent(RplComponent));
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
			m_NewlyRegisteredEntities.Remove(indiciesToRemove[j]);
		}
	}
	
	
	override void OnPlayerConnected(int playerId)
	{
		if(m_AllRegisteredEntities == null)
			return;
		
		array<int> indiciesToRemove = {};
		
		for(int i = 0; i < m_AllRegisteredEntities.Count(); i++)
		{
			if(m_AllRegisteredEntities[i] != null)
			{
				// Order its stream in to this player
				RplComponent rpl = RplComponent.Cast(m_AllRegisteredEntities[i].FindComponent(RplComponent));
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
			m_AllRegisteredEntities.Remove(indiciesToRemove[j]);
		}
	}
	
	// Public inteface to add a map marker
	void AddMapMarker(int fromPlayerId, vector markerPosWorld, string markerText, string markerIconName)
	{
		PrintFormat("Player %1 requests to add marker: %2 %3", fromPlayerId, markerPosWorld, markerText);
		
		// Spawn a prefab with map marker
		EntitySpawnParams p = new EntitySpawnParams();
		p.Transform[3] = markerPosWorld;
		Resource rsc = Resource.Load("{6EF387F31DB53667}Prefabs/Map/MapMarkerBase.et");
		PR_ActiveMapIconMarker marker = PR_ActiveMapIconMarker.Cast(GetGame().SpawnEntityPrefab(rsc));
		marker.Init(null, pos: markerPosWorld);
		marker.InitMarkerProps(markerText, markerIconName);
	}
};
