#ifndef SPRITES_H
#define SPRITES_H

#include <stdint.h>

// Forward declaration of sprite data (generated)
extern const uint8_t SprSnakeDown[];
extern const uint8_t SprSnakeDown1[];
extern const uint8_t SprSnakeDown2[];
extern const uint8_t SprSnakeLeft[];
extern const uint8_t SprSnakeLeft1[];
extern const uint8_t SprSnakeLeft2[];
extern const uint8_t SprSnakeUp[];
extern const uint8_t SprSnakeUp1[];
extern const uint8_t SprSnakeUp2[];
extern const uint8_t SprSnakeRight[];
extern const uint8_t SprSnakeRight1[];
extern const uint8_t SprSnakeRight2[];

// Sprite rendering function
// x, y: position in pixels
// sprite_data: pointer to sprite byte array
// width, height: sprite dimensions in pixels
// vram: pointer to VDP VRAM
void sprite_render(uint8_t *vram, uint16_t x, uint16_t y,
                   const uint8_t *sprite_data,
                   uint16_t width, uint16_t height);

// Get sprite frame based on direction and animation frame
const uint8_t* sprite_get_frame(uint8_t direction, uint8_t anim_frame);

#endif // SPRITES_H