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
	protected static ref array<PR_SupplyHolderComponent> m_aAllHolders = {};
	
	[RplProp()]
	bool m_bCanTransact = true; // We don't allow for transacting of supplies if you are moving etc.
	
	[Attribute("0", desc: "Range in which this holder queries for transfers.")]
	float m_fRange;
	
	[Attribute("0", desc: "Speed under which transfering is available.")]
	float m_fTransferSpeed;
	
	private float m_fElapsedTime = 0;
	
	Physics m_physComponent;
	
	RplComponent m_RplComponent;
		
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		SetEventMask(owner, EntityEvent.INIT);
		
		m_aAllHolders.Insert(this);
		
		m_physComponent = owner.GetPhysics();
		
		m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
	}
	
	override void EOnInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.FRAME | EntityEvent.DIAG);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
	
	void ~PR_SupplyHolderComponent()
	{
		int idx = m_aAllHolders.Find(this);
		if(m_aAllHolders.IsIndexValid(idx))
			m_aAllHolders.Remove(idx);
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		m_fElapsedTime += timeSlice;
		if(m_fElapsedTime >= 1)
		{
			m_fElapsedTime = 0;
			
			if(m_RplComponent && m_RplComponent.IsProxy())
			{
				if(m_bCanTransact)
				{
					GetAvailableHolders();
				}
			}
			else
			{
				SetTransactionAvailability();
			}
		}
	}
	
	protected void GetAvailableHolders()
	{
		// Fill in m_aAvailableHolders from m_aAllHolders
		m_aAvailableHolders.Clear();
		for(int i = 0; i < m_aAllHolders.Count(); i++)
		{
			vector thisTransform[4];
			vector cmpTransform[4];
			GetOwner().GetWorldTransform(thisTransform);
			m_aAllHolders[i].GetOwner().GetWorldTransform(cmpTransform);
			float distance = vector.Distance(thisTransform, cmpTransform);
			
			if(distance <= m_fRange)
			{
				m_aAvailableHolders.Insert(m_aAllHolders[i]);
			} 
		}
	}
	
	protected void SetTransactionAvailability()
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
	
	bool GiveSupplies(PR_SupplyHolderComponent taker, int amount)
	{
		if(m_RplComponent.IsProxy())
			return false;
		
		Print("Server got GiveSupplies request");
		
		if(!m_bCanTransact || !taker.m_bCanTransact)
			return false;
		
		if(m_iSupply >= amount && (taker.m_iSupply + amount) < taker.m_iMaxSupplies)
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
		
		Print("Server got GiveSupplies request");
		
		if(!m_bCanTransact || !giver.m_bCanTransact)
			return false;
		
		if(giver.m_iSupply >= amount && (m_iSupply + amount) < m_iMaxSupplies)
		{
			giver.SetSupply(giver.m_iSupply - amount);	
			SetSupply(m_iSupply + amount);
						
			return true;
		}
		return false;
	}
	
	protected void SetSupply(int amount)
	{
		m_iSupply = amount;
		Replication.BumpMe();
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
			
			s = s + string.Format("IDX: %1\n", m_aAllHolders.Find(this));
			
			if(m_aAllHolders.IsIndexValid(holderIdx))
			{
				int thisIdx = m_aAllHolders[holderIdx].m_aAvailableHolders.Find(this);
				if(m_aAllHolders[holderIdx].m_aAvailableHolders.IsIndexValid(thisIdx))
				{
					s = s + string.Format("Available IDX: %1\n", thisIdx);
				}
			}
			
			s = s + string.Format("Supply: %1\n", m_iSupply);
			
			s = s + string.Format("In range count: %1\n", m_aAvailableHolders.Count());
			
			vector pos = owner.GetOrigin() + Vector(0, 5, 0);
			if(holderIdx == m_aAllHolders.Find(this))
				DebugTextWorldSpace.Create(GetGame().GetWorld(), s, DebugTextFlags.ONCE, pos[0], pos[1], pos[2], size: 13.0, color: COLOR_TEXT, bgColor: COLOR_RED);
			else if(m_aAllHolders.IsIndexValid(holderIdx) && targetIdx == m_aAllHolders[holderIdx].m_aAvailableHolders.Find(this))
				DebugTextWorldSpace.Create(GetGame().GetWorld(), s, DebugTextFlags.ONCE, pos[0], pos[1], pos[2], size: 13.0, color: COLOR_TEXT, bgColor: COLOR_GREEN);
			else
				DebugTextWorldSpace.Create(GetGame().GetWorld(), s, DebugTextFlags.ONCE, pos[0], pos[1], pos[2], size: 13.0, color: COLOR_TEXT, bgColor: COLOR_BACKGROUND);

			
			Shape.CreateCylinder(Color.RED, ShapeFlags.VISIBLE | ShapeFlags.ONCE | ShapeFlags.WIREFRAME, GetOwner().GetOrigin(), m_fRange, 40.0);
		}
		
		if(m_aAvailableHolders.IsIndexValid(targetIdx) && holderIdx == m_aAllHolders.Find(this) && m_RplComponent)
		{
			RplComponent rplComponent = RplComponent.Cast(m_aAvailableHolders[targetIdx].GetOwner().FindComponent(RplComponent));
			RplId target = rplComponent.Id();
						
			if(target.IsValid())
			{
				if(DiagMenu.GetBool(SCR_DebugMenuID.REFINE_TAKE_SUPPLY))
				{
					Print("TAKE SUPPLY");
					PR_PC_SupplyHolderInformerComponent.GetLocalPlayerControllerSupplyHolderInformerComponent().RequestSupplyAction(m_RplComponent.Id(), target, amount, true);
					DiagMenu.SetValue(SCR_DebugMenuID.REFINE_TAKE_SUPPLY, false);
				}
				
				if(DiagMenu.GetBool(SCR_DebugMenuID.REFINE_GIVE_SUPPLY))
				{
					Print("GIVE SUPPLY");
					PR_PC_SupplyHolderInformerComponent.GetLocalPlayerControllerSupplyHolderInformerComponent().RequestSupplyAction(m_RplComponent.Id(), target, amount, false);
					DiagMenu.SetValue(SCR_DebugMenuID.REFINE_GIVE_SUPPLY, false);
				}
			}
		}		
	}	
}