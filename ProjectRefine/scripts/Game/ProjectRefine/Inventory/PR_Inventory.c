class PR_Inventory
{
	// Represents inventory restrictions of our game mode
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