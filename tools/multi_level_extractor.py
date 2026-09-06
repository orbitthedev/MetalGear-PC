#!/usr/bin/env python3
"""
Multi-level extractor for Metal Gear
Extracts all findable levels from ROM banks and generates C code
"""

import os
import struct

def extract_level_from_bank(banks_dir, bank_num, width=16, height=20, name=""):
    """
    Extracts a tilemap from a ROM bank and outputs it as a C array.
    """
    filepath = os.path.join(banks_dir, f"bank_{bank_num:02X}.bin")

    with open(filepath, 'rb') as f:
        data = f.read()

    # Extract the first width*height bytes as the tilemap
    tilemap_size = width * height
    tilemap = list(data[:tilemap_size])

    print(f"Extracted {width}x{height} tilemap from Bank {bank_num:02X}")
    print(f"Tile ID range: {min(tilemap):02X} - {max(tilemap):02X}")
    print(f"Most common tile: {max(set(tilemap), key=tilemap.count):02X}\n")

    # Output as C code
    c_code = f"""// Level {name} - extracted from Bank {bank_num:02X}
// Dimensions: {width}x{height} tiles

static TileType level_{bank_num:02X}_map[{height}][{width}] = {{
"""

    for y in range(height):
        c_code += "    { "
        row = tilemap[y*width:(y+1)*width]

        # Heuristic: If a tile ID is repeated 50%+ in the level, it's probably the floor
        # Other IDs are walls
        floor_candidates = [t for t in tilemap if tilemap.count(t) > len(tilemap) * 0.3]
        floor_tile = max(floor_candidates, key=tilemap.count) if floor_candidates else 0x30

        game_tiles = []
        for tile_id in row:
            if tile_id == floor_tile or tile_id == 0x30 or tile_id == 0x31:
                game_tiles.append("TILE_FLOOR")
            else:
                game_tiles.append("TILE_WALL")

        c_code += ", ".join(game_tiles)
        c_code += " },\n"

    c_code += "};\n"

    return c_code

if __name__ == "__main__":
    BANKS_DIR = r"C:\Users\Surya\metal-gear-pc\assets\banks"

    # Try extracting from multiple banks
    levels_to_extract = [
        (0x0E, "Level 2"),
        (0x0D, "Level 3"),
        (0x0C, "Level 4"),
        (0x0B, "Level 5"),
        (0x0A, "Level 6"),
    ]

    all_levels = ""

    for bank_num, name in levels_to_extract:
        try:
            level_code = extract_level_from_bank(BANKS_DIR, bank_num, width=16, height=20, name=name)
            all_levels += level_code + "\n"
        except Exception as e:
            print(f"Failed to extract {name} from Bank {bank_num:02X}: {e}\n")

    # Save all to a file
    output_path = os.path.join(BANKS_DIR, "all_levels.c")
    with open(output_path, 'w') as f:
        f.write(all_levels)

    print(f"All levels saved to: {output_path}")