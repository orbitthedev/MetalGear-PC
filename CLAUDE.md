# Metal Gear PC Port - Development Log

## Current Status: **PLAYABLE DEMO - REAL ROM DATA** ✅

You have a fully playable Metal Gear game engine running on Windows with:
- ✅ MSX2 memory simulation (64KB address space with Konami bank switching)
- ✅ VDP graphics abstraction (Screen 5 rendering: 256x192, 16 colors)
- ✅ **REAL LEVEL DATA extracted from ROM Bank 0x0F (16×20 tiles)**
- ✅ Snake player character with collision detection
- ✅ Patrolling guard NPC with AI
- ✅ **Game Over condition when guard touches player**
- ✅ Auto-reset after 3 seconds on death

## Architecture

### Core Modules
- **`memory.c/h`**: Simulates Z80 address space, loads 16 extracted 8KB ROM banks, handles Konami mapper
- **`vdp.c/h`**: V9938 Video Display Processor abstraction, renders Screen 5 (MSX2 bitmap mode)
- **`game.c/h`**: Game logic, level data, entity management, collision detection
- **`main.c`**: SDL2 initialization, event loop, input handling

### Asset Pipeline
- **`tools/extractor.py`**: Splits 128KB Metal Gear ROM into 16 individual 8KB banks stored in `assets/banks/`

## Next Steps (Priority Order)

### DONE ✅
- ✅ Extract real ROM level from Bank 0x0F
- ✅ Implement collision detection
- ✅ Add game over/reset mechanic

### Phase 1: Extract All 6 Levels (HIGH PRIORITY)
Metal Gear has 6 levels. Likely stored in Banks 0x0F (done) and surrounding banks.
- Modify `tools/analyzer.py` to scan all banks for level data patterns
- Extract and convert each level to C arrays
- Update game engine to cycle through levels

### Phase 2: Add Real Graphics (HIGH PRIORITY)
Current rendering is solid-color squares. Real Metal Gear uses:
- Tile graphics (small 8×8 or 16×16 pixel tiles)
- Character sprites (Snake, guards, items)
- These are likely in Banks 0x08-0x0C

### Phase 3: Sound (MEDIUM PRIORITY)
- Extract PSG audio data from ROM
- Implement simple sound playback via SDL_mixer

### Phase 4: Advanced Mechanics (MEDIUM PRIORITY)
- Guard alert states (vision cone detection)
- Item pickups
- Codec system (messages)
- Multiple guards with different AI patterns

## Known Limitations
- Hardcoded test level only (no ROM level loading yet)
- Entities are solid-color squares (no animation frames)
- No sound yet
- No save/load system
- Guard AI is simple (back-and-forth patrol only)

## Build & Run
```bash
cd C:\Users\Surya\metal-gear-pc
# Visual Studio: Open folder, press F5 to build and run
# Output: playable window, arrow keys to move, ESC to quit
```

## Token Budget Note
Started with ~15M tokens. Current state is a minimal but fully functional vertical slice. Future work should focus on ROM data extraction (parsing) rather than writing new rendering code.

## Files Created This Session
- `src/main.c` — SDL2 app entry point
- `src/memory.c/h` — CPU memory abstraction
- `src/vdp.c/h` — Graphics rendering
- `src/game.c/h` — Game logic & entities, collision detection, game state
- `tools/extractor.py` — ROM bank splitter
- `tools/analyzer.py` — ROM bank analyzer (finds level data patterns)
- `tools/level_extractor.py` — Converts ROM level data to C arrays
- `CMakeLists.txt` — Build config
- `assets/banks/` — 16 extracted ROM banks (128KB total)
- `assets/banks/level_rom_0x0F.c` — Extracted level data

## How to Continue Development

### Next Session Checklist
1. Modify `analyzer.py` to find all 6 levels in other banks
2. Run `level_extractor.py` on each bank to create level arrays
3. Create a `levels.h` file with all 6 level maps
4. Add level cycling logic to `game.c` (e.g., press Space to go to next level)
5. Extract sprite graphics from banks 0x08-0x0C and render them instead of solid colors

### Architecture Notes
- Memory abstraction (`memory.c`) cleanly separates Z80 address space from C
- VDP abstraction (`vdp.c`) handles MSX2 Screen 5 rendering to SDL2
- Game logic (`game.c`) is independent of graphics/memory details
- This architecture makes it easy to swap renderers or add features