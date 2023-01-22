/*
This component should be attached to game mode. It handles notifications UI.
*/

class PR_NotificationsGameModeComponentClass : PR_BaseGameModeComponentClass
{
}

class PR_NotificationsGameModeComponent : PR_BaseGameModeComponent
{
	override void OnCaptureAreaFactionChanged(PR_CaptureArea area, int oldFactionId, int newFactionId)
	{
		Faction myFaction = SCR_RespawnSystemComponent.GetInstance().GetLocalPlayerFaction();
		FactionManager fm = GetGame().GetFactionManager();
		int myFactionId = fm.GetFactionIndex(myFaction);
		
		if (myFactionId == -1)
			return;
		
		string msgText;
		string msgSound;
		string areaName = area.GetName();
		if (oldFactionId == myFactionId)
		{
			// Lost area
			msgText = string.Format("Point lost: %1", areaName);
			msgSound = SCR_SoundEvent.TASK_CREATED;
		}
		else if (newFactionId == myFactionId)
		{
			// Captured area
			msgText = string.Format("Point captured: %1", areaName);
			msgSound = SCR_SoundEvent.TASK_SUCCEED;
		}
		
		ShowNotification(msgText, msgSound);
	}
	
	void ShowNotification(string text, string sound)
	{
		SCR_PopUpNotification.GetInstance().PopupMsg(text: text,
			duration: 8, text2: string.Empty, prio: -1, sound: sound);
	}
}