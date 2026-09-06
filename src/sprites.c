#include "sprites.h"
#include <string.h>

/**
 * Render a sprite to VRAM
 *
 * Sprite format: Each byte contains 2 pixels (4-bit color values)
 * Sprites are stored as raw pixel data, row by row
 *
 * First byte of sprite data is metadata:
 *   - High nibble: width in pixels / 8
 *   - Low nibble: height in pixels / 8
 */
void sprite_render(uint8_t *vram, uint16_t x, uint16_t y,
                   const uint8_t *sprite_data,
                   uint16_t width, uint16_t height) {
    if (!sprite_data || !vram) return;

    // Sprite data format: each row is (width / 2) bytes
    // (since 2 pixels per byte in Screen 5 mode)
    uint16_t sprite_row_bytes = width / 2;

    for (int row = 0; row < height; row++) {
        int screen_y = y + row;

        // Bounds check
        if (screen_y < 0 || screen_y >= 192) continue;

        // Calculate VRAM offset for this row
        // VRAM row = screen_y * 128 bytes (256 pixels / 2)
        uint16_t vram_row_offset = screen_y * 128;

        // Sprite starts at screen position x (in pixels)
        // But VRAM is addressed in bytes (2 pixels per byte)
        uint16_t vram_col_offset = x / 2;

        // Copy sprite row data to VRAM
        const uint8_t *sprite_row_data = sprite_data + (row * sprite_row_bytes);
        uint8_t *vram_row_ptr = vram + vram_row_offset + vram_col_offset;

        // Copy each byte of the sprite row
        for (int col_byte = 0; col_byte < sprite_row_bytes; col_byte++) {
            uint16_t vram_pos = vram_row_offset + vram_col_offset + col_byte;

            // Bounds check for right edge
            if (vram_pos >= 24576) break;  // Max VRAM for Screen 5 (256x192)

            vram[vram_pos] = sprite_row_data[col_byte];
        }
    }
}

/**
 * Simple sprite animation helper
 * Returns the appropriate sprite frame based on direction and animation frame
 */
const uint8_t* sprite_get_frame(uint8_t direction, uint8_t anim_frame) {
    // direction: 0=right, 1=down, 2=left, 3=up
    // anim_frame: 0-2 for walk animation, higher for other actions

    switch (direction) {
        case 0:  // Right
            if (anim_frame == 0) return SprSnakeRight;
            if (anim_frame == 1) return SprSnakeRight1;
            if (anim_frame == 2) return SprSnakeRight2;
            break;

        case 1:  // Down
            if (anim_frame == 0) return SprSnakeDown;
            if (anim_frame == 1) return SprSnakeDown1;
            if (anim_frame == 2) return SprSnakeDown2;
            break;

        case 2:  // Left
            if (anim_frame == 0) return SprSnakeLeft;
            if (anim_frame == 1) return SprSnakeLeft1;
            if (anim_frame == 2) return SprSnakeLeft2;
            break;

        case 3:  // Up
            if (anim_frame == 0) return SprSnakeUp;
            if (anim_frame == 1) return SprSnakeUp1;
            if (anim_frame == 2) return SprSnakeUp2;
            break;
    }

    return SprSnakeDown;  // Default fallback
}