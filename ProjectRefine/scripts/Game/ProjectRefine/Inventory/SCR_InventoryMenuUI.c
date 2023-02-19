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
		// Check if the item can be dragged or replaced
		InventoryItemComponent itemComp;
		if (m_pSelectedSlotUI)
			itemComp = m_pSelectedSlotUI.GetInventoryItemComponent();
		
		if (itemComp && !CanTakeOrDropItem(itemComp.GetOwner()))
		{
			ResetHighlightsOnAvailableStorages();
			return;
		}
		
		// Check if destination can be replaced
		// We don't want to drag a backpack into a slot with radio backpack and thus drop the radio backpack
		itemComp = null;
		if (m_pFocusedSlotUI)
			itemComp = m_pFocusedSlotUI.GetInventoryItemComponent();
		
		if (itemComp && !CanTakeOrDropItem(itemComp.GetOwner()))
		{
			ResetHighlightsOnAvailableStorages();
			return;
		}
		
		super.Action_Drop();
		return;
	}
	
	static bool CanTakeOrDropItem(IEntity item)
	{
		if (item.FindComponent(BaseRadioComponent))	// Keep your hands off that radio
			return false;
		
		BaseLoadoutClothComponent clothComp = BaseLoadoutClothComponent.Cast(item.FindComponent(BaseLoadoutClothComponent));
		if (clothComp)
		{
			LoadoutAreaType areaType = clothComp.GetAreaType();
			if (LoadoutBackpackArea.Cast(areaType) ||	// You can take a backpack,
				LoadoutHeadCoverArea.Cast(areaType) ||	// a hat
				LoadoutBootsArea.Cast(areaType))		// and boots
				return true;
			else
				return false;	// Keep hands off jackets and pants! We don't do war crimes here!
		}
		
		return true;
	}
}