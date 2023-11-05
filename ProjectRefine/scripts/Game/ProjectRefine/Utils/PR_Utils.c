class PR_Utils
{	
	//-------------------------------------------------------------------------------------------------------------------------------
	// Helper functions for debug strings of entities
	// Since now formatting IEntity to string outputs a fucking 200 character string which can't fit on screen anywhere ...
	
	static string GetEntityStringClassPointerPrefab(IEntity entity)
	{
		if (!entity)
			return "null";
		
		string entityRawName = string.Format("%1", entity);
		int _a = entityRawName.IndexOf("<");
		int _b = entityRawName.IndexOfFrom(_a, ">");
		string entityPtrStr = entityRawName.Substring(_a+1, _b - _a - 1);
		
		string className = entity.ClassName();
		
		return string.Format("%1<%2> %3", className, entityPtrStr, entity.GetPrefabData().GetPrefabName());
	}
	
	static string GetObjectStringClassPointer(Class obj)
	{
		if (!obj)
			return "null";
		
		string objRawName = string.Format("%1", obj);
		int _a = objRawName.IndexOf("<");
		int _b = objRawName.IndexOfFrom(_a, ">");
		string entityPtrStr = objRawName.Substring(_a+1, _b - _a - 1);
		
		string className = obj.ClassName();
		
		return string.Format("%1<%2>", className, entityPtrStr);
	}
}