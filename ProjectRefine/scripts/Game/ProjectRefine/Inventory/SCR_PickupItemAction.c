modded class SCR_PickUpItemAction : SCR_InventoryAction
{
	override bool CanBePerformedScript(IEntity user)
 	{
		if (!PR_Inventory.CanTakeOrDropItem(m_Item.GetOwner()))
			return false;
		
		return super.CanBePerformedScript(user);
	}
}