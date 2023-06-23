# Mission Headers and Worlds
Configuration of maps is achieved by both world and mission header which launches that world.

World specifies only main bases, capture points, vehicle and character spawn positions.

Mission header specifies which factions will be used, type of game mode (Invasion or Advance and Secure), and faction randomization.

Therefore same world can be reused with different factions or game mode types, and also you can override those values through server configuration file.

# Mission Header
Those values are relevant for game mode configuration:

`m_sRefineFaction_0`, `m_sRefineFaction_1` - these values specify keys of factions which will be used. (example: `US`, `USSR`, etc). Note that those factions must be specially configured (modded) to contain lists of Roles and Assets for Escalation to work properly. You can't simply use any faction until it has the Role and Asset list values.

`m_eRefineGameModeArchetype` - specifies the game mode type (Invasion or Advance and Secure)

`m_bRefineRandomizeFactions` - when true, selected factions will be swapped with 50% chance.

# Configuring own factions
