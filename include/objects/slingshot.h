#ifndef SLINGSHOT_H
#define SLINGSHOT_H

#include <raylib.h>
#include "core/skullsManager.h"
#include "core/consts.h"

/**
 * This is what the player will be controlling to shoot skulls
 */
class Slingshot
{
public:
// Basic consts
    Vector2 position = {SCREEN_W / 2, SCREEN_H - 100};
    float aimAngle = -PI / 2; // Point upwards
    
    // Logic consts
    const float CLAMP_ANGLE = 10;
    const float AIM_SPEED = 0.03f;
    const float MIN_ANGLE = -PI + (CLAMP_ANGLE * PI / 180); // -PI + 10deg
    const float MAX_ANGLE = -(CLAMP_ANGLE * PI / 180);      // -10deg

    // Skulls consts
    const float SKULL_SPEED = 600.0f;
    SkullColor nextSkullColor = SKULL_RED;

    Vector2 target = {0, 0};

    // Other connected objects
    SkullsManager *skullsManager;
    ActiveSkull activeSkull;

    void Update();
    void Draw(Texture2D &skullTexture);
    Vector2 GetAimTarget();
    void Shoot(SkullsManager &skullsManager);
};

#endif // SLINGSHOT_H