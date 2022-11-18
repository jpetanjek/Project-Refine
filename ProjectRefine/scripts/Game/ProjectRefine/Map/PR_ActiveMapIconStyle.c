// Class which implements styling of active map icons
[BaseContainerProps()]
class PR_ActiveMapIconStyleBase : Managed
{
	void Apply(PR_ActiveMapIcon mapIcon, MapDescriptorComponent mapDescriptorComp);
}

// Style of characters
[BaseContainerProps()]
class PR_ActiveMapIconStyleCharacter : PR_ActiveMapIconStyleBase
{
	protected const string IMAGE_DEF = "character";
	
	override void Apply(PR_ActiveMapIcon mapIcon, MapDescriptorComponent mapDescriptorComp)
	{
		MapItem mapItem = mapDescriptorComp.Item();
		MapDescriptorProps props = mapItem.GetProps();
		
		mapItem.SetDisplayName("Display Name :D :D");
		mapItem.SetInfoText("Info Text :D", null);
		mapItem.SetBaseType(EMapDescriptorType.MDT_SMALLTREE);
		mapItem.SetImageDef(IMAGE_DEF);
		props.SetImageDef(IMAGE_DEF);
		props.SetTextVisible(true);
		props.SetIconVisible(true);
		props.SetIconSize(4, 1, 1);
		props.SetFrontColor(Color.White);
		props.SetOutlineColor(Color.White);
		props.SetBackgroundColor(Color.White);
		props.SetTextColor(Color.Black);
		
		props.SetFont("{CD2634D279AB011A}UI/Fonts/Roboto/Roboto_Bold.fnt");
		props.SetTextSize(30, 30, 30);
		props.SetTextOffsetX(30);
		props.SetTextOffsetY(30);
		props.SetIconTextAlignH(1);
		props.SetIconTextAlignV(1);
		
		props.Activate(true);
		mapItem.SetProps(props);
	}
}