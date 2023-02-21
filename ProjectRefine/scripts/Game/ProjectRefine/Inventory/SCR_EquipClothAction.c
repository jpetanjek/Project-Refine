modded class SCR_EquipClothAction
{
	override bool CanBePerformedScript(IEntity user)
 	{
		// Check if we can't touch the item
		if (!PR_Inventory.CanTakeOrDropItem(m_Item.GetOwner()))
			return false;
		
		// Check if we can't touch the destination slot
		CharacterInventoryStorageComponent storage = CharacterInventoryStorageComponent.Cast(user.FindComponent(CharacterInventoryStorageComponent));
		if (!storage)
			return false;
		
		InventoryStorageSlot targetSlot = storage.FindSuitableSlotForItem(m_Item.GetOwner());
		if (targetSlot)
		{
			IEntity attachedEntity = targetSlot.GetAttachedEntity();
			if (attachedEntity && !PR_Inventory.CanTakeOrDropItem(attachedEntity))
				return false;
		}
		
		// Finally return base class method
		return super.CanBePerformedScript(user);
	}
}