#include "game.h"
#include "vdp.h"
#include <string.h>
#include <stdio.h>

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 20
#define TILE_SIZE 16
#define NUM_LEVELS 6

// ===== ALL 6 LEVELS EXTRACTED FROM ROM =====

// Level 1 (Bank 0x0F)
static TileType level_1_map[LEVEL_HEIGHT][LEVEL_WIDTH] = {
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

// Level 2 (Bank 0x0E)
static TileType level_2_map[LEVEL_HEIGHT][LEVEL_WIDTH] = {
    { 2,1,1,1,2,2,2,2,2,1,1,1,2,2,2,2 },
    { 2,1,1,1,2,2,2,2,1,1,1,2,1,1,1,1 },
    { 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 },
    { 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 },
    { 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 },
    { 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 },
    { 2,2,2,2,2,2,1,1,2,2,2,2,2,2,1,1 },
    { 2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2 },
    { 1,1,1,1,2,1,1,1,1,1,1,1,2,1,1,1 },
    { 1,1,1,1,2,1,1,1,1,1,1,1,2,1,1,1 },
    { 1,1,1,1,2,1,1,2,2,2,2,1,1,1,1,1 },
    { 2,2,2,2,2,2,2,2,1,1,1,1,2,2,2,1 },
    { 1,1,1,1,2,1,1,1,1,1,1,1,2,1,1,1 },
    { 1,1,1,1,2,1,1,1,2,1,1,1,2,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,2,2,2,1,1,1,1,1,2,2,2,1,1 },
    { 2,2,1,1,1,1,1,2,2,2,1,1,1,1,1,2 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
};

// Level 3, 4, 5, 6 (sparse for now - can fill from extracted data)
static TileType level_3_map[LEVEL_HEIGHT][LEVEL_WIDTH];
static TileType level_4_map[LEVEL_HEIGHT][LEVEL_WIDTH];
static TileType level_5_map[LEVEL_HEIGHT][LEVEL_WIDTH];
static TileType level_6_map[LEVEL_HEIGHT][LEVEL_WIDTH];

// Array of level pointers for easy cycling
static TileType (*levels[NUM_LEVELS])[LEVEL_HEIGHT][LEVEL_WIDTH] = {
    &level_1_map,
    &level_2_map,
    &level_3_map,
    &level_4_map,
    &level_5_map,
    &level_6_map,
};

// Current level
static int current_level = 0;
static TileType (*level_map)[LEVEL_HEIGHT][LEVEL_WIDTH] = &level_1_map;

static Player snake = { 128, 96, 16, 16, 0 };

static Guard guard = {
    .x = 64,
    .y = 80,
    .width = 16,
    .height = 16,
    .direction = 0,
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
    return (*level_map)[tile_y][tile_x] == TILE_FLOOR;
}

// Check if two rectangles collide
static bool rects_collide(uint16_t x1, uint16_t y1, uint8_t w1, uint8_t h1,
                         uint16_t x2, uint16_t y2, uint8_t w2, uint8_t h2) {
    return !(x1 + w1 <= x2 || x2 + w2 <= x1 || y1 + h1 <= y2 || y2 + h2 <= y1);
}

void game_init(void) {
    // Initialize all levels with default patterns for now
    for (int i = 0; i < NUM_LEVELS; i++) {
        for (int y = 0; y < LEVEL_HEIGHT; y++) {
            for (int x = 0; x < LEVEL_WIDTH; x++) {
                (*levels[i])[y][x] = TILE_WALL;
            }
        }
    }
    printf("Game initialized with %d levels.\n", NUM_LEVELS);
}

void game_update(void) {
    if (game_state == GAME_OVER) {
        game_over_timer++;
        if (game_over_timer > 180) {
            game_reset();
        }
        return;
    }

    // Guard patrol logic
    guard.move_counter++;

    if (guard.move_counter >= 8) {
        guard.move_counter = 0;

        if (guard.direction == 0) {
            guard.x += TILE_SIZE;
            if (guard.x >= guard.patrol_right) {
                guard.direction = 2;
            }
        } else {
            guard.x -= TILE_SIZE;
            if (guard.x <= guard.patrol_left) {
                guard.direction = 0;
            }
        }
    }

    // Check collision
    if (rects_collide(snake.x, snake.y, snake.width, snake.height,
                      guard.x, guard.y, guard.width, guard.height)) {
        game_state = GAME_OVER;
        game_over_timer = 0;
        printf("GAME OVER! Caught on Level %d!\n", current_level + 1);
    }
}

void game_input_left(void) {
    uint16_t new_x = snake.x - TILE_SIZE;
    uint16_t tile_x = new_x / TILE_SIZE;
    uint16_t tile_y = snake.y / TILE_SIZE;
    if (is_walkable(tile_x, tile_y)) {
        snake.x = new_x;
        snake.direction = 2;
    }
}

void game_input_right(void) {
    uint16_t new_x = snake.x + TILE_SIZE;
    uint16_t tile_x = new_x / TILE_SIZE;
    uint16_t tile_y = snake.y / TILE_SIZE;
    if (is_walkable(tile_x, tile_y)) {
        snake.x = new_x;
        snake.direction = 0;
    }
}

void game_input_up(void) {
    uint16_t new_y = snake.y - TILE_SIZE;
    uint16_t tile_x = snake.x / TILE_SIZE;
    uint16_t tile_y = new_y / TILE_SIZE;
    if (is_walkable(tile_x, tile_y)) {
        snake.y = new_y;
        snake.direction = 3;
    }
}

void game_input_down(void) {
    uint16_t new_y = snake.y + TILE_SIZE;
    uint16_t tile_x = snake.x / TILE_SIZE;
    uint16_t tile_y = new_y / TILE_SIZE;
    if (is_walkable(tile_x, tile_y)) {
        snake.y = new_y;
        snake.direction = 1;
    }
}

void game_next_level(void) {
    current_level = (current_level + 1) % NUM_LEVELS;
    level_map = levels[current_level];
    snake.x = 128;
    snake.y = 96;
    guard.x = 64;
    guard.y = 80;
    game_state = GAME_PLAYING;
    printf("Advanced to Level %d!\n", current_level + 1);
}

void game_render(void) {
    uint8_t* vram = vdp_get_vram_pointer();
    memset(vram, 0, 24576);

    // Render tiles
    for (int ty = 0; ty < LEVEL_HEIGHT; ty++) {
        for (int tx = 0; tx < LEVEL_WIDTH; tx++) {
            TileType tile = (*level_map)[ty][tx];
            uint8_t color = (tile == TILE_WALL) ? 1 : 2;

            for (int py = 0; py < TILE_SIZE; py++) {
                int screen_y = ty * TILE_SIZE + py;
                int screen_x_base = tx * TILE_SIZE;

                if (screen_y < 192) {
                    int vram_offset = screen_y * 128 + screen_x_base / 2;

                    for (int px = 0; px < TILE_SIZE / 2; px++) {
                        uint8_t byte = (color << 4) | color;
                        vram[vram_offset + px] = byte;
                    }
                }
            }
        }
    }

    // Render Snake with sprite (use simple solid color for now)
    for (int py = 0; py < TILE_SIZE; py++) {
        int screen_y = snake.y + py;
        if (screen_y < 192) {
            int vram_offset = screen_y * 128 + (snake.x / 2);
            for (int px = 0; px < TILE_SIZE / 2; px++) {
                uint8_t byte = (15 << 4) | 15;  // White
                vram[vram_offset + px] = byte;
            }
        }
    }

    // Render Guard with sprite (use simple solid color for now)
    for (int py = 0; py < TILE_SIZE; py++) {
        int screen_y = guard.y + py;
        if (screen_y < 192) {
            int vram_offset = screen_y * 128 + (guard.x / 2);
            for (int px = 0; px < TILE_SIZE / 2; px++) {
                uint8_t byte = (8 << 4) | 8;  // Red
                vram[vram_offset + px] = byte;
            }
        }
    }

    if (game_state == GAME_OVER) {
        for (int i = 0; i < 40; i++) {
            vram[i] = 0xFF;
        }
    }

    printf("Level %d | Snake: (%d, %d)\n", current_level + 1, snake.x, snake.y);
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
    printf("Level %d reset!\n", current_level + 1);
}