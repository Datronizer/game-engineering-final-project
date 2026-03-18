#include <raylib.h>
#include <string>

#include "objects/skull.h"

void Skull::Draw(RenderTexture2D skullTexture)
{
    DrawTexture(skullTexture.texture, position.x, position.y, SkullColorToRaylib(color));
}

void Skull::Draw(Texture2D skullTexture)
{
    // Source rectangle (where the texture is taken from)
    Rectangle sourceRectangle = {0, 0, TILE_SIZE, TILE_SIZE};

    // Destination rectangle (where the texture is drawn on screen)
    Rectangle skullRectangle = {position.x, position.y, SKULL_DIAMETER, SKULL_DIAMETER};

    // Origin point for rotation/scaling
    Vector2 origin = {SKULL_RADIUS, SKULL_RADIUS};

    DrawTexturePro(skullTexture, sourceRectangle, skullRectangle, origin, 0, SkullColorToRaylib(color));
}

std::string Skull::ToString()
{
    return "Skull " + std::to_string(color);
}
