# Changelog

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
