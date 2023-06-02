/*
Marks entities which can serve as building 'sources'.
*/


//-------------------------------------------------------------------------------------------
// Base class for building providers
class PR_BuildingProviderBaseComponentClass : ScriptComponentClass {};

class PR_BuildingProviderBaseComponent : ScriptComponent
{
	[Attribute("0", UIWidgets.CheckBox, desc: "Determines order when querying nearby providers. Lowest order is returned first.")]
	protected int m_iOrder;
	
	protected static ref array<PR_BuildingProviderBaseComponent> s_aAll = {};
	
	void PR_BuildingProviderBaseComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		s_aAll.Insert(this);
	}
	
	void ~PR_BuildingProviderBaseComponent()
	{
		s_aAll.RemoveItem(this);
	}
	
	
	static PR_BuildingProviderBaseComponent FindBuildingProvider(int ownerFactionId, vector posWorld)
	{
		PR_BuildingProviderBaseComponent nearestProvider;
		float minScore = float.MAX;
		
		array<PR_BuildingProviderBaseComponent> allProviders = s_aAll;
		foreach (PR_BuildingProviderBaseComponent provider : allProviders)
		{
			// Ignore if too far
			float distance = vector.Distance(posWorld, provider.GetOwner().GetOrigin());
			if (distance > provider.GetRange())
				continue;
			
			// Ignore if not enabled
			if (!provider.IsEnabled())
				continue;
			
			// Ignore if different faction
			if (provider.GetOwnerFactionId() != ownerFactionId)
				continue;
			
			float score = 2048.0 * provider.m_iOrder + distance;
			
			if (score < minScore)
			{
				nearestProvider = provider;
				minScore = score;
			}
		}
		
		return nearestProvider;
	}
	
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.DIAG);
	}
	
	override void EOnDiag(IEntity owner, float timeSlice)
	{
		if (DiagMenu.GetBool(SCR_DebugMenuID.REFINE_SHOW_BUILDING_PROVIDER_STATE))
		{
			// Draw debug text
			const int COLOR_TEXT = Color.WHITE;
		 	const int COLOR_BACKGROUND = 0xFF001000;
			
			FactionManager fm = GetGame().GetFactionManager();
			
			string s;
			s = s + string.Format("%1\n", this);
			s = s + string.Format("Enabled: %1, Order: %2, Range: %3\n", IsEnabled(), m_iOrder, GetRange().ToString(3, 0));
			s = s + string.Format("Supply: %1, Faction: %2\n", GetSupply().ToString(4), GetOwnerFactionId());
			
			vector pos = owner.GetOrigin() + Vector(0, 3, 0);
			DebugTextWorldSpace.Create(GetGame().GetWorld(), s, DebugTextFlags.ONCE, pos[0], pos[1], pos[2], size: 13.0, color: COLOR_TEXT, bgColor: COLOR_BACKGROUND);
			
			Shape.CreateSphere(Color.DARK_GREEN, ShapeFlags.VISIBLE | ShapeFlags.ONCE | ShapeFlags.WIREFRAME, pos, GetRange());
		}
	}
	
	
	//-----------------------------------------------------------
	// Implement in inherited classes
	
	// Must return amount of available supplies
	int GetSupply();
	
	// Must change amount of available supplies by given amount (negative when building)
	void AddSupplies(int amount);
	
	// Must return building range
	float GetRange();
	
	// Must return ID of owner faction
	int GetOwnerFactionId();
	
	// Must return true when it's possible to build from here (ignoring the supply count)
	bool IsEnabled();
};


//-------------------------------------------------------------------------------------------
// Implementation of BuildingProvider which interfaces to PR_SupplyHolderComponent
class PR_BuildingProviderFromSupplyHolderComponentClass : PR_BuildingProviderBaseComponentClass {};

class PR_BuildingProviderFromSupplyHolderComponent : PR_BuildingProviderBaseComponent
{
	protected PR_SupplyHolderComponent m_SupplyHolder;
	
	// Those are not required to be on entity, it depends on configuration
	protected PR_FobComponent m_FobComponent;
	protected FactionAffiliationComponent m_FactionComponent;
	
	protected IEntity m_OwnerEntity;
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		m_OwnerEntity = owner;
		
		m_SupplyHolder = PR_SupplyHolderComponent.Cast(owner.FindComponent(PR_SupplyHolderComponent));
		if (!m_SupplyHolder)
			Print("PR_BuildingProviderFromSupplyHolderComponent requires PR_SupplyHolderComponent!", LogLevel.ERROR);
		
		IEntity rootEntity = owner;
		while (rootEntity.GetParent())
			rootEntity = rootEntity.GetParent();
		
		m_FobComponent = PR_FobComponent.Cast(owner.FindComponent(PR_FobComponent));
		m_FactionComponent = FactionAffiliationComponent.Cast(rootEntity.FindComponent(FactionAffiliationComponent));
		if(!m_FactionComponent)
			Print("Didn't find faction component");
	}
	
	//-----------------------------------------------------------
	// Implement in inherited classes
	
	// Must return amount of available supplies
	override int GetSupply()
	{
		return m_SupplyHolder.m_iSupply;
	}
	
	// Must return building range
	override float GetRange()
	{
		return m_SupplyHolder.m_fRange;
	}
	
	// Must return ID of owner faction
	override int GetOwnerFactionId()
	{
		if(!m_FobComponent && !m_FactionComponent)
		{
			Print("Attempting to find faction component");
			
			IEntity rootEntity = m_OwnerEntity;
			while (rootEntity.GetParent())
				rootEntity = rootEntity.GetParent();
		
			m_FactionComponent = FactionAffiliationComponent.Cast(rootEntity.FindComponent(FactionAffiliationComponent));
		}
		
		if (m_FobComponent)
		{
			// If attached to FOB, get faction from it
			return m_FobComponent.GetOwnerFactionId();
		}
		else if (m_FactionComponent)
		{
			// If attached to vehicle, get faction from faction component
			SCR_Faction faction = SCR_Faction.Cast(m_FactionComponent.GetAffiliatedFaction());
			if (faction)
				return faction.GetId();
			else
			{
				SCR_Faction defaultFaction = SCR_Faction.Cast(m_FactionComponent.GetDefaultAffiliatedFaction());
				if (defaultFaction)
					return defaultFaction.GetId();
				else
					return -1;
			}
		}
		
		return -1;
	}
	
	// Must return true when it's possible to build from here (ignoring the supply count)
	override bool IsEnabled()
	{
		return true;
	}
	
	// Must change amount of available supplies by given amount (negative when building)
	override void AddSupplies(int amount)
	{
		m_SupplyHolder.AddSupplies(amount);
	}
};