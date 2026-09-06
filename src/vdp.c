#include "vdp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t vram[VRAM_SIZE];
static SDL_Texture* vdp_texture = NULL;

// The default MSX2 16-color palette (RGB format)
// Format: 0xRRGGBB
static uint32_t msx2_palette[16] = {
    0x000000, // 0: Transparent/Black
    0x000000, // 1: Black
    0x20C020, // 2: Medium Green
    0x60E060, // 3: Light Green
    0x2020E0, // 4: Dark Blue
    0x4060E0, // 5: Light Blue
    0xA02020, // 6: Dark Red
    0x40C0E0, // 7: Cyan
    0xE02020, // 8: Medium Red
    0xE06060, // 9: Light Red
    0xC0C020, // 10: Dark Yellow
    0xC0C080, // 11: Light Yellow
    0x208020, // 12: Dark Green
    0xC040A0, // 13: Magenta
    0xA0A0A0, // 14: Gray
    0xE0E0E0  // 15: White
};

bool vdp_init(SDL_Renderer* renderer) {
    memset(vram, 0, VRAM_SIZE);

    // Create a texture that matches the MSX2 resolution (256x192)
    // We use SDL_PIXELFORMAT_ARGB8888 so we can write raw 32-bit RGB values directly
    vdp_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        256, 192
    );

    if (!vdp_texture) {
        printf("Failed to create VDP texture: %s\n", SDL_GetError());
        return false;
    }

    printf("VDP System Initialized.\n");
    return true;
}

void vdp_free(void) {
    if (vdp_texture) {
        SDL_DestroyTexture(vdp_texture);
        vdp_texture = NULL;
    }
}

void vdp_write_vram(uint32_t address, uint8_t value) {
    if (address < VRAM_SIZE) {
        vram[address] = value;
    }
}

uint8_t vdp_read_vram(uint32_t address) {
    if (address < VRAM_SIZE) {
        return vram[address];
    }
    return 0;
}

uint8_t* vdp_get_vram_pointer(void) {
    return vram;
}

void vdp_render(SDL_Renderer* renderer) {
    // Lock the texture so we can manipulate its raw pixel data
    void* pixels = NULL;
    int pitch = 0;

    if (SDL_LockTexture(vdp_texture, NULL, &pixels, &pitch) != 0) {
        return; // Failed to lock
    }

    uint32_t* dst_pixels = (uint32_t*)pixels;

    // Simulate MSX2 Screen 5 rendering
    // Screen 5 is 256x192.
    // 2 pixels per byte = 128 bytes per horizontal line.
    // So 192 lines * 128 bytes = 24,576 bytes representing the screen.

    int vram_index = 0;

    for (int y = 0; y < 192; y++) {
        for (int x = 0; x < 256; x += 2) { // x goes up by 2 because 1 byte = 2 pixels
            uint8_t byte = vram[vram_index++];

            // Extract the two pixels (High nibble and Low nibble)
            uint8_t left_pixel_color = (byte >> 4) & 0x0F;
            uint8_t right_pixel_color = byte & 0x0F;

            // Look up the RGB values in our palette
            // We force the alpha channel to 255 (0xFF000000) so it's opaque
            uint32_t left_rgb = 0xFF000000 | msx2_palette[left_pixel_color];
            uint32_t right_rgb = 0xFF000000 | msx2_palette[right_pixel_color];

            // Write to the SDL Texture buffer
            // (y * 256) gets us to the correct row in a 1D array
            dst_pixels[(y * 256) + x] = left_rgb;
            dst_pixels[(y * 256) + x + 1] = right_rgb;
        }
    }

    SDL_UnlockTexture(vdp_texture);

    // Now tell SDL to draw our fully built texture to the screen!
    SDL_RenderCopy(renderer, vdp_texture, NULL, NULL);
}