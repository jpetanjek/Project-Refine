typedef func PR_OnFobDestroyed;
void PR_OnFobDestroyed(PR_FobComponent fob);

[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class PR_FobComponentClass : ScriptComponentClass
{
	// prefab properties here
};

// Represents an FOB

class PR_FobComponent : ScriptComponent
{
	[RplProp()]
	protected int m_iOwnerFaction = -1;
	
	protected PR_SupplyHolderComponent m_SupplyHolder;
	
	ref ScriptInvokerBase<PR_OnFobDestroyed> m_OnDestroyed = new ScriptInvokerBase<PR_OnFobDestroyed>();
	
	protected static ref array<PR_FobComponent> s_aAll = {};
	
	//------------------------------------------------------------------------------------------------
	void Init(int ownerFactionId)
	{
		m_iOwnerFaction = ownerFactionId;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	PR_SupplyHolderComponent GetSupplyHolder()
	{
		return m_SupplyHolder;
	}
	
	//------------------------------------------------------------------------------------------------
	array<PR_FobComponent> GetAll()
	{
		return s_aAll;
	}
	
	//------------------------------------------------------------------------------------------------
	static PR_FobComponent FindFobAtPosition(int ownerFactionId, vector posWorld)
	{
		foreach (PR_FobComponent fob : s_aAll)
		{
			if (fob.m_iOwnerFaction != ownerFactionId)
				continue;
			
			if (vector.DistanceXZ(fob.GetOwner().GetOrigin(), posWorld) < fob.GetRange())
				return fob;
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetRange()
	{
		return m_SupplyHolder.m_fRange;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetOwnerFactionId()
	{
		return m_iOwnerFaction;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		m_SupplyHolder = PR_SupplyHolderComponent.Cast(owner.FindComponent(PR_SupplyHolderComponent));
		if (!m_SupplyHolder)
			Print("No PR_SupplyHolderComponent found on FOB!", LogLevel.ERROR);
	}

	//------------------------------------------------------------------------------------------------
	void PR_FobComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		s_aAll.Insert(this);
	}

	//------------------------------------------------------------------------------------------------
	void ~PR_FobComponent()
	{
		m_OnDestroyed.Invoke(this);
		
		s_aAll.RemoveItem(this);
	}

};
