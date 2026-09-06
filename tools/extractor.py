#!/usr/bin/env python3
import os

def extract_konami_banks(rom_path: str, output_dir: str):
    """
    Extracts the 16 8KB banks from the Metal Gear MSX2 MegaROM.
    """
    BANK_SIZE = 8192  # 8 KB per bank
    EXPECTED_SIZE = 131072 # 128 KB

    if not os.path.exists(rom_path):
        print(f"Error: ROM not found at {rom_path}")
        return

    file_size = os.path.getsize(rom_path)
    if file_size != EXPECTED_SIZE:
        print(f"Warning: ROM size is {file_size} bytes. Expected {EXPECTED_SIZE} bytes.")

    if not os.path.exists(output_dir):
        os.makedirs(output_dir, exist_ok=True)

    print(f"Extracting ROM: {rom_path}")

    with open(rom_path, 'rb') as rom_file:
        # The MSX cartridge header starts at bank 0 ('AB' 0x41 0x42)
        for bank_number in range(16):
            bank_data = rom_file.read(BANK_SIZE)
            if not bank_data:
                break

            out_file = os.path.join(output_dir, f"bank_{bank_number:02X}.bin")
            with open(out_file, 'wb') as out_f:
                out_f.write(bank_data)

            print(f"Exported Bank {bank_number:02X} (8KB) -> {out_file}")

    print("Extraction complete!")

if __name__ == "__main__":
    # Absolute paths pointing to the user's desktop ROM and our project assets
    INPUT_ROM_PATH = r"C:\Users\Surya\Desktop\Metal Gear 1 (1987) (Konami)\Metal Gear 1 (1987) (Konami) (J).mx2"
    OUTPUT_ROM_DIR = r"C:\Users\Surya\metal-gear-pc\assets\banks"

    extract_konami_banks(INPUT_ROM_PATH, OUTPUT_ROM_DIR)