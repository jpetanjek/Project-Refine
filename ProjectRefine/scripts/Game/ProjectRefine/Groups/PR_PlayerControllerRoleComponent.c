[EntityEditorProps(category: "GameScripted/Groups", description: "This component should be attached to player controller and is used to send role requests to server.")]
class PR_PlayerControllerRoleComponentClass : ScriptComponentClass
{
};

class PR_PlayerControllerRoleComponent : ScriptComponent
{
	void TryClaimRole(int index)
	{
		Rpc(Rpc_TryClaimRole_S, index);
		
		if(Replication.IsClient())
			return;
		
		Rpc_TryClaimRole_S(index);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void Rpc_TryClaimRole_S(int index)
	{
		// Get player ID
		SCR_PlayerController pc = SCR_PlayerController.Cast(GetOwner());
		if(!pc)
			return;
		
		int playerID = pc.GetPlayerId();
		
		// Find player's group
		SCR_GroupsManagerComponent groupsManager = SCR_GroupsManagerComponent.GetInstance();
		if (!groupsManager)
			return;
			
		SCR_AIGroup group = groupsManager.GetPlayerGroup(playerID);
		if(!group)
			return;
		
		// Get it's PR_RoleListComponent
		PR_GroupRoleManagerComponent groupRoleManager = PR_GroupRoleManagerComponent.Cast(group.FindComponent(PR_GroupRoleManagerComponent));
		if(!groupRoleManager)
			return;
		
		// Attempt to claim role
		if(!groupRoleManager.ClaimRole(index, playerID))
		{
			// If you cant claim role, give notification to client
		}
	}
}