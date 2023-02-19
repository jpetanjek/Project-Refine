modded class SCR_InventoryMenuUI
{
	override bool GetCanInteract()
	{
		// Check base class conditions
		if (!super.GetCanInteract())
			return false;
		
		// Check our specific conditions
		if (!m_pFocusedSlotUI)
			return true;
		
		InventoryItemComponent itemComp = m_pFocusedSlotUI.GetInventoryItemComponent();
		if (!itemComp)
			return true;
		
		return CanTakeOrDropItem(itemComp.GetOwner());
	}
	
	override void Action_Drop()
	{
		InventoryItemComponent itemComp;
		if (m_pSelectedSlotUI)
			itemComp = m_pSelectedSlotUI.GetInventoryItemComponent();
		
		if (itemComp && !CanTakeOrDropItem(itemComp.GetOwner()))
		{
			ResetHighlightsOnAvailableStorages();
			return;
		}
		
		super.Action_Drop();
		return;
	}
	
	bool CanTakeOrDropItem(IEntity item)
	{
		if (item.FindComponent(BaseRadioComponent))	// Keep your hands off that radio
			return false;
		else if (item.FindComponent(BaseLoadoutClothComponent)) // Can't manipulate uniforms
			return false;
		
		return true;
	}
}