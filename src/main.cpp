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
#include <core/gameManager.h>

using namespace std;

int main()
{
    // Init Graphics
    InitWindow(SCREEN_W, SCREEN_H, "Break the Skull");
    SetTargetFPS(60);

    // Init Audio
    InitAudioDevice();

    // Load all assets
    // Load Textures
    Texture2D skullMinion = LoadTexture("src/assets/SkeletonEnemy.png");
    Texture2D skullTexture = LoadTexture("src/assets/skull_jelly_32x32.png");
    Texture2D bgTexture = LoadTexture("src/assets/bg2.png"); // 639 x 360
    RenderTexture2D skullRenderTexture;

    // Load Sounds
    Music music = LoadMusicStream("src/assets/SpookyMusic.mp3");
    Sound warningSound = LoadSound("src/assets/Bells.mp3");

    // Skull Minion Animation
    int minionFrame = 0;      // tracks which frame we're currently on
    float minionTimer = 0.0f; // counts time between frame changes

    PlayMusicStream(music);

    // Game elements
    SkullsManager *skullsManager = new SkullsManager();
    Slingshot *slingshot = new Slingshot(skullsManager);
    Ceiling *ceiling = new Ceiling(skullsManager);

    GameManager *gameManager = new GameManager(skullsManager, slingshot, ceiling);
    gameManager->LoadAssets(&bgTexture, &skullMinion, &skullTexture, &skullRenderTexture, &warningSound, &music);

    while (!WindowShouldClose())
    {
        // Play music
        UpdateMusicStream(music);

        // Tick the speed bonus timer down each frame while a shot is in progress
        if (skullsManager->timerActive && skullsManager->speedBonusTimer > 0)
            skullsManager->speedBonusTimer -= GetFrameTime();

        // Update
        slingshot->Update();

        if (!skullsManager->isGameOver && !skullsManager->isWin && IsKeyPressed(KEY_SPACE))
        {
            // Every 6 shots, drop the ceiling by 1 row
            if (!slingshot->activeSkull.isFlying)
            {
                // Award speed bonus earned since the last shot, then reset the timer
                if (skullsManager->timerActive && skullsManager->speedBonusTimer > 0)
                {
                    skullsManager->score += (int)(50000.0f * (skullsManager->speedBonusTimer / 60.0f));
                }

                skullsManager->speedBonusTimer = 60.0f;
                skullsManager->timerActive = true;

                // Every 3 shots (down from 5) drop the ceiling by 1 row
                if (ceiling->shots >= 2)
                {
                    ceiling->shots = 0;
                    ceiling->stage++;
                    skullsManager->stage++; // keep skullsManager in sync with ceiling
                    skullsManager->GoDown();
                }
                else
                {
                    ceiling->shots++;
                }
            }
            slingshot->Shoot();
        }

        // Minion enemy (animation timer)
        minionTimer += GetFrameTime(); // adds how long the last frame took
        if (minionTimer >= 0.25f)
        {                                        // every 0.25 seconds
            minionTimer = 0.0f;                  // reset the timer
            minionFrame = (minionFrame + 1) % 3; // advance to next frame, loop back after 3
        }

        // --------- Draw ---------- //
        BeginDrawing();
        gameManager->Draw();
        ClearBackground(BLACK);
        EndDrawing();
    }

    // Cleanup music as well
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
