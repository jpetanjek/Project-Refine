[ComponentEditorProps(category: "GameScripted/GameMode/Components", description: "Base for gamemode scripted component.")]
class SCR_ActiveMapIconInformerComponentClass : ScriptComponentClass
{
};

//------------------------------------------------------------------------------------------------
class SCR_ActiveMapIconInformerComponent : ScriptComponent
{
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		if (!GetGame().GetWorldEntity())
  			return;

		// inform game mode-> ActiveMapIconManagerComponent that we exist
	}
	
}