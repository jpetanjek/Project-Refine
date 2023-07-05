modded class SCR_AIGroup
{
	override bool RplLoad(ScriptBitReader reader)
    {
		int factionIndex;
		reader.ReadInt(factionIndex);
		if (factionIndex >= 0)
			BroadCastSetFaction(factionIndex);

		reader.ReadInt(m_iGroupRadioFrequency);
		reader.ReadInt(m_iGroupID);

		int count, playerID;
		reader.ReadInt(count);
		for (int i = count - 1; i >= 0; i--)
		{
			reader.ReadInt(playerID);
			m_aPlayerIDs.Insert(playerID);
			s_OnPlayerAdded.Invoke(this,playerID);
		}

		if (m_bPlayable)
		{
			SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
			if (groupsManager)
			{
				groupsManager.RegisterGroup(this);
				groupsManager.ClaimFrequency(GetRadioFrequency(), GetFaction());
				groupsManager.OnGroupCreated(this);
			}
		}

		reader.ReadInt(m_iLeaderID);
		s_OnPlayerLeaderChanged.Invoke(m_iGroupID, m_iLeaderID);
		reader.ReadBool(m_bPrivate);
		s_OnPrivateGroupChanged.Invoke();
		
		reader.ReadString(m_sCustomDescription);
		s_OnCustomDescChanged.Invoke();
		reader.ReadString(m_sCustomName);
		s_OnCustomNameChanged.Invoke();
		
		if (!m_UiInfo)
		 m_UiInfo = new SCR_AIGroupUIInfo();
		
		string flag = m_UiInfo.GetGroupFlag();
		reader.ReadString(flag);
		
		RplId groupID;
		reader.ReadRplId(groupID);
		m_MasterGroup = SCR_AIGroup.Cast(Replication.FindItem(groupID));
		reader.ReadRplId(groupID);
		m_SlaveGroup = SCR_AIGroup.Cast(Replication.FindItem(groupID));
		
		reader.ReadInt(m_iDescriptionAuthorID);
		reader.ReadInt(m_iNameAuthorID);
		reader.ReadInt(m_iMaxMembers);
		
		return true;
    }	
}