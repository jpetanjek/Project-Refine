class PR_SupplyHolderMangerClass : PR_BaseGameModeComponentClass
{
}

class PR_SupplyHolderManger : PR_BaseGameModeComponent
{
	ref array<PR_SupplyHolderComponent> m_aActiveSupplyHolders = new ref array<PR_SupplyHolderComponent>();
	
	protected float m_fElapsedTime = 0;
	
	//------------------------------------------------------------------------------------------------
	static PR_SupplyHolderManger GetInstance()
	{
		BaseGameMode gm = GetGame().GetGameMode();
		if (!gm)
			return null;
		
		return PR_SupplyHolderManger.Cast(gm.FindComponent(PR_SupplyHolderManger));
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	override protected void EOnInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.FRAME);
	}
	
	void ~PR_SupplyHolderManger()
	{
		if(m_aActiveSupplyHolders)
		{
			m_aActiveSupplyHolders.Clear();
			m_aActiveSupplyHolders = null;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(PR_SupplyHolderComponent insertMe)
	{
		int idx = m_aActiveSupplyHolders.Find(insertMe);
		if(!m_aActiveSupplyHolders.IsIndexValid(idx))
		{
			m_aActiveSupplyHolders.Insert(insertMe);
		}
	}
	
	void Unregister(PR_SupplyHolderComponent removeMe)
	{
		int idx = m_aActiveSupplyHolders.Find(removeMe);
		if(m_aActiveSupplyHolders.IsIndexValid(idx))
		{
			m_aActiveSupplyHolders.Remove(idx);
		}
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		m_fElapsedTime += timeSlice;
		if(m_fElapsedTime >= 1)
		{
			m_fElapsedTime = 0;
			if(!m_aActiveSupplyHolders.IsEmpty())
			{
				for(int i = m_aActiveSupplyHolders.Count() -1; i >= 0; i--)
				{
					PR_SupplyHolderComponent selection = m_aActiveSupplyHolders[i];
					if(selection.m_RplComponent && !selection.m_RplComponent.IsProxy())
					{
						selection.SetTransactionAvailability();
					}
				}
			}
		}
	}
}