#ifndef CONSTS_H
#define CONSTS_H

// Consts
const int SCREEN_W = 600;
const int SCREEN_H = 800;

// Game constants
const int MAX_SKULLS_PER_ROW = 10;
const int SKULL_RADIUS = 16;
const int SKULL_DIAMETER = SKULL_RADIUS * 2;

const int PLAY_AREA_WIDTH = SKULL_DIAMETER * MAX_SKULLS_PER_ROW;
const int WALL_WIDTH = SCREEN_W / 2 - PLAY_AREA_WIDTH / 2;

// Skull texture
const int TILE_SIZE = 32;




#endif // CONSTS_H