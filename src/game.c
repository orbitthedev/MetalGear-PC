#include "game.h"
#include "vdp.h"
#include <string.h>
#include <stdio.h>

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 20
#define TILE_SIZE 16

// Real level data extracted from Metal Gear ROM Bank 0x0F
static TileType level_map[LEVEL_HEIGHT][LEVEL_WIDTH] = {
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,2,1,1,1,2,2,2,2,2,2,2,2,2 },
    { 1,1,2,2,1,1,2,2,1,1,2,2,1,1,2,2 },
    { 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 },
    { 2,2,1,1,2,2,1,1,2,2,1,1,2,2,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,2,1,1,1,2,1,1,1,2,1,1,1,2 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
};

static Player snake = { 128, 96, 16, 16, 0 }; // Start in the middle-ish

// Guard patrol the corridor
static Guard guard = {
    .x = 64,
    .y = 80,
    .width = 16,
    .height = 16,
    .direction = 0, // starts going right
    .patrol_left = 32,
    .patrol_right = 224,
    .move_counter = 0
};

static GameState game_state = GAME_PLAYING;
static int game_over_timer = 0;

// Simple collision check: is the tile walkable?
static bool is_walkable(uint16_t tile_x, uint16_t tile_y) {
    if (tile_x >= LEVEL_WIDTH || tile_y >= LEVEL_HEIGHT) {
        return false;
    }
    return level_map[tile_y][tile_x] == TILE_FLOOR;
}

// Check if two rectangles collide
static bool rects_collide(uint16_t x1, uint16_t y1, uint8_t w1, uint8_t h1,
                         uint16_t x2, uint16_t y2, uint8_t w2, uint8_t h2) {
    return !(x1 + w1 <= x2 || x2 + w2 <= x1 || y1 + h1 <= y2 || y2 + h2 <= y1);
}

void game_init(void) {
    printf("Game initialized with test level.\n");
}

void game_update(void) {
    if (game_state == GAME_OVER) {
        // Game over timer: reset after 3 seconds (180 frames at 60 FPS)
        game_over_timer++;
        if (game_over_timer > 180) {
            game_reset();
        }
        return;
    }

    // Guard patrol logic
    guard.move_counter++;

    // Move guard every 8 frames (for a slower patrol speed)
    if (guard.move_counter >= 8) {
        guard.move_counter = 0;

        if (guard.direction == 0) { // Moving right
            guard.x += TILE_SIZE;
            if (guard.x >= guard.patrol_right) {
                guard.direction = 2; // Turn left
            }
        } else { // Moving left
            guard.x -= TILE_SIZE;
            if (guard.x <= guard.patrol_left) {
                guard.direction = 0; // Turn right
            }
        }
    }

    // Check collision between Snake and Guard
    if (rects_collide(snake.x, snake.y, snake.width, snake.height,
                      guard.x, guard.y, guard.width, guard.height)) {
        game_state = GAME_OVER;
        game_over_timer = 0;
        printf("GAME OVER! Guard caught you!\n");
    }
}

void game_input_left(void) {
    uint16_t new_x = snake.x - TILE_SIZE;
    uint16_t tile_x = new_x / TILE_SIZE;
    uint16_t tile_y = snake.y / TILE_SIZE;
    if (is_walkable(tile_x, tile_y)) {
        snake.x = new_x;
        snake.direction = 2; // left
    }
}

void game_input_right(void) {
    uint16_t new_x = snake.x + TILE_SIZE;
    uint16_t tile_x = new_x / TILE_SIZE;
    uint16_t tile_y = snake.y / TILE_SIZE;
    if (is_walkable(tile_x, tile_y)) {
        snake.x = new_x;
        snake.direction = 0; // right
    }
}

void game_input_up(void) {
    uint16_t new_y = snake.y - TILE_SIZE;
    uint16_t tile_x = snake.x / TILE_SIZE;
    uint16_t tile_y = new_y / TILE_SIZE;
    if (is_walkable(tile_x, tile_y)) {
        snake.y = new_y;
        snake.direction = 3; // up
    }
}

void game_input_down(void) {
    uint16_t new_y = snake.y + TILE_SIZE;
    uint16_t tile_x = snake.x / TILE_SIZE;
    uint16_t tile_y = new_y / TILE_SIZE;
    if (is_walkable(tile_x, tile_y)) {
        snake.y = new_y;
        snake.direction = 1; // down
    }
}

void game_render(void) {
    uint8_t* vram = vdp_get_vram_pointer();
    memset(vram, 0, 24576); // Clear VRAM (Screen 5 is 24KB)

    // Render tiles
    for (int ty = 0; ty < LEVEL_HEIGHT; ty++) {
        for (int tx = 0; tx < LEVEL_WIDTH; tx++) {
            TileType tile = level_map[ty][tx];
            uint8_t color = (tile == TILE_WALL) ? 1 : 2; // 1=black wall, 2=green floor

            // Each tile is 16x16 pixels = 8x16 bytes in Screen 5 (2 pixels per byte)
            for (int py = 0; py < TILE_SIZE; py++) {
                int screen_y = ty * TILE_SIZE + py;
                int screen_x_base = tx * TILE_SIZE;

                if (screen_y < 192) {
                    // In Screen 5, each row is 128 bytes (256 pixels / 2)
                    int vram_offset = screen_y * 128 + screen_x_base / 2;

                    for (int px = 0; px < TILE_SIZE / 2; px++) {
                        // One byte = 2 pixels, both the same color for a solid tile
                        uint8_t byte = (color << 4) | color;
                        vram[vram_offset + px] = byte;
                    }
                }
            }
        }
    }

    // Render Snake as a white square
    for (int py = 0; py < TILE_SIZE; py++) {
        int screen_y = snake.y + py;
        if (screen_y < 192) {
            int vram_offset = screen_y * 128 + (snake.x / 2);
            for (int px = 0; px < TILE_SIZE / 2; px++) {
                // Color 15 = white
                uint8_t byte = (15 << 4) | 15;
                vram[vram_offset + px] = byte;
            }
        }
    }

    // Render Guard as a red square
    for (int py = 0; py < TILE_SIZE; py++) {
        int screen_y = guard.y + py;
        if (screen_y < 192) {
            int vram_offset = screen_y * 128 + (guard.x / 2);
            for (int px = 0; px < TILE_SIZE / 2; px++) {
                // Color 8 = medium red
                uint8_t byte = (8 << 4) | 8;
                vram[vram_offset + px] = byte;
            }
        }
    }

    // Display "Game Over" message if dead (simple hack: white pixels in top-left)
    if (game_state == GAME_OVER) {
        // Draw a simple "X" pattern to indicate game over
        for (int i = 0; i < 40; i++) {
            vram[i] = 0xFF;  // White pixels
        }
        printf("Game Over! Resetting in 3 seconds...\n");
    }

    printf("Snake at: (%d, %d)\n", snake.x, snake.y);
}

Player* game_get_player(void) {
    return &snake;
}

GameState game_get_state(void) {
    return game_state;
}

void game_reset(void) {
    game_state = GAME_PLAYING;
    game_over_timer = 0;
    snake.x = 128;
    snake.y = 96;
    guard.x = 64;
    guard.y = 80;
    printf("Game Reset!\n");
}