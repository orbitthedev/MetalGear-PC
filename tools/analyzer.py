#!/usr/bin/env python3
"""
Metal Gear MSX ROM Analyzer
Scans extracted ROM banks to find level data and extract tilemaps.

Metal Gear stores levels as:
- Tilemap: Array of tile IDs (typically 1 byte per tile)
- Width/Height: Dimensions of the level
- Tile data: Graphics for each tile type

This tool searches for recognizable patterns and extracts levels.
"""

import struct
import os

def analyze_rom_banks(banks_dir):
    """
    Scans all 16 ROM banks for level data patterns.
    Looks for:
    1. Repeating patterns (common in tilemaps)
    2. Reasonable dimensions (Metal Gear levels are roughly 256x192 pixels = 16x12 tiles)
    """

    print("=== Metal Gear ROM Bank Analyzer ===\n")

    bank_data = {}
    for i in range(16):
        filepath = os.path.join(banks_dir, f"bank_{i:02X}.bin")
        if os.path.exists(filepath):
            with open(filepath, 'rb') as f:
                bank_data[i] = f.read()

    # Analyze each bank
    for bank_num, data in bank_data.items():
        print(f"\n--- Bank {bank_num:02X} (8192 bytes) ---")

        # Look for runs of repeated values (common in tilemaps)
        max_run_length = 0
        max_run_value = 0
        current_run = 1
        current_value = data[0]

        for i in range(1, len(data)):
            if data[i] == current_value:
                current_run += 1
            else:
                if current_run > max_run_length:
                    max_run_length = current_run
                    max_run_value = current_value
                current_value = data[i]
                current_run = 1

        print(f"  Longest repeated byte: 0x{max_run_value:02X} x {max_run_length}")

        # Look for potential tilemap dimensions
        # Common level sizes: 16x12 (192 tiles), 20x12 (240 tiles), etc.
        possible_sizes = [192, 240, 256, 320]  # tile counts

        for size in possible_sizes:
            if size <= len(data):
                # Try to detect if there are tile patterns
                # (Look for values in range 0-30, which is typical for small tilesets)
                valid_tiles = sum(1 for byte in data[:size] if 0 <= byte < 64)
                tile_density = valid_tiles / size

                if tile_density > 0.8:  # If 80%+ of bytes look like tile IDs
                    width = 16
                    height = size // width
                    print(f"  Potential tilemap: {width}x{height} at offset 0 (density: {tile_density:.1%})")

        # Print first 64 bytes as hex (often header/metadata)
        print(f"  First 64 bytes (hex):")
        hex_str = " ".join(f"{b:02X}" for b in data[:64])
        for i in range(0, len(hex_str), 48):
            print(f"    {hex_str[i:i+48]}")

def extract_level_attempts(banks_dir):
    """
    Attempts to extract level data by trying different offsets and interpreting
    bytes as tilemap data.
    """
    print("\n\n=== Attempting Level Extraction ===\n")

    # Load Bank 0 and 1 (often contain level data in Konami games)
    bank_0 = open(os.path.join(banks_dir, "bank_00.bin"), 'rb').read()

    # Try interpreting first 192 bytes as a 16x12 tilemap
    print("Bank 0, first 192 bytes as 16x12 tilemap:")
    for y in range(12):
        row = " ".join(f"{b:02X}" for b in bank_0[y*16:(y+1)*16])
        print(f"  Row {y:2d}: {row}")

    print("\n(If you see mostly values 00-0F, these are likely tile IDs!)")

if __name__ == "__main__":
    BANKS_DIR = r"C:\Users\Surya\metal-gear-pc\assets\banks"

    analyze_rom_banks(BANKS_DIR)
    extract_level_attempts(BANKS_DIR)