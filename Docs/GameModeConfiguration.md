# Game Mode Configuration
This page explains how to configure Escalation maps and factions.

# Mission Headers and Worlds
Configuration of maps is achieved by both world and mission header which launches that world.

World specifies only main bases, capture points, vehicle and character spawn positions.

Mission header specifies which factions will be used, type of game mode (Invasion or Advance and Secure), and faction randomization.

Therefore same world can be reused with different factions or game mode types, and also you can override those values through server configuration file.

# Mission Header Configuration
Those values are relevant for game mode configuration:

`m_eRefineGameModeArchetype` - specifies the game mode type (Invasion or Advance and Secure)

## Faction Randomization
`m_bRefineRandomFactionArrays`, `m_aRefineFactions_0`, `...1`, `...2`, `...3` - these values control faction selection.

You can specify up to 4 arrays with faction keys.

If `m_bRefineRandomFactionArrays` is `0`, at start the game mode will select a random faction from first and second array.

If `m_bRefineRandomFactionArrays` is `1`, at start the game mode will select random factions from two random arrays. Factions from same array can not fight each other, but they will fight factions from other arrays.

For example if you have arrays:
- Array 0: `US`, `USMC`
- Array 1: `USSR`, `FIA`

Then following random combinations will happen: `US`-`USSR`, `US`-`FIA`, `USMC`-`USSR`,  `USMC`-`FIA`.

If you have three arrays:
- Array 0: `US`
- Array 1: `USSR`
- Array 2: `FIA`

Then following combinations will happen if `m_bRefineRandomFactionArrays` is `1`: `US`-`USSR`, `US`-`FIA`, `USSR`-`FIA`.

But If `m_bRefineRandomFactionArrays` is `0`, then only one combination is possible: `US`-`USSR`

If `m_bRefineRandomFactionArrays` is `0`, then, even though game mode will select a random faction from first and second array, the factions from first array will always occupy the first Main Base on the map. We recommend to set `m_bRefineRandomFactionArrays` to `1` for better variety, although you can set it to `0` if you want a specific faction to always be at same main base.

You can also override the values via config.json:
```
"m_bRefineRandomFactionArrays" : 1,
"m_aRefineFactions_0" : ["US", "USMC"],
"m_aRefineFactions_1" : ["USSR", "VDV"],
"m_aRefineFactions_2" : ["FIA", "REBELS"]
```

# Faction Configuration
If you want to use own faction in Escalation, it must satisfy following criteria:
- It must be present in following faction manager prefab with a unique faction key: `{4A188E44289B9A50}Prefabs/MP/Managers/Factions/FactionManager_Editor.et`. This prefab is from base Reforger game, not from Escalation mod.
- It must have following data configured:
  - `PR_AssetList` - specifies list of vehicles for Asset Spanwers
  - `PR_RoleList` - specifies list of player roles
  - `PR_BuildingList` - specifies list of assets for Building Mode

Full description of PR_AssetList, PR_RoleList, PR_BuildingList will be done later, for now refere to existing factions
## PR_AssetList
TBD
## PR_RoleList
TBD
## PR_BuildingList
TBD

# World Configuration

## Create a SubScene
Create a subscene from any world. The base world must not have Game Mode in it since we will add our own. Also base world must not be a Game Master world such as GM_Arland.

## Add Essential Prefabs
Place following prefabs:

`PR_GameMode.et` - prefab with Escalation game mode

`PR_Systems.et` - prefab with various game systems like garbage manager, chat, etc

`SCR_AIWorld....et` - prefab which is only required if you plan to spawn and use AI. Note that every world is likely to have its own prefab, for instance `SCR_AIWorld_Arland.et`, `SCR_AIWorld_Eden.et`.

## Provide Mission Header for testing
To preview your world in World Editor you will also need to have values from Mission Header, such as keys of used factions or game mode type.

You can provide a test mission header through `Test Mission Header` property of `PR_GameMode` prefab.

**!!!** Values in the misison header embedded into game mode are only valid when previewing the world in World Editor! This is only for testing!


## Add Main Bases and Capture Areas
Use the following prefabs:

`PR_MainBaseArea.et` - for Main Bases

`PR_CaptureArea.et` - for intermediate Capture Areas

Place main bases and capture areas anywhere you like.

Find `PR_CaptureArea` component on every main base and capture area, specify following properties:

`Order` - specifies linkage order. In Invasion mode the invaders' Main Base is first and the defenders' main base is last. In AAS it doesn't matter which base is first or last.

`Radius` - size of capture area

`Name` - the name shown in UI and on the map

Now you should have Main Bases and Capture Areas properly linked with arrows in World Editor according to the order you have specified.

![CaptureAreas](Images/CaptureAreas.jpg)

## Add Asset Spawners
Asset Spawners must be added to Main Bases as **child entities**.

Use `PR_AssetSpawner_XXX.et` prefabs. Don't place plain PR_AssetSpawner entities, at least create own prefabs inherited from `PR_AssetSpawner.et`.

![AssetSpawners](Images/AssetSpawners.jpg)

## Add Infantry Spawn Points
Character spawn points must be added to Main Bases as **child entities**.

Use `PR_CharacterSpawnPosition.et` prefab. You should add multiple spawn points to each main base, they will be selected randomly.

![CharacterSpawnPositions](Images/CharacterSpawnPositions.jpg)

## Add Tutorial Signs
Tutorial signs have the official in-game documentation for Escalation.

Use `TutorialSigns.et` prefab.

Appearence and content will change later, but please place them somewhere at main bases for future compatibility.

You can place them anywhere, they don't have to be within main base hierarchy.

![TutorialSigns](Images/TutorialSigns.jpg)

## Test It
Hit the 'Play' button and see how it works.

You can also use PeerTool to test clients.

You might find some [Diag Menu Options](DiagMenu.md) useful for testing your world.
