#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdbool.h>

// Initialize the memory system and load the 16 extracted ROM banks
bool mem_init(const char* banks_dir);

// Cleanup memory
void mem_free(void);

// Read a byte from the simulated Z80 address space
uint8_t mem_read(uint16_t address);

// Write a byte to the simulated Z80 address space (handles Konami bank switching)
void mem_write(uint16_t address, uint8_t value);

// Expose the raw work RAM (0xC000 - 0xFFFF) for direct manipulation if needed
uint8_t* mem_get_ram_pointer(uint16_t address);

#endif // MEMORY_H