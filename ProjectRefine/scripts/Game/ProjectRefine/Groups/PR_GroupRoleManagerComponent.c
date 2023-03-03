[EntityEditorProps(category: "GameScripted/Groups", description: "Player groups role manager, attach to group!.")]
class PR_GroupRoleManagerComponentClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
class PR_GroupRoleManagerComponent : ScriptComponent
{
	/* Role config (per faction?) - contains all roles, then how many are available to this group as assigned to by GameMode, for now only 1 per faction/in future Commander will define this
	Role 		Number 		Limitation				Prefab
	SL			1			Only SL
	Rifleman	-1			-1
	Medic		2			-1
	AR			1			-1
	MG			1			-1
	AT			2			-1
	Marksman	1			-1
	Spotter		1			-1
	Grenedier	1			-1
	//Engineer	1			-1
	//Radioman	2			Only FireTeam lead?
	*/
	
	// Initialize availability
	// Manage pick and availability logic (first come, first serve), from server to client
	// Network number of avialabe roles via state
	// Only show those roles that are avialable in UI to client
	// Only allow spawning when server assigns role to player
}