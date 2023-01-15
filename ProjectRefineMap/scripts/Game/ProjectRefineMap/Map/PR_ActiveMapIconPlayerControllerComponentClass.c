[EntityEditorProps(category: "GameScripted/ScriptWizard", description: "ScriptWizard generated script file.")]
class PR_ActiveMapIconPlayerControllerComponentClass : ScriptComponentClass
{
	// prefab properties here
};

//------------------------------------------------------------------------------------------------
/*!
	Performs client-side logic of Active Map Icons and some networking.
	This component should be attacked to Player Controller.
*/
class PR_ActiveMapIconPlayerControllerComponent : ScriptComponent
{

	// Returns component attached to our player controller
	static PR_ActiveMapIconPlayerControllerComponent GetLocalInstance()
	{
		PlayerController pc = GetGame().GetPlayerController();
		return PR_ActiveMapIconPlayerControllerComponent.Cast(pc.FindComponent(PR_ActiveMapIconPlayerControllerComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	//override void EOnFrame(IEntity owner, float timeSlice)
	//{
	// Remember to set FRAME flag below!
	//}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);// | EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
	}

	//------------------------------------------------------------------------------------------------
	void PR_ActiveMapIconPlayerControllerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~PR_ActiveMapIconPlayerControllerComponent()
	{
		
	}

	//------------------------------------------------------------------------------------------------
	void AskAddMapMarker(vector markerPos, string markerText, string markerIconName, int markerColor)
	{
		Rpc(RpcAsk_AddMapMarker, markerPos, markerText, markerIconName, markerColor);
	}
	
	//------------------------------------------------------------------------------------------------
	void AskDeleteMapMarker(PR_ActiveMapIconMarker marker)
	{
		if (!marker)
			return;
		
		RplComponent rpl = RplComponent.Cast(marker.FindComponent(RplComponent));
		RplId id = rpl.Id();
		
		Rpc(RpcAsk_DeleteMapMarker, id);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_AddMapMarker(vector markerPos, string markerText, string markerIconName, int markerColor)
	{
		PlayerController pc = PlayerController.Cast(GetOwner());
		PR_ActiveMapIconManagerComponent mgr = PR_ActiveMapIconManagerComponent.GetInstance();
		
		int fromPlayerId = pc.GetPlayerId();
		
		mgr.AddMapMarker(fromPlayerId, markerPos, markerText, markerIconName, markerColor);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_DeleteMapMarker(RplId rplId)
	{
		PR_ActiveMapIconManagerComponent mgr = PR_ActiveMapIconManagerComponent.GetInstance();
		RplComponent rpl = RplComponent.Cast(Replication.FindItem(rplId));
		if(!rpl)
			return;
		
		PR_ActiveMapIconMarker marker = PR_ActiveMapIconMarker.Cast(rpl.GetEntity());
		mgr.DeleteMapMarker(marker);
	}
};
