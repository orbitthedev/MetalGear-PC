#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BANK_SIZE 8192
#define NUM_BANKS 16

// 64KB Total MSX Address Space arrays
// We only really need RAM for C000-FFFF, but we keep an array for BIOS just in case
static uint8_t msx_bios[0x4000]; // 0x0000 - 0x3FFF
static uint8_t msx_ram[0x4000];  // 0xC000 - 0xFFFF

// The 16 ROM banks we extracted
static uint8_t rom_banks[NUM_BANKS][BANK_SIZE];

// The currently active bank numbers for each of the 4 cartridge pages
// By default, usually pages are mapped 0, 1, 2, 3 on boot, or whatever the MSX bootloader sets.
static int active_bank_page1 = 0; // Maps to 0x4000 - 0x5FFF
static int active_bank_page2 = 0; // Maps to 0x6000 - 0x7FFF
static int active_bank_page3 = 0; // Maps to 0x8000 - 0x9FFF
static int active_bank_page4 = 0; // Maps to 0xA000 - 0xBFFF

bool mem_init(const char* banks_dir) {
    memset(msx_bios, 0, sizeof(msx_bios));
    memset(msx_ram, 0, sizeof(msx_ram));

    // Load the 16 banks from disk
    for (int i = 0; i < NUM_BANKS; i++) {
        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s/bank_%02X.bin", banks_dir, i);

        FILE* f = fopen(filepath, "rb");
        if (!f) {
            printf("Error: Failed to open ROM bank %s\n", filepath);
            return false;
        }

        size_t read_bytes = fread(rom_banks[i], 1, BANK_SIZE, f);
        fclose(f);

        if (read_bytes != BANK_SIZE) {
            printf("Error: Bank %s is incomplete (%zu bytes)\n", filepath, read_bytes);
            return false;
        }
    }

    printf("Memory System Initialized. All 16 ROM banks loaded.\n");
    return true;
}

void mem_free(void) {
    // Nothing dynamic to free yet
}

uint8_t mem_read(uint16_t address) {
    if (address < 0x4000) {
        return msx_bios[address];
    } else if (address < 0x6000) {
        return rom_banks[active_bank_page1][address - 0x4000];
    } else if (address < 0x8000) {
        return rom_banks[active_bank_page2][address - 0x6000];
    } else if (address < 0xA000) {
        return rom_banks[active_bank_page3][address - 0x8000];
    } else if (address < 0xC000) {
        return rom_banks[active_bank_page4][address - 0xA000];
    } else {
        // C000 to FFFF is Work RAM
        return msx_ram[address - 0xC000];
    }
}

void mem_write(uint16_t address, uint8_t value) {
    if (address < 0x4000) {
        // Cannot write to BIOS ROM
        return;
    } else if (address < 0x6000) {
        // Write to Page 1 switches the active ROM bank for Page 1
        active_bank_page1 = value % NUM_BANKS;
        // printf("Bank Switched: Page 1 (0x4000) points to Bank %d\n", active_bank_page1);
    } else if (address < 0x8000) {
        // Write to Page 2 switches the active ROM bank for Page 2
        active_bank_page2 = value % NUM_BANKS;
        // printf("Bank Switched: Page 2 (0x6000) points to Bank %d\n", active_bank_page2);
    } else if (address < 0xA000) {
        // Write to Page 3 switches the active ROM bank for Page 3
        active_bank_page3 = value % NUM_BANKS;
        // printf("Bank Switched: Page 3 (0x8000) points to Bank %d\n", active_bank_page3);
    } else if (address < 0xC000) {
        // Write to Page 4 switches the active ROM bank for Page 4
        active_bank_page4 = value % NUM_BANKS;
        // printf("Bank Switched: Page 4 (0xA000) points to Bank %d\n", active_bank_page4);
    } else {
        // C000 to FFFF is Work RAM, this is where game variables are saved!
        msx_ram[address - 0xC000] = value;
    }
}

uint8_t* mem_get_ram_pointer(uint16_t address) {
    if (address >= 0xC000) {
        return &msx_ram[address - 0xC000];
    }
    return NULL;
}