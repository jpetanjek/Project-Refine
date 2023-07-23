modded class SCR_FactionManager
{
	//------------------------------------------------------------------------------------------------
	/*!
		Authority:
			Update player faction info for target player with their up-to-date state.
	*/
	override void UpdatePlayerFaction_S(SCR_PlayerFactionAffiliationComponent playerFactionComponent)
	{
		int targetPlayerId = playerFactionComponent.GetPlayerController().GetPlayerId();
		Faction targetFaction = playerFactionComponent.GetAffiliatedFaction();
		int targetFactionIndex = GetFactionIndex(targetFaction);
		
		// Modded in bull*** !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		PR_FactionMemberManager memberManager = PR_FactionMemberManager.GetInstance();
		memberManager.SetPlayerFaction(targetFaction.GetFactionKey(), targetPlayerId);

		// See if we have a record of player in the map
		SCR_PlayerFactionInfo foundInfo;
		if (m_MappedPlayerFactionInfo.Find(targetPlayerId, foundInfo))
		{
			// Adjust player counts
			Faction previousFaction = GetPlayerFaction(targetPlayerId);
			if (previousFaction)
			{
				// But only if previous entry was valid
				int previousIndex = GetFactionIndex(previousFaction);
				if (previousIndex != -1)
				{
					int previousCount;
					m_PlayerCount.Find(previousIndex, previousCount); // Will not set value if not found
					int newCount = previousCount - 1;
					m_PlayerCount.Set(previousIndex, newCount); // Remove this player
					OnPlayerFactionCountChanged(previousFaction, newCount);
				}
			}

			// Update existing record
			foundInfo.SetFactionIndex(targetFactionIndex);
			
			// If new faction is valid, add to player count
			if (targetFactionIndex != -1)
			{
				int previousCount;
				m_PlayerCount.Find(targetFactionIndex, previousCount); // Will not set value if not found
				int newCount = previousCount + 1;
				m_PlayerCount.Set(targetFactionIndex, newCount); // Remove this player
				OnPlayerFactionCountChanged(targetFaction, newCount);
			}
			
			// Raise authority callback
			OnPlayerFactionSet_S(playerFactionComponent, targetFaction);
			
			Replication.BumpMe();
			return;
		}


		// Insert new record
		SCR_PlayerFactionInfo newInfo = SCR_PlayerFactionInfo.Create(targetPlayerId);
		newInfo.SetFactionIndex(targetFactionIndex);
		m_aPlayerFactionInfo.Insert(newInfo);
		// And map it
		m_MappedPlayerFactionInfo.Set(targetPlayerId, newInfo);
		// And since this player was not assigned, increment the count of players for target faction
		if (targetFactionIndex != -1)
		{
			int previousCount;
			m_PlayerCount.Find(targetFactionIndex, previousCount);
			int newCount = previousCount + 1;
			m_PlayerCount.Set(targetFactionIndex, newCount);
			OnPlayerFactionCountChanged(targetFaction, newCount);
		}
		
		
		// Raise authority callback
		OnPlayerFactionSet_S(playerFactionComponent, targetFaction);
		Replication.BumpMe();
	}
}