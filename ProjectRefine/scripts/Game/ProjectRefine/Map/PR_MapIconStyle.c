[BaseContainerProps()]
class PR_MapIconStyleCaptureArea : PR_MapIconStyleBase
{
	override void Apply(MapDescriptorComponent mapDescriptorComp)
	{
		MapItem mapItem = mapDescriptorComp.Item();
		if (!mapItem)
			return;
		
		MapDescriptorProps props = mapItem.GetProps();
		
		mapItem.SetDisplayName("Capture Point Name");
		mapItem.SetBaseType(EMapDescriptorType.MDT_SMALLTREE);
		//mapItem.SetGroupType(EMapDescriptorGroup.MDG_AGGREGATE);
		//mapItem.SetImageDef("capture_point");
		props.SetTextVisible(true);
		props.SetIconVisible(true);
		props.SetIconSize(1, 52.0/128.0, 52.0/128.0);
		props.SetFrontColor(Color.Red);
		props.SetOutlineColor(Color.White);
		props.SetBackgroundColor(Color.White);
		props.SetTextColor(Color.Black);
		
		props.SetFont("{CD2634D279AB011A}UI/Fonts/Roboto/Roboto_Bold.fnt");
		props.SetTextSize(20, 20, 20);
		props.SetTextOffsetX(40);
		props.SetTextOffsetY(-14);
		//props.SetIconTextAlignH(1);
		//props.SetIconTextAlignV(0);
		
		//props.SetVisible(false);
		
		props.Activate(true);
		mapItem.SetProps(props);
	}
}

[BaseContainerProps()]
class PR_MapIconStyleFob : PR_MapIconStyleBase
{
	override void Apply(MapDescriptorComponent mapDescriptorComp)
	{
		MapItem mapItem = mapDescriptorComp.Item();
		if (!mapItem)
			return;
		
		MapDescriptorProps props = mapItem.GetProps();
		
		//mapItem.SetDisplayName("FOB Name Placeholder");
		mapItem.SetBaseType(EMapDescriptorType.MDT_SMALLTREE);
		//mapItem.SetGroupType(EMapDescriptorGroup.MDG_AGGREGATE);
		mapItem.SetImageDef("castle");
		props.SetTextVisible(true);
		props.SetIconVisible(true);
		props.SetIconSize(1, 52.0/128.0, 52.0/128.0);
		props.SetFrontColor(Color.Red);
		props.SetOutlineColor(Color.White);
		props.SetBackgroundColor(Color.White);
		props.SetTextColor(Color.Black);
		
		props.SetFont("{CD2634D279AB011A}UI/Fonts/Roboto/Roboto_Bold.fnt");
		props.SetTextSize(20, 20, 20);
		props.SetTextOffsetX(40);
		props.SetTextOffsetY(-14);
		//props.SetIconTextAlignH(1);
		//props.SetIconTextAlignV(0);
		
		//props.SetVisible(false);
		
		props.Activate(true);
		mapItem.SetProps(props);
	}
}

[BaseContainerProps()]
class PR_MapIconStyleSpawnPointFob : PR_MapIconStyleBase
{
	override void Apply(MapDescriptorComponent mapDescriptorComp)
	{
		MapItem mapItem = mapDescriptorComp.Item();
		if (!mapItem)
			return;
		
		MapDescriptorProps props = mapItem.GetProps();
		
		//mapItem.SetDisplayName("Spawn Point Name Placeholder");
		mapItem.SetBaseType(EMapDescriptorType.MDT_SMALLTREE);
		//mapItem.SetGroupType(EMapDescriptorGroup.MDG_AGGREGATE);
		mapItem.SetImageDef("barrack");
		props.SetTextVisible(true);
		props.SetIconVisible(true);
		props.SetIconSize(1, 52.0/128.0, 52.0/128.0);
		props.SetFrontColor(Color.Red);
		props.SetOutlineColor(Color.White);
		props.SetBackgroundColor(Color.White);
		props.SetTextColor(Color.Black);
		
		props.SetFont("{CD2634D279AB011A}UI/Fonts/Roboto/Roboto_Bold.fnt");
		props.SetTextSize(20, 20, 20);
		props.SetTextOffsetX(40);
		props.SetTextOffsetY(-14);
		//props.SetIconTextAlignH(1);
		//props.SetIconTextAlignV(0);
		
		//props.SetVisible(false);
		
		props.Activate(true);
		mapItem.SetProps(props);
	}
}