class PR_BuildingDisplay : SCR_InfoDisplay
{
	// Temporary we store configuration of buildables here
	[Attribute()]
	protected ref PR_BuildingEntry m_BuildingEntry;
	
	protected ref PR_BuildingDisplayWidgets widgets = new PR_BuildingDisplayWidgets();
	
	// Root of currently open building entry configuration
	protected PR_BuildingEntryCategory m_BuildingEntryRoot;
	
	// Stack of entries and last child entry IDs which are added as we go deeper into the menu
	protected ref array<PR_BuildingEntry> m_aEntryStack = {};
	protected ref array<int> m_aChildEntryIdStack = {};
	
	// ID of current entry when a category is open
	protected int m_iCurrentEntryId;
	
	protected bool m_bActive;
	
	protected ref PR_BuildingPreviewMode m_PreviewMode = new PR_BuildingPreviewMode();
	
	
	// Constants
	protected const float ANIMATION_FADE_IN_SPEED = 6.0;
	
	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);
		widgets.Init(GetRootWidget());
		
		InputManager im = GetGame().GetInputManager();
		im.AddActionListener("PR_BuildingPrev", EActionTrigger.DOWN, Callback_OnPrev);
		im.AddActionListener("PR_BuildingNext", EActionTrigger.DOWN, Callback_OnNext); 
		im.AddActionListener("PR_BuildingOpen", EActionTrigger.DOWN, Callback_OnOpen);
		im.AddActionListener("PR_BuildingClose", EActionTrigger.DOWN, Callback_OnClose);
		im.AddActionListener("PR_BuildingRotate", EActionTrigger.VALUE, Callback_OnRotate);
		im.AddActionListener("CharacterFire", EActionTrigger.DOWN, Callback_OnFire);
		im.AddActionListener("CharacterWeaponADS", EActionTrigger.DOWN, Callback_OnAds);
		
		Deactivate();
	}
	
	override event void OnStopDraw(IEntity owner)
	{
		InputManager im = GetGame().GetInputManager();
		im.RemoveActionListener("PR_BuildingPrev", EActionTrigger.DOWN, Callback_OnPrev);
		im.RemoveActionListener("PR_BuildingNext", EActionTrigger.DOWN, Callback_OnNext); 
		im.RemoveActionListener("PR_BuildingOpen", EActionTrigger.DOWN, Callback_OnOpen);
		im.RemoveActionListener("PR_BuildingClose", EActionTrigger.DOWN, Callback_OnClose);
		im.RemoveActionListener("PR_BuildingRotate", EActionTrigger.VALUE, Callback_OnRotate);
		im.RemoveActionListener("CharacterFire", EActionTrigger.DOWN, Callback_OnFire);
		im.RemoveActionListener("CharacterWeaponADS", EActionTrigger.DOWN, Callback_OnAds);
	}
	
	override event void UpdateValues(IEntity owner, float timeSlice)
	{
		InputManager im = GetGame().GetInputManager();

		if (m_bActive)
		{
			PR_BuildingEntryAsset asset = GetCurrentChildAsset();
			PR_BuildingProviderBaseComponent buildingProvider = FindBuildingProvider();
			PR_FobComponent fob;
			
			//-------------------------------------
			// Check building conditions
			bool enoughResources = 0;
			int supplyCount = 0;
			if (buildingProvider)
			{
				supplyCount = buildingProvider.GetSupply();
				if (asset)
					enoughResources = supplyCount >= asset.m_iCost;
				fob = PR_FobComponent.Cast(buildingProvider.GetOwner().FindComponent(PR_FobComponent));
			}
			
			bool isLeader = IsLeader();
			bool buildingAllowedHere = IsBuildingAllowedAtMyPos();
			bool fobMissing = IsFobMissing(asset, fob);
			bool isAnotherFobTooClose = IsAnotherFobTooClose(asset);
			
			bool canBuild = buildingProvider && enoughResources && isLeader && buildingAllowedHere && !fobMissing && !isAnotherFobTooClose;
			
			//-------------------------------------
			// Update preview mode
			vector buildingAreaPos;
			float buildingAreaRadius;
			if (buildingProvider)
			{
				buildingAreaPos = buildingProvider.GetOwner().GetOrigin();
				buildingAreaRadius = buildingProvider.GetRange();
			}
			m_PreviewMode.Update(timeSlice, canBuild, buildingAreaPos, buildingAreaRadius);
			
			//-------------------------------------
			// Update the UI elements
			
			// Resources count
			widgets.m_ResourcesAmountText.SetText(supplyCount.ToString());
			
			// Resources warning
			widgets.m_ResourcesWarning.SetVisible(asset != null && !enoughResources);
			
			// Role (leader) warning
			widgets.m_SquadLeaderRoleWarning.SetVisible(!isLeader);
			
			// Building not allowed here warning
			widgets.m_BuildingAreaRestrictedWarning.SetVisible(!buildingAllowedHere);
			
			// FOB required warning
			widgets.m_FobRequiredWarning.SetVisible(fobMissing);
			
			// FOB too close warning
			widgets.m_TooCloseToFobWarning.SetVisible(isAnotherFobTooClose);
			
			// Building source name
			string sourceNameText;
			if (!buildingProvider)
				sourceNameText = "-";
			else
			{
				if (fob)
					sourceNameText = "FOB";
				else
				{
					// Find root entity, since supply holder can be attached to slot
					IEntity holderRootEntity = buildingProvider.GetOwner();
					while (holderRootEntity.GetParent())
						holderRootEntity = holderRootEntity.GetParent();
					
					// Vehicle?
					SCR_EditableEntityComponent editableComp = SCR_EditableEntityComponent.Cast(holderRootEntity.FindComponent(SCR_EditableEntityComponent));
					if (editableComp)
						sourceNameText = editableComp.GetDisplayName();
					else
						sourceNameText = buildingProvider.GetOwner().ToString();
				}
			}
			widgets.m_SourceNameText.SetText(sourceNameText);
			
			im.ActivateContext("PR_BuildingContext", 0);
		}
	}
	bool IsLeader()
	{
		SCR_AIGroup group = SCR_GroupsManagerComponent.GetLocalPlayerGroup();
		if (!group)
			return null;
		int playerId = GetGame().GetPlayerController().GetPlayerId();
		return group.IsPlayerLeader(playerId);
	}
	bool IsBuildingAllowedAtMyPos()
	{
		PR_GameMode gm = PR_GameMode.Cast(GetGame().GetGameMode());
		if (!gm)
			return true;
		
		IEntity myEntity = GetGame().GetPlayerController().GetControlledEntity();
		if (!myEntity)
			return false;
		
		PR_CaptureArea captureArea = gm.GetAreaAtPos(myEntity.GetOrigin());
		if (!captureArea)
			return true;
		
		return !gm.IsMainBaseArea(captureArea);
	}
	bool IsFobMissing(PR_BuildingEntryAsset asset, PR_FobComponent fob)
	{
		if (!asset)
			return false;
		
		if ((asset.m_eFlags & PR_EAssetBuildingFlags.REQUIRES_FOB) == 0)
			return false;
		
		return fob == null;
	}
	bool IsAnotherFobTooClose(PR_BuildingEntryAsset asset)
	{
		if (!asset)
			return false;
		
		if ((asset.m_eFlags & PR_EAssetBuildingFlags.FORBID_FOB_2X_RANGE) == 0)
			return false;
		
		// todo we must check not position of player, but future position of FOB
		// at least it's verified in build request RPC now
		// finish this later
		
		IEntity myEntity = GetGame().GetPlayerController().GetControlledEntity();
		if (!myEntity)
			return false;
		
		SCR_Faction myFaction = SCR_Faction.Cast(PR_FactionMemberManager.GetLocalPlayerFaction());
		if (!myFaction)
			return false;
		
		vector myPos = myEntity.GetOrigin();
		
		PR_FobComponent fob = PR_FobComponent.FindFobAtPosition(myFaction.GetId(), myPos, true);
		
		return fob != null;
	}
	
	//----------------------------------------------------------------
	// Activation and deactivation
	
	static void ActivateBuilding()
	{
		PlayerController pc = GetGame().GetPlayerController();
		HUDManagerComponent hudMgr = HUDManagerComponent.Cast(pc.FindComponent(HUDManagerComponent));
		array<BaseInfoDisplay> displays = {};
		hudMgr.GetInfoDisplays(displays);
		foreach (BaseInfoDisplay display : displays)
		{
			PR_BuildingDisplay buildingDisplay = PR_BuildingDisplay.Cast(display);
			if (buildingDisplay)
			{
				buildingDisplay.Activate(PR_BuildingEntryCategory.Cast(buildingDisplay.m_BuildingEntry));
				return;
			}
		}
	}
	
	protected void Deactivate()
	{
		m_bActive = false;
		GetRootWidget().SetVisible(false);
		m_PreviewMode.Deactivate();
	}
	
	protected void Activate(PR_BuildingEntryCategory buildingEntryRoot)
	{
		m_BuildingEntryRoot = buildingEntryRoot;
		m_iCurrentEntryId = 0;
		GetRootWidget().SetVisible(true);
		OpenEntry(m_BuildingEntryRoot);
		m_bActive = true;
	}
	
	
	//----------------------------------------------------------------
	// Entry operations
	
	// Going into the hierarchy
	protected void OpenEntry(PR_BuildingEntry entry)
	{
		PR_BuildingEntryCategory category = PR_BuildingEntryCategory.Cast(entry);
		PR_BuildingEntryAsset asset = PR_BuildingEntryAsset.Cast(entry);
		
		if (category)
		{
			// We are opening a new category
			// Save ID of currently selected child
			if (!m_aChildEntryIdStack.IsEmpty())
				m_aChildEntryIdStack[m_aChildEntryIdStack.Count()-1] = m_iCurrentEntryId;
			
			// Push new entry to stack
			m_aEntryStack.Insert(entry);
			m_aChildEntryIdStack.Insert(0);
			
			ProcessCategory(category, 0);
		}
		else if (asset)
		{
			TryPlaceAsset(asset);
		}
	}
	
	// Going back rowards the root
	protected void CloseEntry()
	{
		int size = m_aEntryStack.Count();
		
		m_aEntryStack.Remove(size-1); // Remove last
		m_aChildEntryIdStack.Remove(size-1);
		size = size - 1;
		
		if (size > 0)
		{
			int prevChildId = m_aChildEntryIdStack[size-1];
			// Open the entry before last
			// It is guaranteed to be a category, otherwise is impossible
			ProcessCategory(PR_BuildingEntryCategory.Cast(m_aEntryStack[size-1]), prevChildId);
		}
		else
			Deactivate();
	}
	
	protected void TryPlaceAsset(PR_BuildingEntryAsset asset)
	{
		PR_BuildingProviderBaseComponent buildingProvider = FindBuildingProvider();
		if (!buildingProvider)
			return;
		
		if (buildingProvider.GetSupply() < asset.m_iCost)
			return;
		
		vector buildingAreaPos = buildingProvider.GetOwner().GetOrigin();
		float buildingAreaRadius = buildingProvider.GetRange();
		
		// Try to place the asset
		vector transform[4];
		bool posValid;
		m_PreviewMode.GetAndValidateTransform(buildingAreaPos, buildingAreaRadius, transform, posValid);
		
		if (!posValid)
			return;
		
		PR_PC_BuildingComponent.GetLocalInstance().AskBuild(asset, buildingProvider, transform);
	}
	
	// Resolves the supply holder from which we are building
	protected PR_BuildingProviderBaseComponent FindBuildingProvider()
	{
		IEntity myEntity = GetGame().GetPlayerController().GetControlledEntity();
		if (!myEntity)
			return null;
		
		vector myPos = myEntity.GetOrigin();
		
		SCR_Faction myFaction = SCR_Faction.Cast(PR_FactionMemberManager.GetLocalPlayerFaction());
		if (!myFaction)
			return null;
		int myFactionId = myFaction.GetId();
		
		PR_BuildingProviderBaseComponent buildingProvider = PR_BuildingProviderBaseComponent.FindBuildingProvider(myFactionId, myPos);
		
		return buildingProvider;
	}
	
	// Going to next/prev entry in a category
	protected void CycleCategoryChildEntry(int offsetSign)
	{
		PR_BuildingEntryCategory category = GetCurrentCategory();
		
		int count = category.m_aEntries.Count();
		if (offsetSign > 0)
		{
			m_iCurrentEntryId++;
			if (m_iCurrentEntryId >= count)
				m_iCurrentEntryId = 0;
		}
		else
		{
			m_iCurrentEntryId--;
			if (m_iCurrentEntryId < 0)
				m_iCurrentEntryId = count-1;
		}
		
		SetEntriesXPos(m_iCurrentEntryId, true);
		SetDescriptionText(category.m_aEntries[m_iCurrentEntryId].m_sDescription);
		
		PR_BuildingEntry currentChildEntry = category.m_aEntries[m_iCurrentEntryId];
		ProcessCategoryChildEntry(currentChildEntry);
	}
	
	// Processes entry - category or asset
	protected void ProcessCategory(PR_BuildingEntryCategory category, int childEntryId)
	{		
		// Create entry name widgets
		while (widgets.m_EntryNames.GetChildren())
			widgets.m_EntryNames.GetChildren().RemoveFromHierarchy();
		
		foreach (PR_BuildingEntry e : category.m_aEntries)
		{
			Widget w = GetGame().GetWorkspace().CreateWidgets("{761FCDF0F144F139}UI/Building/EntryName.layout", widgets.m_EntryNames);
			TextWidget wText = TextWidget.Cast(w.FindAnyWidget("NameText"));
			wText.SetText(e.m_sDisplayName);
			
			PR_BuildingEntryNameComponent entryNameComp = PR_BuildingEntryNameComponent.Cast(w.FindHandler(PR_BuildingEntryNameComponent));
			
			PR_BuildingEntryAsset entryAsset = PR_BuildingEntryAsset.Cast(e);
			if (entryAsset)
				entryNameComp.Init(entryAsset.m_sDisplayName, cost: entryAsset.m_iCost, costVisible: true);
			else
				entryNameComp.Init(e.m_sDisplayName, costVisible: false);
		}
		
		m_iCurrentEntryId = childEntryId;
		
		// Animate entry names position
		SetEntriesXPos(m_iCurrentEntryId, false);
		
		// Animate entry names opacity
		widgets.m_EntryNames.SetOpacity(0);
		AnimateWidget.Opacity(widgets.m_EntryNames, 1, ANIMATION_FADE_IN_SPEED);
		
		SetDescriptionText(category.m_aEntries[m_iCurrentEntryId].m_sDescription);
		
		PR_BuildingEntry currentChildEntry = category.m_aEntries[m_iCurrentEntryId];
		ProcessCategoryChildEntry(currentChildEntry);
		
		// Entry name
		widgets.m_CurrentEntryNameText.SetText(category.m_sDisplayName);
	}
	
	// Processes child entry of a category, if we are browsing a category
	// Essentially it activate building preview mode
	protected void ProcessCategoryChildEntry(PR_BuildingEntry entry)
	{
		PR_BuildingEntryCategory category = PR_BuildingEntryCategory.Cast(entry);
		PR_BuildingEntryAsset asset = PR_BuildingEntryAsset.Cast(entry);
		
		if (category)
		{
			m_PreviewMode.Deactivate();
		}
		else if (asset)
		{
			bool orientToSurface = asset.m_eFlags & PR_EAssetBuildingFlags.ORIENT_TO_SURFACE;
			m_PreviewMode.Activate(asset.m_sPrefab, orientToSurface);
		}
	}
	
	//----------------------------------------------------------------
	// Stack operations
	
	// Returns current category
	protected PR_BuildingEntryCategory GetCurrentCategory()
	{
		return PR_BuildingEntryCategory.Cast(m_aEntryStack[m_aEntryStack.Count()-1]);
	}
	
	// Returns currently selected child asset, if current entry is a category
	protected PR_BuildingEntryAsset GetCurrentChildAsset()
	{
		PR_BuildingEntryCategory category = PR_BuildingEntryCategory.Cast(m_aEntryStack[m_aEntryStack.Count()-1]);
		if (!category)
			return null;
		
		PR_BuildingEntry currentChildEntry = category.m_aEntries[m_iCurrentEntryId];
		return PR_BuildingEntryAsset.Cast(currentChildEntry);;
	}
	
	//----------------------------------------------------------------
	// Widget operations
	
	protected void SetEntriesXPos(int id, bool animate)
	{
		if (animate)
		{
			float endPosX = CalculateEntriesXPos(id);
			float endPosY = FrameSlot.GetPosY(widgets.m_EntryNames);
			float endPos[2] = {endPosX, endPosY};
			WidgetAnimationPosition animation = AnimateWidget.Position(widgets.m_EntryNames, endPos, 6);
			if (animation)
				animation.SetCurve(EAnimationCurve.EASE_OUT_SINE);
		}
		else
			FrameSlot.SetPosX(widgets.m_EntryNames, CalculateEntriesXPos(id));
	}
	
	protected float CalculateEntriesXPos(int id)
	{
		const float entryWidth = 250; // Check width of EntryName.layout
		return -(id * entryWidth) - 0.5*entryWidth;
	}
	
	protected void SetDescriptionText(string description)
	{
		widgets.m_DescriptionText.SetText(description);
		
		widgets.m_DescriptionText.SetOpacity(0);
		AnimateWidget.Opacity(widgets.m_DescriptionText, 1.0, ANIMATION_FADE_IN_SPEED);
	}
	
	//----------------------------------------------------------------
	// Input callbacks
	
	protected void Callback_OnNext()
	{
		// Ignore if not browsing a category
		if (!GetCurrentCategory())
			return;
		
		CycleCategoryChildEntry(1);
	}
	
	protected void Callback_OnPrev()
	{
		// Ignore if not browsing a category
		if (!GetCurrentCategory())
			return;
		
		CycleCategoryChildEntry(-1);
	}
	
	// Same action for opening category or placing asset
	protected void Callback_OnOpen()
	{
		PR_BuildingEntryCategory category = GetCurrentCategory();
		
		if (!category)
			return;
		
		PR_BuildingEntry currentEntry = category.m_aEntries[m_iCurrentEntryId];
		OpenEntry(currentEntry);
	}
	
	protected void Callback_OnClose()
	{
		CloseEntry();
	}
	
	protected void Callback_OnRotate(float value)
	{
		if (!m_bActive) // This one is called even if context is not active, somehow
			return;
		
		if (!GetCurrentChildAsset())
			return;
		
		if (value == 0)
			return;
		
		int direction = 0;
		if (value > 0)
			direction = 1;
		else if (value < 0)
			direction = -1;
		
		m_PreviewMode.CycleDirection(direction);
	}
	
	// Quickly close the building mode when we want to fire
	protected void Callback_OnFire()
	{
		Deactivate();
	}
	
	protected void Callback_OnAds()
	{
		Deactivate();
	}
}