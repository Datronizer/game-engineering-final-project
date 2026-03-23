#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <string>
#include <vector>
#include <cfloat>
#include <algorithm>

#include "core/consts.h"
#include "objects/skull.h"
#include <slingshot.h>

using namespace std;

/**
 * Compresses the play area, one row at a time
 *
 * Default mode
 */
class Ceiling
{
public:
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
    SkullsManager *skullsManager;

    int stage = 1; // Each stage pushes the ceiling down by SKULL_DIAMETER
    int shots = 0;

    void Draw()
    {
        DrawRectangle(0, 0, SCREEN_W, stage * SKULL_DIAMETER, BLACK);
    }
};

int main()
{
    InitWindow(SCREEN_W, SCREEN_H, "Bust-a-Move (but cooler)");

    // Audio device
    InitAudioDevice();
   
    SetTargetFPS(60);

    // Loading music
    Music music = LoadMusicStream("src/assets/lol.mp3");
    PlayMusicStream(music);


    // Debug should always start at level 0
    // Normal gameplay starts at 1
    int level = 0;

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
        // Play music
        UpdateMusicStream(music); 

        // Update
        slingshot.Update();
        skullsManager.CheckLoseCondition(slingshot);

        if (IsKeyPressed(KEY_SPACE))
        {
            // Every 6 shots, drop the ceiling by 1 row
            if (!slingshot.activeSkull.isFlying)
            {
                if (ceiling.shots >= 5)
                {
                    ceiling.shots = 0;
                    ceiling.stage++;
                    skullsManager.GoDown();
                }
                else
                {
                    ceiling.shots++;
                }
            }
            slingshot.Shoot(skullsManager);
        }

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        slingshot.Draw(skullTexture);

        // Draw play area
        DrawRectangle(0, 0, WALL_WIDTH, SCREEN_H, BLACK);                                // From left (0) to half play area width, centered horizontally
        DrawRectangle(SCREEN_W / 2 + PLAY_AREA_WIDTH / 2, 0, SCREEN_W, SCREEN_H, BLACK); // From right (SCREEN_W - PLAY_AREA_WIDTH) to right

        ceiling.Draw();

        if (skullTexture.id != 0)
            skullsManager.Draw(skullTexture);
        else
            skullsManager.Draw(skullRenderTexture);

        DrawText(("angle: " + to_string(slingshot.aimAngle)).c_str(), 10, 10, 20, PURPLE);
        DrawText(("FPS: " + to_string(GetFPS())).c_str(), 10, 30, 20, PURPLE);
        DrawText(("Score: " + to_string(skullsManager.score)).c_str(), 10, 50, 20, PURPLE);
        DrawText(("Shots: " + to_string(ceiling.shots)).c_str(), 10, 70, 20, PURPLE);
        DrawText(("Stage: " + to_string(ceiling.stage)).c_str(), 10, 90, 20, PURPLE);

        EndDrawing();
    }

    // Cleanup music as well
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
