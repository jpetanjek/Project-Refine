// Tooltip which is shown when we press the action button on group entry
// It has some buttons for group management
class PR_GroupTooltipComponent : PR_ButtonsTooltipComponent
{
	protected SCR_AIGroup m_Group;
	
	void Init(SCR_AIGroup group)
	{
		m_Group = group;
		
		CreateButton("Do Stuff");
		CreateButton("Do Other Stuff");
		CreateButton("Ban Everyone");
	}
}