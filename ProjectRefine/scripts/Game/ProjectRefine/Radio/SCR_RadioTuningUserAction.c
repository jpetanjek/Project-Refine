modded class SCR_RadioTuningUserAction : SCR_InventoryAction
{
	protected override void PerformActionInternal(SCR_InventoryStorageManagerComponent manager, IEntity pOwnerEntity, IEntity pUserEntity)
	{
		return; // No way
	}
	
	override bool GetActionNameScript(out string outName)
	{
		outName = "Radio tuning is disabled!"; // Told you!
		return true;
	}
};