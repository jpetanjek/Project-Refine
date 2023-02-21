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
		
		return PR_Inventory.CanTakeOrDropItem(itemComp.GetOwner());
	}
	
	override void Action_Drop()
	{
		// Check if the item can be dragged or replaced
		InventoryItemComponent itemComp;
		if (m_pSelectedSlotUI)
			itemComp = m_pSelectedSlotUI.GetInventoryItemComponent();
		
		if (itemComp && !PR_Inventory.CanTakeOrDropItem(itemComp.GetOwner()))
		{
			ResetHighlightsOnAvailableStorages();
			return;
		}
		
		// Check if destination can be replaced
		// We don't want to drag a backpack into a slot with radio backpack and thus drop the radio backpack
		itemComp = null;
		if (m_pFocusedSlotUI)
			itemComp = m_pFocusedSlotUI.GetInventoryItemComponent();
		
		if (itemComp && !PR_Inventory.CanTakeOrDropItem(itemComp.GetOwner()))
		{
			ResetHighlightsOnAvailableStorages();
			return;
		}
		
		super.Action_Drop();
		return;
	}
}