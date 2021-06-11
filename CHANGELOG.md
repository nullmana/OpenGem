# Changelog

## [0.2.12] - 2021-06-11

### Added

- Gems have components created and combined, track own color combination and total number of components


## [0.2.11] - 2021-06-10

### Added

- Monsters and structures have HP and armor, take appropriate damage from gems and shrines, respect killingShotOnTheWay
- Monster HP rendered as a basic healthbar
- Monster banishment cost multiplier increases when banished by the orb, and is affected by gems amplifying the orb
- Monsters give mana when killed

### Fixed

- Monsters which spawned from a destroyed monster nest will pick a new source node when banished


## [0.2.10] - 2021-06-09

### Added

- Added amplifiers affecting gem stats

### Fixed

- Fixed towers and traps still firing when their socketed gem is being dragged


## [0.2.9] - 2021-06-08

### Added

- Added gem stats in ShotData, created and combined using correct formulas
- Added gem coloring based on components, with GCL and GCCS gems available.
- Towers now use gem range. Towers and traps no longer fire without a socketed gem. Tower shots are now colored based on gem towers.

### Fixed

- Fixed creating gems in inventory creating a gem one grade higher than desired


## [0.2.8] - 2021-06-05

### Added

- Added structure controller and monster nests. Nests spawn monsters and can be destroyed by a tower if they are not indestructible. Can mouse over a monster nest to see underneath it.

### Fixed

- Fixed pathfinder blocking check only looking for monsters on path tiles instead of any pathable tile, eg traps
- Fixed shrine building hotkeys working outside GCL


## [0.2.7] - 2021-06-04

### Added

- Added GCL shrines with attack patterns matching source, activate instantly and with no visual effects

### Fixed

- Fixed wall demolition to only trigger when bombing a wall or demolishable buildings, to avoid wasting demolitions
- Fixed trap causing overflow when placed on bottom row

### Changed

- Renamed BUILDING\_TYPE to TILE\_TYPE since the type is used to describe all tiles, not just buildings


## [0.2.6] - 2021-06-02

### Added

- Added gem bomb demolishing 3x3 area of walls for CS

### Fixed

- Fixed dragged gem resetting when switching between drag combine and drag bomb
- Fixed template bomb turning off immediately when out of mana, should only turn off when a bomb failed due to not having enough mana
- Fixed destroying walls not triggering repath of nearby monsters
- Fixed dropping gem bomb not demolishing walls
- Fixed build wall button turning off when dragging to build row of walls


## [0.2.5] - 2021-05-31

### Added

- Added gem bombing, multiple bombing, template bombing
- Added gem salvage by bombing onto gem anvil, cannot template bomb since this would just burn mana
- Template bombing backported as QoL

### Fixed

- Fixed input state resetting when using hotkeys while dragging a gem


## [0.2.4] - 2021-05-31

### Added

- Added buttons for speed control, frame advance backported as QoL


## [0.2.3] - 2021-05-30

### Added

- Added hotkeys for gem upgrade, duplicate, and salvage
- Added GCL gem anvil for duplicating gems, no bombing for salvage yet
- Added GCCS salvage by dropping gems onto mana bar, backporting as QoL

### Fixed

- Fixed dragged gem going behind button windows since they were later in the root window's child order than the inventory window
- Fixed dragged gem wrapping or getting stuck in the corner of the screen when pointer goes offscreen


## [0.2.2] - 2021-05-30

### Added

- Added gem combining with either hotkey or button, can be activated during dragging
- Added gem duplication by combining into empty slot as in GCCS, backporting as QoL
- Added mana cost to creating gems


## [0.2.1] - 2021-05-29

### Added

- Added mana pool
- Mana pool button in GCL now used to control autopool or expand mana pool, GCCS mana pool automatically expands and increases level
- Monster banishment now costs 1 mana, orb can be broken, does not end game
- Gems and buildings do not currently consume mana
- Debug key Ctrl+M to add mana

### Fixed

- Fixed dragged gem getting stuck when changing input state with a hotkey

### Changed

- Debug key M to spawn monsters moved to N


## [0.2.0] - 2021-05-28

### Added

- Added spell button panel for selecting build options, reflecting hotkey selections
- Added gem creation button for making gems, with standard hotkeys

### Fixed

- Fixed gems in buildings displaying at half size in GCCS mode
- Fixed debug path weight draw using root window instead of map window
- Fixed dragged gem getting stuck when changing input state with a hotkey


## [0.1.2] - 2021-05-27

### Added

- Added Amplifiers, with calculations to determine how many amplifiers a gem is affected by, displayed while amplifier build is selected in debug mode

### Fixed

- Fixed tiles, gems, and monsters using incorrect offsets for translating uint32 colors to RGB
- Made buildings not destroyable if they have a gem socketed

### Changed

- Added DEBUG define to debugflags in Makefile
- Changed building colors to be more distinct
- Moved orb from IngameMap into IngameBuildingController


## [0.1.1] - 2021-05-26

### Added

- Added gems, 6 created during initialization for testing, which can be dragged around, swapped between slots, placed into buildings, and swapped between buildings.
- Improved interface window system to handle input and rendering recursively, with each window handling inputs within its boundary.

### Fixed

- Fixed crash due to incorrect window scaling in GCL mode going out-of-bounds while attempting to draw the map. All windows will now automatically scale to a sane size during initialization


## [0.1.0] - 2021-05-25

### Added

- Initial Release
- Added basic tower defense functionality as part of v0.1 description
- Dynamic Map: Map buildings can be placed with standard hotkeys GC and removed with right click. Both GCL single-tile and GCCS double-tile maps are supported. Map restricts construction based on blocking paths to edges, or blocking monsters already on the map from reaching the orb.
- Pathfinding: Monster pathfinding fully supported with either GCL or GCCS pathing weights. Handles edge cases where a building is placed in front of a monster's path by rerouting.
- Towers: Towers create shots which target monsters with either GCL or GCCS projectile movement. Handles warping shots to monsters reaching the orb. Handles KillingShot retargeting against monsters. Handles edge cases with projectiles flying towards a monster killed by a trap or other projectile.
- Traps: Traps target monsters in a reasonably well-optimized algorithm which only checks adjacent tiles for monsters within range, instead of all monsters.

### Performance
- Traps behave approximately O(M).
	- Map full of traps: M=50k=60fps, M=170k=10fps
- Towers are noticably worse and around O(M*T):
	- T=4:(M=30k=60fps; M=100k=15fps; M=200k=5fps)
	- T=10:(M=10k=60fps; M=20k=40fps; M=50k=15fps; M=100k=7fps)
	- T=24:(M=10k=30fps; M=50k=8fps)
