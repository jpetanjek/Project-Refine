//------------------------------------------------------------------------------------------------
/*!
	Class generated via ScriptWizard.
*/

class PR_BuildableEventHandlerComponentClass : ScriptComponentClass
{
};

class PR_BuildableEventHandlerComponent : ScriptComponent
{
	// Called when entity is built
	void OnBuild(PR_BuildingManager buildingManager);
	
	// Called when Building Manager is enabled or disabled
	void OnEnabled(PR_BuildingManager buildingManager, bool enabled);
};