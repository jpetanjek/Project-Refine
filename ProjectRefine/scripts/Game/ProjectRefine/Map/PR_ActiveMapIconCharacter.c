class PR_ActiveMapIconCharacterClass : PR_ActiveMapIconClass
{
}

class PR_ActiveMapIconCharacter : PR_ActiveMapIcon
{
	// ID of player who controls this character
	[RplProp(onRplName: "OnLocalStateChanged")]
	protected int m_iPlayerId;
	
	override void OnLocalStateChanged()
	{
		super.OnLocalStateChanged();
	}
	
	
}