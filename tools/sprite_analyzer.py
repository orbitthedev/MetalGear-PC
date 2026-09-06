#!/usr/bin/env python3
"""
Metal Gear Sprite/Tile Graphics Extractor
Analyzes graphics banks to find and extract sprite patterns
"""

import os
import struct

def analyze_graphics_bank(banks_dir, bank_num):
    """
    Analyzes a graphics bank looking for tile/sprite patterns.
    Looks for repeating patterns that could be 8x8 or 16x16 sprites.
    """
    filepath = os.path.join(banks_dir, f"bank_{bank_num:02X}.bin")

    with open(filepath, 'rb') as f:
        data = f.read()

    print(f"\n--- Analyzing Bank {bank_num:02X} (Graphics) ---")
    print(f"Total size: {len(data)} bytes")

    # Look for repeating 8-byte chunks (MSX2 Screen 5 uses 2 bytes per pixel row for 8x8 tile)
    # Or 64-byte chunks (8x8 tile in Screen 5 = 8 rows * 8 bytes/row)

    chunks_64 = {}
    for i in range(0, len(data) - 64, 64):
        chunk = data[i:i+64]
        if chunk in chunks_64:
            chunks_64[chunk] += 1
        else:
            chunks_64[chunk] = 1

    # Find most common 64-byte chunks (likely repeating sprites)
    common_chunks = sorted(chunks_64.items(), key=lambda x: x[1], reverse=True)[:5]

    print(f"Most common 64-byte patterns (likely 8x8 tiles):")
    for i, (chunk, count) in enumerate(common_chunks):
        print(f"  Pattern {i}: appears {count} times")
        print(f"    First 32 bytes: {chunk[:32].hex()}")

    # Look for headers or markers
    print(f"\nFirst 256 bytes (hex):")
    hex_str = data[:256].hex()
    for i in range(0, len(hex_str), 32):
        print(f"  {hex_str[i:i+32]}")

if __name__ == "__main__":
    BANKS_DIR = r"C:\Users\Surya\metal-gear-pc\assets\banks"

    # Analyze graphics banks
    for bank in [0x08, 0x09, 0x0A, 0x0B]:
        analyze_graphics_bank(BANKS_DIR, bank)