// Class which implements styling of active map icons
[BaseContainerProps()]
class PR_MapIconStyleBase : Managed
{
	void Apply(MapDescriptorComponent mapDescriptorComp);
	void SetVisibility(bool visibility, MapDescriptorComponent mapDescriptorComp)
	{
		mapDescriptorComp.Item().GetProps().SetVisible(visibility);
	}
}

// Style of characters
[BaseContainerProps()]
class PR_MapIconStyleCharacter : PR_MapIconStyleBase
{	
	override void Apply(MapDescriptorComponent mapDescriptorComp)
	{
		MapItem mapItem = mapDescriptorComp.Item();
		MapDescriptorProps props = mapItem.GetProps();
		
		//mapItem.SetDisplayName("Display Name :D :D");
		//mapItem.SetInfoText("Info Text :D", null);
		mapItem.SetBaseType(EMapDescriptorType.MDT_SMALLTREE);
		props.SetTextVisible(true);
		props.SetIconVisible(true);
		props.SetIconSize(1, 38.0/128.0, 38.0/128.0);
		props.SetFrontColor(Color.White);
		props.SetOutlineColor(Color.White);
		props.SetBackgroundColor(Color.White);
		props.SetTextColor(Color.Black);
		
		props.SetFont("{CD2634D279AB011A}UI/Fonts/Roboto/Roboto_Bold.fnt");
		props.SetTextSize(14, 14, 14);
		props.SetTextOffsetX(16);
		props.SetTextOffsetY(-7);
		props.SetIconTextAlignH(1);
		props.SetIconTextAlignV(1);
		
		//props.SetVisible(false);
		
		props.Activate(true);
		mapItem.SetProps(props);
	}
}

// Style of highlight of current player. It's a separate texture.
[BaseContainerProps()]
class PR_MapIconStyleCharacterPlayerHighlight : PR_MapIconStyleBase
{	
	override void Apply(MapDescriptorComponent mapDescriptorComp)
	{
		MapItem mapItem = mapDescriptorComp.Item();
		MapDescriptorProps props = mapItem.GetProps();
		
		mapItem.SetBaseType(EMapDescriptorType.MDT_SMALLTREE);
		mapItem.SetImageDef("view_cone");
		props.SetTextVisible(false);
		props.SetIconVisible(true);
		props.SetIconSize(1, 128.0/128.0, 128.0/128.0);
		props.SetFrontColor(Color.White);
		props.SetOutlineColor(Color.White);
		props.SetBackgroundColor(Color.White);
		
		props.Activate(true);
		mapItem.SetProps(props);
	}
}

// Style of vehicles
[BaseContainerProps()]
class PR_MapIconStyleVehicle : PR_MapIconStyleBase
{	
	override void Apply(MapDescriptorComponent mapDescriptorComp)
	{
		MapItem mapItem = mapDescriptorComp.Item();
		MapDescriptorProps props = mapItem.GetProps();
		
		mapItem.SetBaseType(EMapDescriptorType.MDT_SMALLTREE);
		mapItem.SetImageDef("vehicle");
		props.SetTextVisible(true);
		props.SetIconVisible(true);
		props.SetIconSize(1, 38.0/128.0, 38.0/128.0);
		props.SetFrontColor(Color.White);
		props.SetOutlineColor(Color.White);
		props.SetBackgroundColor(Color.White);
		props.SetTextColor(Color.Black);
		
		props.SetFont("{CD2634D279AB011A}UI/Fonts/Roboto/Roboto_Bold.fnt");
		props.SetTextSize(14, 14, 14);
		props.SetTextOffsetX(12);
		props.SetTextOffsetY(-5);
		props.SetIconTextAlignH(1);
		props.SetIconTextAlignV(1);
		
		props.Activate(true);
		mapItem.SetProps(props);
	}
}

[BaseContainerProps()]
class PR_MapIconStyleMarker : PR_MapIconStyleBase
{
	override void Apply(MapDescriptorComponent mapDescriptorComp)
	{
		MapItem mapItem = mapDescriptorComp.Item();
		MapDescriptorProps props = mapItem.GetProps();
		
		mapItem.SetDisplayName("Marker Name");
		mapItem.SetBaseType(EMapDescriptorType.MDT_SMALLTREE);
		props.SetTextVisible(true);
		props.SetIconVisible(true);
		props.SetIconSize(1, 52.0/128.0, 52.0/128.0);
		props.SetFrontColor(Color.White);
		props.SetOutlineColor(Color.White);
		props.SetBackgroundColor(Color.White);
		props.SetTextColor(Color.Black);
		
		props.SetFont("{CD2634D279AB011A}UI/Fonts/Roboto/Roboto_Bold.fnt");
		props.SetTextSize(20, 20, 20);
		props.SetTextOffsetX(30);
		props.SetTextOffsetY(-12);
		//props.SetIconTextAlignH(1);
		//props.SetIconTextAlignV(0);
		
		//props.SetVisible(true);
		
		props.Activate(true);
		mapItem.SetProps(props);
	}
}
