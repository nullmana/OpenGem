# Changelog

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
