modded class SCR_InventoryStorageManagerComponent
{
	// This is made for preventing of swapping radio backpack to another radio
	override bool EquipAny(BaseInventoryStorageComponent storage, IEntity item, int prefered = -1, SCR_InvCallBack cb = null)
	{
		// First check if we can't equip
		if (!storage || !item)
			return false;
		
		// Check if we can't touch the item
		if (!SCR_InventoryMenuUI.CanTakeOrDropItem(item))
			return false;
		
		// Check if we can't touch the destination slot
		InventoryItemComponent itemComp = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		if (!itemComp)
			return false;
		
		InventoryStorageSlot m_TargetSlot = storage.FindSuitableSlotForItem(item);
		if (m_TargetSlot)
		{
			IEntity attachedEntity = m_TargetSlot.GetAttachedEntity();
			if (attachedEntity && !SCR_InventoryMenuUI.CanTakeOrDropItem(attachedEntity))
				return false;
		}
		
		// Finally call base class method
		return super.EquipAny(storage, item, prefered, cb);
	}
}