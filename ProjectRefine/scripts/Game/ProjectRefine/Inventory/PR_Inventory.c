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
			if (LoadoutJacketArea.Cast(areaType) ||
				LoadoutPantsArea.Cast(areaType))		// Keep hands off jackets and pants! We don't do war crimes here!
				return false;
			else
				return true;
		}
		
		return true;
	}
}