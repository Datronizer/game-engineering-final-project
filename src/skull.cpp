#include <raylib.h>

#ifndef SKULL_H
#define SKULL_H


class Skull
{
public:
    SkullColor color;
    Vector2 position;

    void Draw(RenderTexture2D skullTexture);
        void Draw(Texture2D skullTexture);

        string ToString();


    // Default draw if there is no texture (high performance)
    void Draw(RenderTexture2D skullTexture)
    {
        DrawTexture(skullTexture.texture, position.x, position.y, SkullColorToRaylib(color));
    }
    // Prefered draw if there is a texture
    void Draw(Texture2D skullTexture)
    {
        // Source rectangle (where the texture is taken from)
        Rectangle sourceRectangle = {0, 0, TILE_SIZE, TILE_SIZE};

        // Destination rectangle (where the texture is drawn on)
        Rectangle skullRectangle = {position.x, position.y, SKULL_DIAMETER, SKULL_DIAMETER};

        // Origin of the texture (rotation and scale won't be applied)
        Vector2 origin = {SKULL_RADIUS, SKULL_RADIUS};

        // Draw the skull
        DrawTexturePro(skullTexture, sourceRectangle, skullRectangle, origin, 0, SkullColorToRaylib(color));
    }

    /**
     * For debugging: returns the skull's color as a string
     */
    string ToString()
    {
        return "Skull " + to_string(color);
    }
};



#endif SKULL_H