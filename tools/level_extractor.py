#!/usr/bin/env python3
"""
Metal Gear Level Extractor
Converts raw ROM tilemap data into C code suitable for our game engine.
"""

import os
import struct

def extract_level_from_bank(banks_dir, bank_num, width=16, height=20):
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
    print(f"Tile ID range: {min(tilemap):02X} - {max(tilemap):02X}\n")

    # Output as C code
    c_code = f"""// Level data extracted from Bank {bank_num:02X}
// Dimensions: {width}x{height} tiles
// Total tiles: {len(tilemap)}

static TileType rom_level_map[{height}][{width}] = {{
"""

    for y in range(height):
        c_code += "    { "
        row = tilemap[y*width:(y+1)*width]

        # Convert tile IDs to our game tile types
        # Tile 0x30 seems to be floor, others are walls
        game_tiles = []
        for tile_id in row:
            if tile_id == 0x30 or tile_id == 0x31:
                game_tiles.append("TILE_FLOOR")
            else:
                game_tiles.append("TILE_WALL")

        c_code += ", ".join(game_tiles)
        c_code += " },\n"

    c_code += "};\n"

    return c_code

if __name__ == "__main__":
    BANKS_DIR = r"C:\Users\Surya\metal-gear-pc\assets\banks"

    # Extract level from Bank 0x0F
    level_code = extract_level_from_bank(BANKS_DIR, 0x0F, width=16, height=20)

    # Save to a file
    output_path = os.path.join(BANKS_DIR, "level_rom_0x0F.c")
    with open(output_path, 'w') as f:
        f.write(level_code)

    print(f"Level extracted and saved to: {output_path}\n")
    print("First 5 rows of extracted level:")
    print(level_code.split("TILE_")[1:6])  # Print first few tiles