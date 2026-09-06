#!/usr/bin/env python3
"""
Extract sprite data from Metal Gear disassembly ASM file
Converts Sjasm format sprite definitions to C arrays
"""

import re

def parse_sprites_asm(filepath):
    """
    Parse sprites.asm and extract sprite definitions
    Format: SprName: db 0xXX, 0xYY, ...
    """
    sprites = {}
    current_sprite = None
    current_data = []

    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()

            # Skip comments and empty lines
            if not line or line.startswith(';'):
                continue

            # Check for sprite label
            if ':' in line and 'Spr' in line:
                # Save previous sprite
                if current_sprite:
                    sprites[current_sprite] = current_data

                # Start new sprite
                current_sprite = line.split(':')[0].strip()
                current_data = []

            # Extract hex values from db statements
            if 'db' in line:
                # Remove "db" and split by commas
                hex_part = line.split('db', 1)[1]
                # Extract all hex values (0xXX or XXh format)
                hex_values = re.findall(r'([0-9A-Fa-f]{1,2})[hH]?', hex_part)
                for val in hex_values:
                    try:
                        current_data.append(int(val, 16))
                    except:
                        pass

        # Save last sprite
        if current_sprite:
            sprites[current_sprite] = current_data

    return sprites

def generate_c_code(sprites):
    """
    Generate C code for sprite data
    """
    c_code = """// Metal Gear Sprites - Extracted from disassembly
// Format: 4-bit pixel data (2 pixels per byte)

#include <stdint.h>

"""

    # Group sprites by type
    snake_sprites = {k: v for k, v in sprites.items() if 'Snake' in k}
    guard_sprites = {k: v for k, v in sprites.items() if 'Guard' in k or 'Actor' in k}

    # Generate Snake sprites
    c_code += "// SNAKE SPRITES\n"
    for name, data in sorted(snake_sprites.items()):
        c_code += f"\nstatic const uint8_t {name}[] = {{\n"
        for i in range(0, len(data), 16):
            chunk = data[i:i+16]
            hex_str = ", ".join(f"0x{b:02X}" for b in chunk)
            c_code += f"    {hex_str},\n"
        c_code += "};\n"

    # Generate Guard sprites (if available)
    if guard_sprites:
        c_code += "\n// GUARD SPRITES\n"
        for name, data in sorted(guard_sprites.items())[:10]:  # Limit to first 10
            c_code += f"\nstatic const uint8_t {name}[] = {{\n"
            for i in range(0, len(data), 16):
                chunk = data[i:i+16]
                hex_str = ", ".join(f"0x{b:02X}" for b in chunk)
                c_code += f"    {hex_str},\n"
            c_code += "};\n"

    # Generate sprite index
    c_code += "\n\n// SPRITE INDEX\n"
    c_code += "typedef struct {\n"
    c_code += "    const char *name;\n"
    c_code += "    const uint8_t *data;\n"
    c_code += "    uint16_t size;\n"
    c_code += "} SpriteDefinition;\n\n"

    c_code += "static const SpriteDefinition sprite_library[] = {\n"
    for name, data in sorted(sprites.items()):
        c_code += f'    {{ "{name}", {name}, sizeof({name}) }},\n'
    c_code += "    { NULL, NULL, 0 }\n"
    c_code += "};\n"

    return c_code

if __name__ == "__main__":
    ASM_PATH = r"C:\Users\Surya\metal-gear-pc\reference\MetalGear-Disassembly\gfx\sprites.asm"
    OUTPUT_PATH = r"C:\Users\Surya\metal-gear-pc\src\sprites_data.c"

    print(f"Parsing {ASM_PATH}...")
    sprites = parse_sprites_asm(ASM_PATH)
    print(f"Found {len(sprites)} sprites")

    # Show first few
    for name in list(sprites.keys())[:5]:
        print(f"  {name}: {len(sprites[name])} bytes")

    print(f"\nGenerating C code...")
    c_code = generate_c_code(sprites)

    with open(OUTPUT_PATH, 'w') as f:
        f.write(c_code)

    print(f"Sprites saved to {OUTPUT_PATH}")