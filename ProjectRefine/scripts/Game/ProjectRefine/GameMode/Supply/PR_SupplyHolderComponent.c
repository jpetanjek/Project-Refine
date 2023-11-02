[EntityEditorProps(category: "GameScripted", description: "Enables a entity to hold supplies", color: "0 0 255 255")]
class PR_SupplyHolderComponentClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
//! Makes a entity able to carry supplies
class PR_SupplyHolderComponent : ScriptComponent
{
	[Attribute("0", desc: "Starting supply amount."),RplProp()]
	int m_iSupply; // Current supply count
	
	[Attribute("0", desc: "Maximum supplies this component can hold."), RplProp()]
	int m_iMaxSupplies;
	
	ref array<PR_SupplyHolderComponent> m_aAvailableHolders = {};
	
	// Server only
	protected static ref array<PR_SupplyHolderComponent> s_aAllHolders = {};
	
	[RplProp()]
	bool m_bCanTransact = true; // We don't allow for transacting of supplies if you are moving etc.
	
	[Attribute("0", desc: "Range in which this holder queries for transfers.")]
	float m_fRange;
	
	[Attribute("0", desc: "Speed under which transfering is available.")]
	float m_fTransferSpeed;
	
	private float m_fElapsedTime = 0;
	
	Physics m_physComponent;
	
	RplComponent m_RplComponent;
	
	PR_SupplyHolderComponent m_ActionTarget;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		SetEventMask(owner, EntityEvent.INIT);
		
		s_aAllHolders.Insert(this);
		
		m_physComponent = owner.GetPhysics();
		
