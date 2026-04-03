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

    PlayMusicStream(music);

    // Game elements
    SkullsManager *skullsManager = new SkullsManager();
    Slingshot *slingshot = new Slingshot(skullsManager);
    Ceiling *ceiling = new Ceiling(skullsManager);

    GameManager *gameManager = new GameManager(skullsManager, slingshot, ceiling);
    gameManager->LoadAssets(&bgTexture, &skullMinion, &skullTexture, &skullRenderTexture, &warningSound, &music);

    while (!WindowShouldClose())
    {
        gameManager->Update();

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
