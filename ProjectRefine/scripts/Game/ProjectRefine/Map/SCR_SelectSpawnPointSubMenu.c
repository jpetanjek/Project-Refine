//------------------------------------------------------------------------------------------------
modded class SCR_SelectSpawnPointSubMenu
{
	//------------------------------------------------------------------------------------------------
	// Fix the condition:
	// else if ((sp.GetParent() != null ) && sp.GetParent().Type() == GenericEntity)
	// We don't want it to run for our spawn points attached to capture areas
	override void UpdateAndShowSelection()
	{
		if (!m_RespawnSystemComponent || !m_MapEntity || !m_MapEntity.IsOpen())
			return;

		Faction faction = m_RespawnSystemComponent.GetPlayerFaction(m_iPlayerId);
		if (!faction)
			return;

		string factionKey = faction.GetFactionKey();
		array<SCR_SpawnPoint> spawnPoints = SCR_SpawnPoint.GetSpawnPointsForFaction(factionKey);
		int cnt = spawnPoints.Count();
		SetButtonsEnabled(cnt > 1);

		for (int i = 0; i < cnt; ++i)
		{
			SCR_SpawnPoint sp = spawnPoints[i];

			RplId id = SCR_SpawnPoint.GetSpawnPointRplId(sp);
			if (!id.IsValid())
				continue;

			if (m_mSpawnPoints.Get(id))
				continue;

			string spName = m_sDefaultRespawnPointName;
			// todo: bases and mobile hqs also could have ui info?
			if ((sp.GetParent() != null ) && sp.GetParent().Type() == SCR_CampaignBase)
			{
				SCR_CampaignBase base = SCR_CampaignBase.Cast(sp.GetParent());
				spName = base.GetBaseName();

				/* TODO FIX MAP???
				if (base.GetType() == CampaignBaseType.MAIN)
					m_DefaultSpawnPointId = id;
				*/
			}
			/*
			else if ((sp.GetParent() != null ) && sp.GetParent().Type() == GenericEntity)
			{
				spName = m_sMobileAssembly;
			}
			*/
			else
			{
				SCR_UIInfo info = sp.GetInfo();
				if (info)
					spName = info.GetName();

				if (m_SelectionSpinBox.m_aElementNames.Find(spName) > -1)
				{
					int spId = i+1;
					spName = string.Format("%1 %2", spName, spId);
				}

				if (id == 0)
					m_DefaultSpawnPointId = id;
			}

			m_mSpawnPoints.Set(id, spName);
			if (m_SelectionSpinBox.m_aElementNames.Find(spName) == -1)
				m_SelectionSpinBox.AddItem(spName);
		}

		foreach (RplId id, string name : m_mSpawnPoints)
		{
			SCR_SpawnPoint sp = SCR_SpawnPoint.GetSpawnPointByRplId(id);
			if (!sp)
				continue;
			if (sp.GetFactionKey() != factionKey)
			{
				if (sp == SCR_SpawnPoint.GetSpawnPointByRplId(SCR_SpawnPoint.s_LastUsed))
					SCR_SpawnPoint.s_LastUsed = RplId.Invalid();
				int nameId = m_SelectionSpinBox.m_aElementNames.Find(name);
				if (nameId != -1)
					m_SelectionSpinBox.RemoveItem(nameId);
				m_mSpawnPoints.Remove(id);
			}
		}

		SetDeployAvailable();
		UpdateTimedSpawnPoint();
	}
};