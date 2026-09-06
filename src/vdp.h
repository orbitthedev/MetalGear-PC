#ifndef VDP_H
#define VDP_H

#include <SDL.h>
#include <stdint.h>
#include <stdbool.h>

#define VRAM_SIZE 0x20000 // 128 KB of VRAM

// Initialize the VDP, passing the SDL Renderer so the VDP can manage its own texture
bool vdp_init(SDL_Renderer* renderer);

// Cleanup VDP resources
void vdp_free(void);

// Write a byte to VRAM (how the Z80 CPU normally sends graphics)
void vdp_write_vram(uint32_t address, uint8_t value);

// Read a byte from VRAM
uint8_t vdp_read_vram(uint32_t address);

// Expose raw VRAM pointer for easy loading/hacking
uint8_t* vdp_get_vram_pointer(void);

// Render the current VRAM onto the SDL screen
void vdp_render(SDL_Renderer* renderer);

#endif // VDP_H