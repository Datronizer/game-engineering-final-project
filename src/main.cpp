#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <string>
#include <vector>
#include <cfloat>
#include <algorithm>

#include "objects/skull.h"
#include <slingshot.h>

using namespace std;


// fuck you forward declarations
void SkullsManager::LoadRandomSkull(Slingshot &slingshot)
{
    if (skulls.empty())
        return; // safety check

    slingshot.activeSkull.color = slingshot.nextSkullColor;
    slingshot.activeSkull.position = slingshot.position;
    slingshot.activeSkull.velocity = {0, 0};
    slingshot.activeSkull.isFlying = false;

    // Pick next preview from any skull in the grid
    slingshot.nextSkullColor = skulls[rand() % skulls.size()].color;
}

/**
 * Compresses the play area, one row at a time
 *
 * Default mode
 */
class Ceiling
{
    // void GoDown()
    // {
    //     // Compress the ceiling
    //     for (int i = 0; i < SKULL_DIAMETER; i++)
    //     {
    //         // Move the row down
    //         for (int j = 0; j < SKULL_DIAMETER; j++)
    //         {
    //             skulls[i * SKULL_DIAMETER + j].position.y -= SKULL_DIAMETER;
    //         }
    //     }
    // }

    void Draw()
    {
        // Draw the ceiling
        for (int i = 0; i < SKULL_RADIUS; i++)
        {
            DrawRectangle(0, SCREEN_H - (i * SKULL_RADIUS), SCREEN_W, SKULL_RADIUS, BLACK);
        }
    }
};

int main()
{
    InitWindow(SCREEN_W, SCREEN_H, "Bust-a-Move (but cooler)");

    SetTargetFPS(60);

    int level = 1;

    Slingshot slingshot;

    SkullsManager skullsManager;
    Ceiling ceiling; // Default mode
    slingshot.skullsManager = &skullsManager;

    skullsManager.Spawn(level);
    skullsManager.LoadRandomSkull(slingshot); // seeds nextSkullColor
    skullsManager.LoadRandomSkull(slingshot); // activeSkull gets the real first color

    Texture2D skullTexture = LoadTexture("src/assets/skull_jelly_32x32.png");

    // Insane performance hack from Raylib docs (wtf do you mean a texture works better than a circle)
    RenderTexture2D skullRenderTexture = LoadRenderTexture(SKULL_RADIUS * 2, SKULL_RADIUS * 2);

    BeginTextureMode(skullRenderTexture);
    DrawCircle(SKULL_RADIUS, SKULL_RADIUS, SKULL_RADIUS, WHITE);
    DrawCircleLines(SKULL_RADIUS, SKULL_RADIUS, SKULL_RADIUS, BLACK);
    EndTextureMode();

    while (!WindowShouldClose())
    {
        // Update
        slingshot.Update();

        if (IsKeyPressed(KEY_SPACE))
        {
            slingshot.Shoot(skullsManager);
        }

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText(("angle: " + to_string(slingshot.aimAngle)).c_str(), 10, 10, 20, BLACK);
        DrawText(("FPS: " + to_string(GetFPS())).c_str(), 10, 30, 20, BLACK);
        DrawText(("Score: " + to_string(skullsManager.score)).c_str(), 10, 50, 20, BLACK);
        slingshot.Draw(skullTexture);

        if (skullTexture.id != 0)
            skullsManager.Draw(skullTexture);
        else
            skullsManager.Draw(skullRenderTexture);

        // DrawText("Press [SPACE] to shoot!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
