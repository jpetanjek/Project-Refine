[ComponentEditorProps(category: "GameScripted/GameMode/Components", description: "Base for gamemode scripted component.")]
class PR_ActiveMapIconInformerComponentClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
class PR_ActiveMapIconInformerComponent : ScriptComponent
{
	[Attribute(uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_IconPrefab;
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		if (!GetGame().GetWorldEntity())
  			return;

		// inform game mode-> ActiveMapIconManagerComponent that we exist
		PR_ActiveMapIconManagerComponent mapManager = PR_ActiveMapIconManagerComponent.GetInstance();
		if(mapManager)
		{
			mapManager.Register(owner, m_IconPrefab);
		}
		
		GetGame().GetGameMode()
	}
	
	
	
}