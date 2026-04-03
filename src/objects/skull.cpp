#include <raylib.h>
#include <string>

#include "objects/skull.h"

using namespace std;

/**
 * Fallback drawing if there is no texture
 */
void Skull::Draw(RenderTexture2D *skullTexture)
{
    if (color == SKULL_WALL)
    {
        DrawRectangle(position.x - SKULL_RADIUS, position.y - SKULL_RADIUS, SKULL_DIAMETER, SKULL_DIAMETER, BROWN);
        DrawRectangleLines(position.x - SKULL_RADIUS, position.y - SKULL_RADIUS, SKULL_DIAMETER, SKULL_DIAMETER, BLACK);
    }
    else
        DrawTexture(skullTexture->texture, position.x - SKULL_RADIUS, position.y - SKULL_RADIUS, SkullColorToRaylib(color));
}

/**
 * Draws the skull using a texture
 */
void Skull::Draw(Texture2D *skullTexture)
{
    // Source rectangle (where the texture is taken from)
    Rectangle sourceRectangle = {0, 0, TILE_SIZE, TILE_SIZE};

    // Destination rectangle (where the texture is drawn on screen)
    Rectangle skullRectangle = {position.x, position.y, SKULL_DIAMETER, SKULL_DIAMETER};

    // Origin point for rotation/scaling
    Vector2 origin = {SKULL_RADIUS, SKULL_RADIUS};

    if (color == SKULL_WALL)
    {
        DrawRectangle(position.x - SKULL_RADIUS, position.y - SKULL_RADIUS, SKULL_DIAMETER, SKULL_DIAMETER, BROWN);
        DrawRectangleLines(position.x - SKULL_RADIUS, position.y - SKULL_RADIUS, SKULL_DIAMETER, SKULL_DIAMETER, BLACK);
    }
    else
        DrawTexturePro(*skullTexture, sourceRectangle, skullRectangle, origin, 0, SkullColorToRaylib(color));
}

string Skull::ToString()
{
    return "Skull " + to_string(color);
}