		m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
	}
	
	override void EOnInit(IEntity owner)
	{
		//SetEventMask(owner, EntityEvent.DIAG);
		owner.SetFlags(EntityFlags.ACTIVE, true);
		
		if(m_RplComponent && !m_RplComponent.IsProxy())
		{
			PR_SupplyHolderManger instance = PR_SupplyHolderManger.GetInstance();
			if(instance)
				instance.Register(this);
		}
	}
	
	void ~PR_SupplyHolderComponent()
	{
		int idx = s_aAllHolders.Find(this);
		if(s_aAllHolders.IsIndexValid(idx))
			s_aAllHolders.Remove(idx);
		
		// Unregister, don't check m_RplComponent, when the destructor runs the RplComponent is already null
		PR_SupplyHolderManger instance = PR_SupplyHolderManger.GetInstance();
		if(instance)
			instance.Unregister(this);
		
		m_aAvailableHolders.Clear();
		m_aAvailableHolders = null;
	}
	
	void UpdateAvailableHolders()
	{
		// Fill in m_aAvailableHolders from s_aAllHolders
		m_aAvailableHolders.Clear();
		int thisIdx = s_aAllHolders.Find(this);
		vector thisPos = GetOwner().GetOrigin();
		for(int i = 0; i < s_aAllHolders.Count(); i++)
		{
			if(i == thisIdx)
				continue;
			
			PR_SupplyHolderComponent other = s_aAllHolders[i];
			vector otherPos = other.GetOwner().GetOrigin();
			
			float distance = vector.Distance(thisPos, otherPos);
			
			if(distance <= Math.Max(m_fRange, other.m_fRange))
			{
				m_aAvailableHolders.Insert(s_aAllHolders[i]);
			} 
		}
	}
	
	static array<PR_SupplyHolderComponent> GetAll()
	{
		return s_aAllHolders;
	}
	
	void SetTransactionAvailability()
	{
		// Check speed and set variable
		if(m_physComponent)
		{
			if(m_physComponent.GetVelocity().Length() <= m_fTransferSpeed)
				m_bCanTransact = true;
			else
				m_bCanTransact = false;
		}
		else
		{
			m_bCanTransact = true;
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	bool GiveSupplies(PR_SupplyHolderComponent taker, int amount)
	{
		if(m_RplComponent.IsProxy())
			return false;
				
		if(!m_bCanTransact || !taker.m_bCanTransact)
			return false;
		
		if(m_iSupply < amount)
			amount = m_iSupply;
		
		if((taker.m_iSupply + amount) > taker.m_iMaxSupplies)
			amount = taker.m_iMaxSupplies - taker.m_iSupply;
		
		if(m_iSupply >= amount && (taker.m_iSupply + amount) <= taker.m_iMaxSupplies)
		{
			SetSupply(m_iSupply - amount);
			taker.SetSupply(taker.m_iSupply + amount);
			
			Replication.BumpMe();
			
			return true;
		}
		return false;
	}
	
	bool TakeSupplies(PR_SupplyHolderComponent giver, int amount)
	{
		if(m_RplComponent.IsProxy())
			return false;
		
		if(!m_bCanTransact || !giver.m_bCanTransact)
			return false;
		
		if(giver.m_iSupply < amount)
			amount = giver.m_iSupply;
		
		if((m_iSupply + amount) > m_iMaxSupplies)
			amount = m_iMaxSupplies - m_iSupply;
		
		if(giver.m_iSupply >= amount && (m_iSupply + amount) <= m_iMaxSupplies)
		{
			giver.SetSupply(giver.m_iSupply - amount);	
			SetSupply(m_iSupply + amount);
			
			return true;
		}
		return false;
	}
	
	// For adding/consuming supplies by external systems, not other supply holders
	void AddSupplies(int amount)
	{
		m_iSupply = Math.ClampInt(m_iSupply + amount, 0, m_iMaxSupplies);
		Replication.BumpMe();
	}
	
	protected void SetSupply(int amount)
	{
		m_iSupply = amount;
		Replication.BumpMe();
	}
	
	int GetSupply()
	{
		return m_iSupply;
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnDiag(IEntity owner, float timeSlice)
	{
		int amount = 100 + DiagMenu.GetValue(SCR_DebugMenuID.REFINE_SUPPLY_AMOUNT);
		int holderIdx = DiagMenu.GetValue(SCR_DebugMenuID.REFINE_HOLDER_IDX);
		int targetIdx = DiagMenu.GetValue(SCR_DebugMenuID.REFINE_TARGET_IDX);
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.REFINE_SHOW_SUPPLY_HOLDER_INFO))
		{
			// Draw debug text
			const int COLOR_TEXT = Color.WHITE;
		 	const int COLOR_BACKGROUND = Color.BLACK;
			
			string s;
			
			s = s + string.Format("IDX: %1\n", s_aAllHolders.Find(this));
			
			if(s_aAllHolders.IsIndexValid(holderIdx))
			{
				int thisIdx = s_aAllHolders[holderIdx].m_aAvailableHolders.Find(this);
				if(s_aAllHolders[holderIdx].m_aAvailableHolders.IsIndexValid(thisIdx))
				{
					s = s + string.Format("Available IDX: %1\n", thisIdx);
				}
			}
			
			s = s + string.Format("Supply: %1\n", m_iSupply);
			
			s = s + string.Format("In range count: %1\n", m_aAvailableHolders.Count());
			
			vector pos = owner.GetOrigin() + Vector(0, 5, 0);
			if(holderIdx == s_aAllHolders.Find(this))
				DebugTextWorldSpace.Create(GetGame().GetWorld(), s, DebugTextFlags.ONCE, pos[0], pos[1], pos[2], size: 13.0, color: COLOR_TEXT, bgColor: COLOR_RED);
			else if(s_aAllHolders.IsIndexValid(holderIdx) && targetIdx == s_aAllHolders[holderIdx].m_aAvailableHolders.Find(this))
				DebugTextWorldSpace.Create(GetGame().GetWorld(), s, DebugTextFlags.ONCE, pos[0], pos[1], pos[2], size: 13.0, color: COLOR_TEXT, bgColor: COLOR_GREEN);
			else
				DebugTextWorldSpace.Create(GetGame().GetWorld(), s, DebugTextFlags.ONCE, pos[0], pos[1], pos[2], size: 13.0, color: COLOR_TEXT, bgColor: COLOR_BACKGROUND);

			
			Shape.CreateCylinder(Color.RED, ShapeFlags.VISIBLE | ShapeFlags.ONCE | ShapeFlags.WIREFRAME, GetOwner().GetOrigin(), m_fRange, 40.0);
		}
		
		if(m_aAvailableHolders.IsIndexValid(targetIdx) && holderIdx == s_aAllHolders.Find(this) && m_RplComponent)
		{
			RplComponent rplComponent = RplComponent.Cast(m_aAvailableHolders[targetIdx].GetOwner().FindComponent(RplComponent));
			RplId target = rplComponent.Id();
						
			if(target.IsValid())
			{
				if(DiagMenu.GetBool(SCR_DebugMenuID.REFINE_TAKE_SUPPLY))
				{
					PR_PC_SupplyHolderInformerComponent.GetLocalPlayerControllerSupplyHolderInformerComponent().RequestSupplyAction(m_RplComponent.Id(), target, true);
					DiagMenu.SetValue(SCR_DebugMenuID.REFINE_TAKE_SUPPLY, false);
				}
				
				if(DiagMenu.GetBool(SCR_DebugMenuID.REFINE_GIVE_SUPPLY))
				{
					PR_PC_SupplyHolderInformerComponent.GetLocalPlayerControllerSupplyHolderInformerComponent().RequestSupplyAction(m_RplComponent.Id(), target, false);
					DiagMenu.SetValue(SCR_DebugMenuID.REFINE_GIVE_SUPPLY, false);
				}
			}
		}		
	}	
}