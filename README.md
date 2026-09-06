# Metal Gear MSX PC Port

A reverse engineering project: decompiling and porting the original **Metal Gear (1987, MSX2)** to PC using C and SDL2.

## Status

🎮 **Playable Proof of Concept** — Full game engine with real ROM-extracted level data, collision detection, guard AI, and game over conditions.

## What This Is

This project demonstrates a complete reverse engineering pipeline:
1. **ROM extraction** — Split 128KB Metal Gear cartridge into 16 individual 8KB banks
2. **ROM analysis** — Identify level data patterns within raw bank files
3. **Data extraction** — Convert ROM tilemaps to C arrays
4. **Engine rewrite** — Recreate the MSX2 game logic in modern C/SDL2

**Result:** A playable Metal Gear engine that boots real game levels from the original 1987 ROM.

## Building

### Requirements
- **C Compiler**: Visual Studio, GCC, or Clang
- **CMake**: 3.14+
- **SDL2**: Automatically downloaded via CMakeLists.txt

### Steps

```bash
cd metal-gear-pc

# Visual Studio: Open the folder, select the build config, press F5
# Or use CMake directly:
mkdir build && cd build
cmake ..
cmake --build .
./metal_gear_pc
```

The game will:
- Open a 512×384 window (2× MSX2 native resolution)
- Load the extracted Metal Gear level from Bank 0x0F
- Let you move Snake with arrow keys
- Show a red guard patrol the level
- Game Over if the guard catches you

## Architecture

### Core Modules

| File | Purpose |
|------|---------|
| `src/memory.c/h` | Z80 address space simulation (64KB with Konami bank switching) |
| `src/vdp.c/h` | V9938 VDP graphics (128KB VRAM, Screen 5 rendering) |
| `src/game.c/h` | Game logic: entities, collision, AI, level rendering |
| `src/main.c` | SDL2 entry point and event loop |

### Asset Pipeline

| Tool | Output |
|------|--------|
| `tools/extractor.py` | Split ROM into 16 banks → `assets/banks/` |
| `tools/analyzer.py` | Scan banks for level data patterns |
| `tools/level_extractor.py` | Convert ROM tilemaps to C arrays |

## Next Steps

- [ ] Extract all 6 levels from remaining ROM banks
- [ ] Extract sprite graphics and render them instead of solid squares
- [ ] Implement advanced guard AI (vision detection, alert states)
- [ ] Add codec system for in-game messages
- [ ] Implement sound (PSG audio extraction from ROM)

## License

This project is for **educational and historical preservation purposes only**. Metal Gear is a trademark of Konami. The original ROM is required to use this project (you must own a copy or legally acquire it).

## References

- MSX2 Technical Handbook: https://konamiman.github.io/MSX2-Technical-Handbook/
- Ghidra Z80 Plugin: https://github.com/VitorVilela7/ghidra-z80
- OpenMSX Emulator: https://openmsx.org/