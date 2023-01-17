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
	void AskAddMapMarker(vector markerPos, string markerText, string markerIconName, int markerColor, PR_EMarkerVisibility markerVisibility)
	{
		Rpc(RpcAsk_AddMapMarker, markerPos, markerText, markerIconName, markerColor, markerVisibility);
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
	int GetMarkerFactionId(PR_EMarkerVisibility markerVisibility)
	{
		if (markerVisibility == PR_EMarkerVisibility.EVERYONE)
			return -1; // Visible for every faction
		
		PlayerController pc = PlayerController.Cast(GetOwner());
		
		// Try get faction from controlled entity
		IEntity controlledEntity = pc.GetControlledEntity();
		if (controlledEntity)
		{
			FactionAffiliationComponent fComp = FactionAffiliationComponent.Cast(controlledEntity.FindComponent(FactionAffiliationComponent));
			if (fComp)
			{
				Faction faction = fComp.GetAffiliatedFaction();
				int factionId = GetGame().GetFactionManager().GetFactionIndex(faction);
				if (factionId != -1)
					return factionId;
			}
		}
		
		// Try get faction from respawn system
		SCR_RespawnSystemComponent respawnSystem = SCR_RespawnSystemComponent.GetInstance();
		SCR_PlayerRespawnInfo playerRespawnInfo = respawnSystem.FindPlayerRespawnInfo(pc.GetPlayerId());
		if (playerRespawnInfo)
		{
			int factionId = playerRespawnInfo.GetPlayerFactionIndex();
			if (factionId != -1)
				return factionId;
		} 
		
		return -1;
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_AddMapMarker(vector markerPos, string markerText, string markerIconName, int markerColor, PR_EMarkerVisibility markerVisibility)
	{
		PlayerController pc = PlayerController.Cast(GetOwner());
		PR_ActiveMapIconManagerComponent mgr = PR_ActiveMapIconManagerComponent.GetInstance();
		
		int fromPlayerId = pc.GetPlayerId();
		
		int factionId = GetMarkerFactionId(markerVisibility);
		
		mgr.AddMapMarker(fromPlayerId, factionId, markerPos, markerText, markerIconName, markerColor);
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
