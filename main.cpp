#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <string>

using namespace std;

// Consts
const int SCREEN_W = 600;
const int SCREEN_H = 800;

/**
 * This is what the player will be controlling to shoot skulls
 */
class Slingshot
{
public:
    Vector2 position = {SCREEN_W / 2, SCREEN_H - 100};
    float aimAngle = -PI / 2; // Point upwards
    const float CLAMP_ANGLE = 10;
    const float AIM_SPEED = 0.03f;
    const float MIN_ANGLE = -PI + (CLAMP_ANGLE * PI / 180); // -PI + 10deg
    const float MAX_ANGLE = -(CLAMP_ANGLE * PI / 180);      // -10deg

    void Update()
    {
        // Aim and clamp aim angle to 10 degrees from horizon
        if (IsKeyDown(KEY_LEFT))
        {
            aimAngle -= AIM_SPEED;
            if (aimAngle < MIN_ANGLE)
                aimAngle = MIN_ANGLE;
        }
        else if (IsKeyDown(KEY_RIGHT))
        {
            aimAngle += AIM_SPEED;
            if (aimAngle > MAX_ANGLE)
                aimAngle = MAX_ANGLE;
        }
    }

    void Draw()
    {
        Vector2 target = GetAimTarget();
        DrawCircle(position.x, position.y, 20, DARKBLUE);  // base of the slingshot (replace with texture later)
        DrawLine(position.x - 1, position.y, target.x, target.y, DARKBLUE);
    }

    // Returns the endpoint of the aim line
    Vector2 GetAimTarget()
    {
        Vector2 target = position;
        target.x += cos(aimAngle) * SCREEN_W / 2;
        target.y += sin(aimAngle) * SCREEN_W / 2;
        target.x = Clamp(target.x, 0, SCREEN_W);
        return target;
    }
};

class Skull
{
};

/**
 * This is a static skull that will not move
 */
class StaticSkull : public Skull
{
};

/**
 * This is a skull that is shot by the player
 */
class ActiveSkull : public Skull
{
};

/**
 * Spawns skulls
 */
class Spawner
{
};

int main()
{
    InitWindow(SCREEN_W, SCREEN_H, "Strike-A-Pose (but cooler)");

    SetTargetFPS(60);

    Slingshot slingshot;

    while (!WindowShouldClose())
    {
        // Update
        slingshot.Update();

        if (IsKeyPressed(KEY_SPACE))
        {
            // Shoot skull
        }

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText(("angle: " + to_string(slingshot.aimAngle)).c_str(), 10, 10, 20, BLACK);
        slingshot.Draw();

        // DrawText("Press [SPACE] to shoot!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}