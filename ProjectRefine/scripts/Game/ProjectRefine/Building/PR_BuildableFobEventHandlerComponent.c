// This component interfaces PR_BuildingManager to PR_FobComponent
// ... since FOB must know which faction has built it or it must react to BuildingManager state

class PR_BuildableFobEventHandlerComponentClass : PR_BuildableEventHandlerComponentClass
{
};

class PR_BuildableFobEventHandlerComponent : PR_BuildableEventHandlerComponent
{
	override void OnBuild(PR_BuildingManager buildingManager)
	{
		PR_FobComponent fobComponent = PR_FobComponent.Cast(GetOwner().FindComponent(PR_FobComponent));
		
		// Initialize the FOB
		int ownerFactionId = buildingManager.GetOwnerFactionId();
		fobComponent.Init(ownerFactionId);
	}
	
	// Here we can also enable/disable FOB from BuildingManager events
	// override void OnEnabled(PR_BuildingManager buildingManager, bool enabled);
};