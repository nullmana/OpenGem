# OpenGem

OpenGem : Open Source tower defense engine recreating GiaB's Gemcraft games
=====

The OpenGem project is an unofficial port of Game in a Bottle's Gemcraft games to a modern engine written in C++.

## DISCLAIMER
This is an UNOFFICIAL project whose development is in NO WAY associated with "Game in a Bottle" or "Armor Games"
No code from the original Gemcraft games is copied into this project.
No assets, visual or sound, are included in the project.

## Supported Games
The engine will be designed to import assets from an official, purchased copy of one of the gemcraft games to play.
The engine will support:
 * Gemcraft Labyrinth (Available for free on GiaB's website)
 * Gemcraft Chasing Shadows (Available for purchase on Steam)
 * Gemcraft Frostborn Wrath (Available for purchase on Steam)

Initial development will prioritize GCL over the other games, for simplicity and accessibility.
Support for other Gemcraft games will be added after GCL is feature-complete.

## Feature Development

This is an approximate list of the order features are expected to be added.
This is not a hard dependency ordering, and is subject to change as necessary.

v0.1:
- [x] Dynamic map, able to construct and remove buildings on the map
- [x] Pathfinding, able to route monsters around obstacles to reach the orb
- [x] Towers, able to shoot projectiles which behave as in GC
- [x] Traps, able to strike monsters passing within range

v0.2:
- [x] Gems, able to be created and placed into buildings
- [x] Amplifiers, able to be placed on the map and affect towers and traps, no specific math yet
- [x] Primitive UI elements, able to select actions with mouse and keyboard with visual response

v0.3:
- [ ] Mana pool displayed and used for gem creation, combining, banishment, etc.
- [ ] Gems have stats and can be combined to create higher-grade gems
- [ ] Amplifiers affect gems in adjacent tiles
- [ ] Shrines can be placed and used, no visual effects required
- [ ] Structures, monster nests and beacons which can be placed on the map and targeted by gems
- [ ] Mana Shards
- [ ] Gem bombing
- [ ] Waves and Monster Types, spawn when called

v0.4:
- [ ] Import graphics from SWF container. As of this version, a swf will be required to launch the game
- [ ] Tower, Trap, Amplifier, Gem, Structure rendering with real sprites
- [ ] UI rendering with real sprites
- [ ] Monster health bars, visual status effects
- [ ] Ingame Options menu

v0.5:
- [ ] Import levels from SWF container. Can be individually selected, no map screen required yet.
- [ ] Apparitions
- [ ] Shadows
- [ ] Pylons
- [ ] Wave Formations

v0.6:
- [ ] Overworld map screen
- [ ] Skills
- [ ] Badges
- [ ] Stats
- [ ] Level Settings

v0.7:
- [ ] Tutorial
- [ ] Story
- [ ] Forgotten
- [ ] Complete VFX Engine
- [ ] Complete Sound Engine
