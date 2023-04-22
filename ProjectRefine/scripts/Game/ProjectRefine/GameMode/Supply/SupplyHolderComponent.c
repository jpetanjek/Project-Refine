[EntityEditorProps(category: "GameScripted", description: "Enables a entity to hold supplies", color: "0 0 255 255")]
class PR_SupplyHolderComponentClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
//! Makes a entity able to carry supplies
class PR_SupplyHolderComponent : ScriptComponent
{
	[RplProp()]
	int m_iSupply; // Current supply count
	
	[Attribute("0", desc: "Maximum supplies this component can hold."), RplProp()]
	int m_iMaxSupplies;
	
	[RplProp()]
	protected ref array<RplId> m_AvailableHolders = {};
	
	[RplProp()]
	bool m_bCanTransact = true; // We don't allow for transacting of supplies if you are moving etc.
	
	[Attribute("0", desc: "Range in which this holder queries for transfers.")]
	int m_iRange;
	
	private float m_fElapsedTime = 0;
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FIXEDFRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
	
	override void EOnFixedFrame(IEntity owner, float timeSlice)
	{
		m_fElapsedTime += timeSlice;
		if(m_fElapsedTime >= 1)
		{
			m_fElapsedTime = 0;
			GetAvailableHolders();
			SetTransactionAvailability();
		}
	}
	
	void GetAvailableHolders()
	{
		// Fill in m_AvailableHolders
	}
	
	void SetTransactionAvailability()
	{
		// Check speed and set variable
	}
	
	bool TakeSupplies(PR_SupplyHolderComponent taker, int amount)
	{
		if(m_iSupply >= amount && (taker.m_iSupply + amount) < taker.m_iMaxSupplies)
		{
			m_iSupply -= amount;
			taker.m_iSupply += amount;
			return true;
		}
		return false;
	}
	
	bool GiveSupplies(PR_SupplyHolderComponent giver, int amount)
	{
		if(giver.m_iSupply >= amount && (m_iSupply + amount) < m_iMaxSupplies)
		{
			giver.m_iSupply -= amount;	
			m_iSupply += amount;
			return true;
		}
		return false;
	}
	
	
}