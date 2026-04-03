#ifndef SKULL_H
#define SKULL_H

#include <raylib.h>
#include <string>
#include "core/consts.h"

#pragma region SkullColor
// This is in case we're gonna use a texture for the skulls
// For now, we'll just cast this to Raylib colors
enum SkullColor
{
    SKULL_RED,
    SKULL_GREEN,
    SKULL_BLUE,
    SKULL_YELLOW,
    SKULL_PURPLE,
    SKULL_ORANGE,
    SKULL_WHITE,
    SKULL_BLACK,

    SKULL_MAGENTA,
    SKULL_WALL, // lmao funny name
};

inline Color SkullColorToRaylib(SkullColor color)
{
    switch (color)
    {
    case SKULL_RED:
        return RED;
    case SKULL_GREEN:
        return GREEN;
    case SKULL_BLUE:
        return BLUE;
    case SKULL_YELLOW:
        return YELLOW;
    case SKULL_PURPLE:
        return PURPLE;
    case SKULL_ORANGE:
        return ORANGE;
    case SKULL_WHITE:
        return WHITE;
    case SKULL_BLACK:
        return BLACK;
    default:
        return MAGENTA;
    }
}

// Converts a char from the level file to a SkullColor
inline SkullColor ColorCharToSkullColor(char c)
{
    switch (c)
    {
    case 'R':
        return SKULL_RED;
    case 'G':
        return SKULL_GREEN;
    case 'B':
        return SKULL_BLUE;
    case 'Y':
        return SKULL_YELLOW;
    case 'P':
        return SKULL_PURPLE;
    case 'O':
        return SKULL_ORANGE;
    case 'W':
        return SKULL_WHITE;
    case 'K':
        return SKULL_BLACK;
    default:
        return SKULL_MAGENTA; // just so we know something went wrong
    }
}
#pragma endregion

class Skull
{
public:
    SkullColor color;
    Vector2 position;
    Rectangle hitbox = {0, 0, SKULL_DIAMETER, SKULL_DIAMETER};

    void Draw(RenderTexture2D *skullTexture);
    void Draw(Texture2D *skullTexture);

    std::string ToString();
};

/**
 * A wall is a special skull that is not affected by the slingshot
 * It is indestructible
 *
 * This is like League using minions for literally everything (nice spaghetti code buddy)
 */
class Wall : public Skull
{
public:
    SkullColor color = SKULL_WALL;
};

/**
 * This is a skull that is shot by the player
 */
class ActiveSkull : public Skull
{
public:
    Vector2 velocity = {0, 0};
    bool isFlying = false;
};

#endif // SKULL_H
