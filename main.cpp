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

/**
 * Calculates the line to be drawn that represents the player's aim
 * The line is drawn from the center of the screen to the aim angle
 */
Vector2 calculateLine(float angle)
{
    // Start point
    Vector2 line = {SCREEN_W / 2, SCREEN_H - 100};

    // End point
    line.x += cos(angle) * SCREEN_W / 2;
    line.y += sin(angle) * SCREEN_W / 2;

    // Restrict to -pi/2 to pi/2
    if (line.x < 0)
    {
        line.x = 0;
    }
    else if (line.x > SCREEN_W)
    {
        line.x = SCREEN_W;
    }

    return line;
}

float degToRad(float deg)
{
    return deg * PI / 180;
}

int main()
{
    // TODO: Replace this with a line from middle bottom of screen that draws to where we're aiming
    Vector2 aimReticle = {0, 0};
    float aimAngle = -PI / 2; // Point upwards

    InitWindow(SCREEN_W, SCREEN_H, "Strike-A-Pose (but cooler)");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Update
        if (IsKeyPressed(KEY_SPACE))
        {
            // Shoot skull
            DrawText("pew!", 190, 200, 20, RED);
        }

        Clamp(aimAngle, -PI, 0);
        if (IsKeyDown(KEY_LEFT))
        {
            // Aim left, but clamped to -PI + 10deg
            if (aimAngle < -PI + degToRad(10))
            {
                aimAngle = -PI + degToRad(10);
            }
            aimAngle -= 0.03;  

            // minus bc unit circle starts from the right
        }
        else if (IsKeyDown(KEY_RIGHT))
        {
            // Aim right, but clamped to 0 - 10deg
            if (aimAngle > 0 - degToRad(10))
            {
                aimAngle = -degToRad(10);
            }
            aimAngle += 0.03;
        }

        // Calculate aim
        aimReticle = calculateLine(aimAngle);

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText(("angle: " + to_string(aimAngle)).c_str(), 10, 10, 20, BLACK);
        DrawCircle(SCREEN_W / 2, SCREEN_H - 100, 20, DARKBLUE);
        DrawLine(SCREEN_W / 2 - 1, SCREEN_H - 100, aimReticle.x, aimReticle.y, DARKBLUE);

        // DrawText("Press [SPACE] to shoot!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}