// This component interfaces PR_BuildingManager to HAB

class PR_BuildableSpawnPointFobEventHandlerComponentClass : PR_BuildableEventHandlerComponentClass
{
};

class PR_BuildableSpawnPointFobEventHandlerComponent : PR_BuildableEventHandlerComponent
{
	override void OnBuild(PR_BuildingManager buildingManager)
	{
		PR_SpawnPointFob spawnPoint = PR_SpawnPointFob.Cast(GetOwner().FindComponent(PR_SpawnPointFob));
		
		int ownerFactionId = buildingManager.GetOwnerFactionId();
		
		spawnPoint.Init(ownerFactionId);
	}
};