[ComponentEditorProps(category: "GameScripted/GameMode", description: "")]
class SCR_ActiveMapIconManagerComponentClass: SCR_BaseGameModeComponentClass
{
};

//------------------------------------------------------------------------------------------------
//! Manages streaming logic of all icons - for now stream everything registered to everyone
class SCR_ActiveMapIconManagerComponent: SCR_BaseGameModeComponent
{	
	array<IEntity> m_AllRegisteredEntities;
	array<IEntity> m_NewlyRegisteredEntities;
	// TODO: Maybe map<IEntity, icon> and then unregister rather than nullchecking
		
	void Register(IEntity target,ResourceName m_ActiveMapIconPrefab)
	{
		if(m_ActiveMapIconPrefab != null)
		{
			SCR_ActiveMapIcon activeMapIcon = SCR_ActiveMapIcon.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_ActiveMapIconPrefab), GetOwner().GetWorld()));
			activeMapIcon.SetTarget(target);
			m_AllRegisteredEntities.Insert(activeMapIcon);
			m_NewlyRegisteredEntities.Insert(activeMapIcon);
		}
	}
	
	override void EOnFixedFrame(IEntity owner, float timeSlice)
	{
		// TODO: Only runs on master/server
		StreamingLogic();
		m_NewlyRegisteredEntities.Clear();
	}
	
	void StreamingLogic()
	{
		// Stream new ones in to everyone
		array<int> indiciesToRemove;
		
		for(int i = 0; i < m_NewlyRegisteredEntities.Count(); i++)
		{
			if(m_NewlyRegisteredEntities[i] != null)
			{
				// Order its stream in to every player
				RplComponent rpl = RplComponent.Cast(m_NewlyRegisteredEntities[i].FindComponent(RplComponent));
				if(rpl != null)
				{
					array<int> players;
					GetGame().GetPlayerManager().GetPlayers(players);
					
					for(int j = 0; j < players.Count(); j++)
					{
						RplIdentity identity = GetGame().GetPlayerManager.GetPlayerController(players[j]).GetRplIdentity();
						if(identity != null)
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
		m_Identities.Insert(playerId);
		array<int> indiciesToRemove;
		
		for(int i = 0; i < m_AllRegisteredEntities.Count(); i++)
		{
			if(m_AllRegisteredEntities[i] != null)
			{
				// Order its stream in to this player
				RplComponent rpl = RplComponent.Cast(m_NewlyRegisteredEntities[i].FindComponent(RplComponent));
				if(rpl != null)
				{
					RplIdentity identity = GetGame().GetPlayerManager.GetPlayerController(playerId).GetRplIdentity();
					if(identity != null)
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
	
	override void OnPlayerDisconnected(int playerId)
	{		
		m_Identities.Remove(playerId);
	}
};
