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
	void AskAddMapMarker(vector markerPos, string markerText)
	{
		Rpc(RpcAsk_AddMapMarker, markerPos, markerText);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_AddMapMarker(vector markerPos, string markerText)
	{
		PlayerController pc = PlayerController.Cast(GetOwner());
		PR_ActiveMapIconManagerComponent mgr = PR_ActiveMapIconManagerComponent.GetInstance();
		
		int fromPlayerId = pc.GetPlayerId();
		
		mgr.AddMapMarker(markerPos, markerText, fromPlayerId);
	}
	
};
