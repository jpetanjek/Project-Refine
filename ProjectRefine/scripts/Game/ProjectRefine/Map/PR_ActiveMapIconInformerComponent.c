[ComponentEditorProps(category: "GameScripted/GameMode/Components", description: "Base for gamemode scripted component.")]
class PR_ActiveMapIconInformerComponentClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
class PR_ActiveMapIconInformerComponent : ScriptComponent
{
	[Attribute(uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_IconPrefab;
	
	protected PR_ActiveMapIconManagerComponent m_MapManager;
	
	protected PR_ActiveMapIcon m_Icon;
	
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
		m_MapManager = PR_ActiveMapIconManagerComponent.GetInstance();
		if(m_MapManager)
		{
			m_Icon = m_MapManager.Register(this, m_IconPrefab);
		}
		
		GetGame().GetGameMode()
	}
	
	void ~PR_ActiveMapIconInformerComponent()
	{
		if(m_MapManager && m_Icon)
		{
			m_MapManager.Unregister(m_Icon);
		}
	}
	
}