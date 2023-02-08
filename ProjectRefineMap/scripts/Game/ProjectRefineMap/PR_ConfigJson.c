class PR_ConfigJson : JsonApiStruct
{
	bool ShowCharactersMarkers = true;
	bool ShowVehicleMarkers = true;

	void PR_ConfigJson()
	{
		RegV("ShowCharactersMarkers");
		RegV("ShowVehicleMarkers");
	}
}