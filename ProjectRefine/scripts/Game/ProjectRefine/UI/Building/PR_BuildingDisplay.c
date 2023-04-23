[BaseContainerProps(), BaseContainerCustomTitleField("m_sDisplayName")]
class PR_BuildingEntry
{
	[Attribute(desc: "Name to be shown in UI")]
	string m_sDisplayName;
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
	
	// Stack of entries which are added as we go deeper into the menu
	protected ref array<PR_BuildingEntry> m_aEntryStack = {};
	
	// ID of current entry when a category is open
	protected int m_iCurrentEntryId;
	
	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);
		widgets.Init(GetRootWidget());
		
		InputManager im = GetGame().GetInputManager();
		im.AddActionListener("PR_BuildingPrev", EActionTrigger.DOWN, Callback_OnPrev);
		im.AddActionListener("PR_BuildingNext", EActionTrigger.DOWN, Callback_OnNext); 
		
		// Temporary
		OpenEntry(m_BuildingEntry);
	}
	
	override event void OnStopDraw(IEntity owner)
	{
		InputManager im = GetGame().GetInputManager();
		im.RemoveActionListener("PR_BuildingPrev", EActionTrigger.DOWN, Callback_OnPrev);
		im.RemoveActionListener("PR_BuildingNext", EActionTrigger.DOWN, Callback_OnNext); 
	}
	
	override event void UpdateValues(IEntity owner, float timeSlice)
	{
		GetGame().GetInputManager().ActivateContext("PR_BuildingContext", 0);
	}

	
	//----------------------------------------------------------------
	// Entry operations
	
	// Going into the hierarchy
	void OpenEntry(PR_BuildingEntry entry)
	{
		m_aEntryStack.Insert(entry);
		
		ProcessEntry(entry);
	}
	
	// Going back rowards the root
	void CloseEntry()
	{
		m_aEntryStack.Remove(m_aEntryStack.Count()-1); // Remove last
		
		ProcessEntry(m_aEntryStack[m_aEntryStack.Count()-1]); // Open the entry before last
	}
	
	// Going to next entry in a category
	void NextEntry()
	{
		PR_BuildingEntryCategory category = GetCurrentCategory();
		
		if (m_iCurrentEntryId == category.m_aEntries.Count()-1)
			return;
		
		m_iCurrentEntryId++;
		SetEntriesXPos(m_iCurrentEntryId, true);
	}
	
	// Going to previous entry in a category
	void PrevEntry()
	{
		PR_BuildingEntryCategory category = GetCurrentCategory();
		
		if (m_iCurrentEntryId == 0)
			return;
		
		m_iCurrentEntryId--;
		SetEntriesXPos(m_iCurrentEntryId, true);
	}
	
	void ProcessEntry(PR_BuildingEntry entry)
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
			
			m_iCurrentEntryId = 0;
			SetEntriesXPos(0, false);
		}
		else if (PR_BuildingEntryAsset.Cast(entry))
		{
			// Start building mode
		}
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
		
		NextEntry();
	}
	
	void Callback_OnPrev()
	{
		// Ignore if not browsing a category
		if (!GetCurrentCategory())
			return;
		
		PrevEntry();
	}
}