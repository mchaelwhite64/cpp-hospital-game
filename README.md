# cpp-hospital-game

A text based survival game written in C++. Explore a randomly generated hospital, find the artifact, and defeat the boss to escape.

## Features
- Randomly generated map every play through
- Turn-based combat (block, dodge, run)
- Multiple weapons (fists, knife, sword, gun), each with its own strengths and drawbacks that change how fights play out
- Random loot drops and healing rooms
- Boarded-up rooms and breaking tools (axe and chainsaw)
- Random enemy spawns of varying difficulty
- Rooms remember if you've visited or looted them, so you can't farm enemies or chests

## Build and Run
```
g++ -std=c++11 -pthread -o game hospital_game.cpp
./game
```

## Controls
- `w` / `a` / `s` / `d`: move
- `interact`: interact with the current room
- `quit`: exit the game

## Map Key
| Symbol | Meaning |
|--------|---------|
| `P` | You (also patient rooms once you've left them) |
| `?` | Unexplored room |
| `H` | Hallway |
| `N` | Nursing office |
| `D` | Doctor's office |
| `M` | Medical room |
| `C` | Cafeteria |
| `L` | Locked room |
| `B` | Bathroom, boarded-up room, or boss room |

## Weapons

You start with fists. Weapons are found in cabinets (40% chance per cabinet). When one drops, it's a knife 30% of the time, a sword 35%, or a gun 35%. Equipping a new weapon replaces your current one.

| Weapon | Base Damage | Critical (x1.5) | Special | Notes |
|--------|-------------|-----------------|---------|-------|
| Fists | 8 | 5% chance (12) | Double hit, 10% chance (16) | Default, always available |
| Knife | 14 | 15% chance (21) | Double hit, 25% chance (28) | Reliable and consistent |
| Sword | 20 | 10% chance (30) | "Stab, Stab, Slash" double hit, 30% chance (40) | Highest steady damage |
| Gun | 18 | 20% chance (27) | Rapid fire, 15% chance (54) | 6 bullets, 10% chance to miss, must reload (costs a turn) |

**Gun details:** Rapid fire uses 3 bullets total and needs enough ammo left, otherwise you just get a normal shot. When you run out of ammo, you reload to 6 bullets but deal no damage that turn.

### Breaking Tools
Cabinets can also contain an **axe** or a **chainsaw** (20% chance, split 50/50). These are for boarded-up rooms. The axe breaks after one use, and the chainsaw doesn't.

## Enemies

You have 250 HP. Enemies spawn randomly when you enter certain rooms.

| Enemy | HP | Damage per Hit | Spawn Chance | Where |
|-------|----|----------------|--------------|-------|
| Small | 50 | 10 | 15% | Hallways, patient rooms, bathrooms |
| Medium | 125 | 18 | 10% | Hallways, patient rooms, bathrooms |
| Large | 200 | 24 | 5% | Hallways, patient rooms, bathrooms |
| Guardian | 150 | 20 | 60% | Locked prep chambers (only when you choose to prepare) |
| Boss | 300 | 35 | Always | Boss room (center of the map) |

### Boss Abilities
Each turn the boss has a 30% chance to use one of these:
- **Heavy Strike:** doubles its damage (70)
- **Power Stance:** hits harder (40 damage)
- **Regeneration:** heals 30 HP

You can't run from the boss.

## Combat
Each round you attack automatically with your equipped weapon, then choose how to handle the enemy's attack:

| Action | Effect |
|--------|--------|
| `block` | 50% chance to take half damage |
| `dodge` | 55% chance to avoid the hit completely |
| `run` | 40% chance to escape (not allowed against the boss) |
| `attack` | Brace for the hit, no defensive bonus |

## Special Features

### Typewriter Text
Story and combat text prints one character at a time using `<thread>` and `<chrono>`, giving the game a tense, atmospheric feel. The `typeText()` function takes a delay in milliseconds (30 ms by default). Menus, the map, and health bars use a delay of 0 so they appear instantly.

### Procedurally Generated Map
The 21x21 hospital is built fresh every run using `rand()` seeded with the current time. Hallways form a cross pattern leading to the boss room in the center, with hallways around the edges, and the interior rooms are filled randomly using weighted odds (for example, hallways 25%, patient rooms 18%, locked rooms 10%).

### Fog of War Map
Rooms appear as `?` until you visit them, then show a letter for their type. The map redraws every turn.

### Room Memory
Each room is stored in a `Room` struct that tracks whether it has been visited, looted, or locked. This is how the game prevents chest and medicine "camping."

### Live Health Bars
During combat, both you and the enemy get a 10-segment health bar with exact HP numbers.

### Chance-Based Combat
A helper function, `happens(percent)`, drives dodges, blocks, escapes, critical hits, double hits, misses, and boss abilities, so no two fights play out the same.

### Ammo and Reloading
The gun tracks a 6-bullet magazine, and running dry costs you a turn to reload.

### Stored Health Packs
You can save health packs for later. When you take a big hit (15+ damage) or your HP is low, the game offers to use one on the spot.

### Tool Durability
The axe breaks after one use, while the chainsaw lasts.

## Libraries Used
`<iostream>`, `<vector>`, `<string>`, `<cstdlib>`, `<ctime>`, `<thread>`, `<chrono>`

## Author
Michael White
