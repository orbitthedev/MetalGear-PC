#!/usr/bin/env python3
"""
Z80 Disassembly pattern matcher for Metal Gear
Looks for common sprite rendering / VDP writing patterns in Z80 code
"""

import os

def search_for_patterns(banks_dir, bank_num):
    """
    Search for common Z80 patterns that indicate sprite/graphics operations.

    Common patterns:
    - 32 xx (LD A, xx) followed by writes to memory
    - D3 xx (OUT A, xx) - VDP register writes
    - ED 79 (LD (register), A) - writes to memory
    """
    filepath = os.path.join(banks_dir, f"bank_{bank_num:02X}.bin")

    with open(filepath, 'rb') as f:
        data = f.read()

    print(f"\n--- Bank {bank_num:02X} pattern search ---")

    # Look for common VDP/sprite patterns
    # 3E xx = LD A, xx (load immediate)
    # 32 yyyy = LD (yyyy), A (write A to address)

    sprite_patterns = []
    for i in range(0, len(data) - 4):
        # Look for LD A, followed by writes
        if data[i] == 0x3E:  # LD A, imm
            addr_load = i
            # Scan next 20 bytes for pattern
            for j in range(i+2, min(i+20, len(data)-2)):
                if data[j] == 0x32:  # LD (addr), A
                    sprite_patterns.append((addr_load, j, data[i+1]))

    # Print findings
    print(f"Found {len(sprite_patterns)} load-store patterns")
    if sprite_patterns[:5]:
        print("First 5 patterns:")
        for addr_load, addr_store, value in sprite_patterns[:5]:
            print(f"  Offset {addr_load:04X}: LD A, 0x{value:02X}")
            print(f"  Offset {addr_store:04X}: (likely) LD (addr), A")

    # Look for specific graphics-related addresses
    # VDP registers are typically at 0xC0 area in MSX
    print(f"\nSearching for VDP register writes (0xC0-0xDF range)...")
    vdp_writes = 0
    for i in range(0, len(data) - 3):
        if data[i] == 0x32:  # LD (addr), A
            addr_target = (data[i+1] | (data[i+2] << 8))
            if 0xC000 <= addr_target <= 0xC0FF:
                vdp_writes += 1

    print(f"Potential VDP register writes: {vdp_writes}")

    # Look for loops / repetitive patterns (likely sprite rendering)
    print(f"\nSearching for loop constructs (JP / JR instructions)...")
    loops = 0
    for i in range(0, len(data)):
        if data[i] in [0xC2, 0xCA, 0xD2, 0xDA]:  # JP cc, nn
            loops += 1
        elif data[i] in [0x18, 0x20, 0x28, 0x30, 0x38]:  # JR / JR cc
            loops += 1

    print(f"Jump instructions (likely loops): {loops}")

if __name__ == "__main__":
    BANKS_DIR = r"C:\Users\Surya\metal-gear-pc\assets\banks"

    # Analyze code banks for sprite routines
    for bank in [0x00, 0x01, 0x02, 0x03, 0x04]:
        search_for_patterns(BANKS_DIR, bank)