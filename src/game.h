#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <stdbool.h>

// Simple tile enum for our test level
typedef enum {
    TILE_EMPTY = 0,
    TILE_WALL = 1,
    TILE_FLOOR = 2,
} TileType;

// Game state
typedef enum {
    GAME_PLAYING = 0,
    GAME_OVER = 1,
} GameState;

// Player/Snake state
typedef struct {
    uint16_t x;        // Pixel X position
    uint16_t y;        // Pixel Y position
    uint8_t width;     // 16 pixels
    uint8_t height;    // 16 pixels
    uint8_t direction; // 0=right, 1=down, 2=left, 3=up
} Player;

// Guard/Enemy state
typedef struct {
    uint16_t x;        // Pixel X position
    uint16_t y;        // Pixel Y position
    uint8_t width;     // 16 pixels
    uint8_t height;    // 16 pixels
    uint8_t direction; // 0=right, 2=left
    int patrol_left;   // Patrol boundary left
    int patrol_right;  // Patrol boundary right
    int move_counter;  // Counter for movement timing
} Guard;

// Initialize the game (creates the test level)
void game_init(void);

// Update game logic (called once per frame)
void game_update(void);

// Handle input
void game_input_left(void);
void game_input_right(void);
void game_input_up(void);
void game_input_down(void);

// Render game to VRAM
void game_render(void);

// Get player position (for debugging)
Player* game_get_player(void);

// Get game state
GameState game_get_state(void);

// Reset game (for when you die)
void game_reset(void);

#endif // GAME_H