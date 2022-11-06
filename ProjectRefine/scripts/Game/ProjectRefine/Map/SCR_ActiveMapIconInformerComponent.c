[ComponentEditorProps(category: "GameScripted/GameMode/Components", description: "Base for gamemode scripted component.")]
class SCR_ActiveMapIconInformerComponentClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
class SCR_ActiveMapIconInformerComponent : ScriptComponent
{
	[Attribute(uiwidget: UIWidgets.ResourcePickerThumbnail, params: "ActiveMapIcon prefab")]
	protected ResourceName m_IconPrefab;
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		if (!GetGame().GetWorldEntity())
  			return;

		// inform game mode-> ActiveMapIconManagerComponent that we exist
		SCR_ActiveMapIconManagerComponent mapManager = SCR_ActiveMapIconManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_ActiveMapIconManagerComponent));
		if(m_Info != null && mapManager != null)
		{
			mapManager.Register(owner, m_IconPrefab);
		}
		
		GetGame().GetGameMode()
	}
	
}