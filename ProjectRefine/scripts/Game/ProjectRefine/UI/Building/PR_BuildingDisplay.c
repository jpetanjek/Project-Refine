[BaseContainerProps(), BaseContainerCustomTitleField("m_sDisplayName")]
class PR_BuildingEntry
{
	[Attribute(desc: "Name to be shown in UI")]
	string m_sDisplayName;
	
	[Attribute(desc: "Description to be shown in UI")]
	string m_sDescription;
}

[BaseContainerProps(configRoot: true), BaseContainerCustomTitleField("m_sDisplayName")]
class PR_BuildingEntryCategory : PR_BuildingEntry
{
	[Attribute()]
	ref array<ref PR_BuildingEntry> m_aEntries;
}

[BaseContainerProps(), PR_BuildingEntryAssetCustomTitleAttribute()]
class PR_BuildingEntryAsset : PR_BuildingEntry
{
	[Attribute("", UIWidgets.ResourceNamePicker, "Prefab of asset", "et")]
	ResourceName m_sPrefab;
	
	[Attribute("0", UIWidgets.EditBox, "Cost of asset")]
	float m_fCost;
}

class PR_BuildingEntryAssetCustomTitleAttribute : BaseContainerCustomTitle
{	
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{
		string displayName;
		float cost;
		source.Get("m_sDisplayName", displayName);
		source.Get("m_fCost", cost);
		
		title = string.Format("%1 - %2", cost.ToString(), displayName);
		
		return true;
	}
}



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
		
		Deactivate();
	}
	
	override event void OnStopDraw(IEntity owner)
	{
		InputManager im = GetGame().GetInputManager();
		im.RemoveActionListener("PR_BuildingPrev", EActionTrigger.DOWN, Callback_OnPrev);
		im.RemoveActionListener("PR_BuildingNext", EActionTrigger.DOWN, Callback_OnNext); 
		im.RemoveActionListener("PR_BuildingOpen", EActionTrigger.DOWN, Callback_OnOpen);
		im.RemoveActionListener("PR_BuildingClose", EActionTrigger.DOWN, Callback_OnClose);
	}
	
	override event void UpdateValues(IEntity owner, float timeSlice)
	{
		InputManager im = GetGame().GetInputManager();

		if (m_bActive)
			im.ActivateContext("PR_BuildingContext", 0);
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
	void OpenEntry(PR_BuildingEntry entry)
	{
		// Save ID of currently selected child
		if (!m_aChildEntryIdStack.IsEmpty())
			m_aChildEntryIdStack[m_aChildEntryIdStack.Count()-1] = m_iCurrentEntryId;
		
		// Push new entry to stack
		m_aEntryStack.Insert(entry);
		m_aChildEntryIdStack.Insert(0);
		
		ProcessEntry(entry, 0);
	}
	
	// Going back rowards the root
	void CloseEntry()
	{
		int size = m_aEntryStack.Count();
		
		m_aEntryStack.Remove(size-1); // Remove last
		m_aChildEntryIdStack.Remove(size-1);
		size = size - 1;
		
		if (size > 0)
		{
			int prevChildId = m_aChildEntryIdStack[size-1];
			ProcessEntry(m_aEntryStack[size-1], prevChildId); // Open the entry before last
		}
		else
			Deactivate();
	}
	
	// Going to next/prev entry in a category
	void CycleEntry(int offsetSign)
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
		SetDescriptionText(m_iCurrentEntryId);
	}
	
	void ProcessEntry(PR_BuildingEntry entry, int childEntryId)
	{
		PR_BuildingEntryCategory category = PR_BuildingEntryCategory.Cast(entry);
		
		if (category)
		{
			// Show category
			
			// Create entry name widgets
			while (widgets.m_EntryNames.GetChildren())
				widgets.m_EntryNames.GetChildren().RemoveFromHierarchy();
			
			foreach (PR_BuildingEntry e : category.m_aEntries)
			{
				Widget w = GetGame().GetWorkspace().CreateWidgets("{761FCDF0F144F139}UI/Building/EntryName.layout", widgets.m_EntryNames);
				TextWidget wText = TextWidget.Cast(w.FindAnyWidget("NameText"));
				wText.SetText(e.m_sDisplayName);
			}
			
			m_iCurrentEntryId = childEntryId;
			
			// Animate entry names position
			SetEntriesXPos(m_iCurrentEntryId, false);
			
			// Animate entry names opacity
			widgets.m_EntryNames.SetOpacity(0);
			AnimateWidget.Opacity(widgets.m_EntryNames, 1, ANIMATION_FADE_IN_SPEED);
			
			SetDescriptionText(m_iCurrentEntryId);
		}
		else if (PR_BuildingEntryAsset.Cast(entry))
		{
			// Start building mode
		}
		
		// Entry name
		widgets.m_CurrentEntryNameText.SetText(entry.m_sDisplayName);
	}
	
	//----------------------------------------------------------------
	// Stack operations
	
	// Returns current category
	PR_BuildingEntryCategory GetCurrentCategory()
	{
		return PR_BuildingEntryCategory.Cast(m_aEntryStack[m_aEntryStack.Count()-1]);
	}
	
	// Returns current asset
	PR_BuildingEntryAsset GetCurrentAsset()
	{
		return PR_BuildingEntryAsset.Cast(m_aEntryStack[m_aEntryStack.Count()-1]);
	}
	
	//----------------------------------------------------------------
	// Widget operations
	
	void SetEntriesXPos(int id, bool animate)
	{
		if (animate)
		{
			float endPosX = CalculateEntriesXPos(id);
			float endPos[2] = {endPosX, 0};
			WidgetAnimationPosition animation = AnimateWidget.Position(widgets.m_EntryNames, endPos, 6);
			if (animation)
				animation.SetCurve(EAnimationCurve.EASE_OUT_SINE);
		}
		else
			FrameSlot.SetPosX(widgets.m_EntryNames, CalculateEntriesXPos(id));
	}
	
	void SetDescriptionText(int id)
	{
		widgets.m_DescriptionText.SetText(GetCurrentCategory().m_aEntries[id].m_sDescription);
		
		widgets.m_DescriptionText.SetOpacity(0);
		AnimateWidget.Opacity(widgets.m_DescriptionText, 1.0, ANIMATION_FADE_IN_SPEED);
	}
	
	float CalculateEntriesXPos(int id)
	{
		const float entryWidth = 200;
		return -(id * entryWidth) - 0.5*entryWidth;
	}
	
	//----------------------------------------------------------------
	// Input callbacks
	
	void Callback_OnNext()
	{
		// Ignore if not browsing a category
		if (!GetCurrentCategory())
			return;
		
		CycleEntry(1);
	}
	
	void Callback_OnPrev()
	{
		// Ignore if not browsing a category
		if (!GetCurrentCategory())
			return;
		
		CycleEntry(-1);
	}
	
	void Callback_OnOpen()
	{
		PR_BuildingEntryCategory category = GetCurrentCategory();
		
		if (!category)
			return;
		
		PR_BuildingEntry currentEntry = category.m_aEntries[m_iCurrentEntryId];
		OpenEntry(currentEntry);
	}
	
	void Callback_OnClose()
	{
		CloseEntry();
	}
}