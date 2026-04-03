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

enum GameState { MENU, PLAYING, GAME_OVER };
GameState gameState = MENU; // Starts off in main menu

int main()
{
    InitWindow(SCREEN_W, SCREEN_H, "Break the Skull");

    // Audio device
    InitAudioDevice();
   
    SetTargetFPS(60);

    // Loading music
    Music music = LoadMusicStream("src/assets/SpookyMusic.mp3");

    Sound warningSound = LoadSound("src/assets/Bells.mp3");
    PlayMusicStream(music);

    //Skull Minion Animation
    Texture2D skullMinion = LoadTexture("src/assets/SkeletonEnemy.png");

    int minionFrame = 0; // tracks which frame we're currently on
    float minionTimer = 0.0f; // counts time between frame changes

    // Debug should always start at level 0
    // Normal gameplay starts at 1
    int level = 0;

    // If you beat all 10 levels
    bool gameComplete = false;

    Slingshot slingshot;

    SkullsManager skullsManager;
    Ceiling ceiling; // Default mode
    slingshot.skullsManager = &skullsManager;

    skullsManager.Spawn(level);
    skullsManager.LoadRandomSkull(slingshot); // seeds nextSkullColor
    skullsManager.LoadRandomSkull(slingshot); // activeSkull gets the real first color

    Texture2D skullTexture = LoadTexture("src/assets/skull_jelly_32x32.png");
    Texture2D bgTexture = LoadTexture("src/assets/bg2.png");  // 639 x 360

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

        // Tick the speed bonus timer down each frame while a shot is in progress
        if (skullsManager.timerActive && skullsManager.speedBonusTimer > 0)
            skullsManager.speedBonusTimer -= GetFrameTime();

        // Update
        slingshot.Update();

        if (!skullsManager.isGameOver && !skullsManager.isWin && IsKeyPressed(KEY_SPACE))
        {
            // Every 6 shots, drop the ceiling by 1 row
            if (!slingshot.activeSkull.isFlying)
            {
                // Award speed bonus earned since the last shot, then reset the timer
                if (skullsManager.timerActive && skullsManager.speedBonusTimer > 0){
                    skullsManager.score += (int)(50000.0f * (skullsManager.speedBonusTimer / 60.0f));
                }
    
            skullsManager.speedBonusTimer = 60.0f;
            skullsManager.timerActive = true;
                
                //Every 3 shots (down from 5) drop the ceiling by 1 row
                if (ceiling.shots >= 2)
                {
                    ceiling.shots = 0;
                    ceiling.stage++;
                    skullsManager.stage++; // keep skullsManager in sync with ceiling
                    skullsManager.GoDown();
                }
                else
                {
                    ceiling.shots++;
                }
            }
            slingshot.Shoot(skullsManager);
        }

        //Minion enemy (animation timer)
        minionTimer += GetFrameTime(); // adds how long the last frame took
        if (minionTimer >= 0.25f) {  // every 0.25 seconds 
            minionTimer = 0.0f; // reset the timer
            minionFrame = (minionFrame + 1) % 3; // advance to next frame, loop back after 3
        }

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

    if (gameState == MENU)
    {
        // Draw background!
        DrawTexturePro(bgTexture, Rectangle{0, 0, (float)bgTexture.width, (float)bgTexture.height},
            Rectangle{0, 0, SCREEN_W, SCREEN_H}, Vector2{0, 0}, 0, WHITE);

        // Title
        DrawText("BREAK THE SKULL", SCREEN_W / 2 - MeasureText("BREAK THE SKULL", 40) / 2, SCREEN_H / 2 - 80, 40, RED);

        // Play button 
        Rectangle playBtn = {(float)SCREEN_W / 2 - 80, (float)SCREEN_H / 2, 160, 50};
        DrawRectangleRec(playBtn, DARKGRAY);
        DrawText("PLAY", (int)playBtn.x + 50, (int)playBtn.y + 12, 28, WHITE);

        //When the left mouse button is clicked and the mouse position is inside the play button rectangle, switch the game state to PLAYING
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), playBtn))
            gameState = PLAYING; 

    }  else if (gameState == PLAYING){

        // Center the background
        DrawTexturePro(
            bgTexture, Rectangle{0, 0, (float)bgTexture.width, (float)bgTexture.height},
            Rectangle{0, 0, SCREEN_W, SCREEN_H + SKULL_DIAMETER}, 
            Vector2{0, 0}, 0, WHITE);

        slingshot.Draw(skullTexture);

        // Draw play area
        DrawRectangle(0, 0, WALL_WIDTH, SCREEN_H, BLACK);                                // From left (0) to half play area width, centered horizontally
        DrawRectangle(SCREEN_W / 2 + PLAY_AREA_WIDTH / 2, 0, SCREEN_W, SCREEN_H, BLACK); // From right (SCREEN_W - PLAY_AREA_WIDTH) to right


        // Right minion 
        DrawTexturePro(
            skullMinion,
            Rectangle{(float)(minionFrame * 64) + 64, 3 * 64, -64, 64},
            Rectangle{(float)(SCREEN_W / 2 + PLAY_AREA_WIDTH / 2 - 20), (float)(SCREEN_H - 250), 160, 160},
            Vector2{0, 0}, 0, WHITE
        );

        // Left minion (which is mirrored)
        DrawTexturePro(
            skullMinion,
            Rectangle{(float)(minionFrame * 64), 3 * 64, 64, 64},
            Rectangle{(float)(WALL_WIDTH - 140), (float)(SCREEN_H - 250), 160, 160},
            Vector2{0, 0}, 0, WHITE
        );

        ceiling.Draw();

        if (skullTexture.id != 0)
            skullsManager.Draw(skullTexture);
        else
            skullsManager.Draw(skullRenderTexture);


        // Play warning sound when any skull gets close to the danger line 
        bool nearBottom = false;
        for (Skull &skull : skullsManager.skulls)
        {
            if (skull.position.y > slingshot.position.y - SKULL_RADIUS * 8)
            {
                nearBottom = true;
                break;
            }
        }

        //this plays the warning sound if skulls are near the bottom
        if (nearBottom && !skullsManager.isGameOver && !skullsManager.isWin)
        {
            if (!IsSoundPlaying(warningSound))
                PlaySound(warningSound);
        }
        else
        {
            StopSound(warningSound);
        }

            if (DEBUG)
            {
        DrawText(("angle: " + to_string(slingshot.aimAngle)).c_str(), 10, 10, 20, PURPLE);
        DrawText(("FPS: " + to_string(GetFPS())).c_str(), 10, 30, 20, PURPLE);
        DrawText(("Score: " + to_string(skullsManager.score)).c_str(), 10, 50, 20, PURPLE);
        DrawText(("Shots: " + to_string(ceiling.shots)).c_str(), 10, 70, 20, PURPLE);
        DrawText(("Stage: " + to_string(level + 1)).c_str(), 10, 90, 20, PURPLE);
            }

        // Show the current speed bonus the player would earn if they shot right now
        DrawText(("Speed Bonus: " + to_string(skullsManager.timerActive
        ? (int)(50000.0f * max(0.0f, skullsManager.speedBonusTimer / 60.0f))
        : 0)).c_str(), SCREEN_W - 250, 10, 20, ORANGE);


        // After all drawing, check win/lose
        skullsManager.CheckLoseCondition(slingshot);
        skullsManager.CheckWinCondition();


        if (skullsManager.isGameOver)
            gameState = GAME_OVER;

        if (skullsManager.isWin)
        {

            slingshot.Update();

            // Dark overlay
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 150});
            DrawText("YOU WIN!", SCREEN_W / 2 - MeasureText("YOU WIN!", 60) / 2, SCREEN_H / 2 - 60, 60, GREEN);

            
            // Next level button
            Rectangle nextBtn = {(float)SCREEN_W / 2 - 100, (float)SCREEN_H / 2 + 20, 200, 50};
            DrawRectangleRec(nextBtn, DARKGREEN);
            DrawText("NEXT LEVEL", (int)nextBtn.x + 10, (int)nextBtn.y + 12, 24, WHITE);

        // Click to advance
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), nextBtn))
        {
            level++;

            // Check if next level file exists before loading
            string nextFile = level < 10 
                ? "src/levels/level-0" + to_string(level)
                : "src/levels/level-" + to_string(level);

            FILE* test = fopen(nextFile.c_str(), "r");

            if (test == NULL)
            {
            // No more levels, show a different message instead
            // No more levels so game complete yippee!
            skullsManager.isWin = false;
            skullsManager.isGameOver = false;
            gameComplete = true;
            gameState = GAME_OVER; //reuse game over state for now
            }
            else
            {
                fclose(test);

                //Resets all stats
                skullsManager.skulls.clear();
                skullsManager.score = 0;
                skullsManager.stage = 1;
                skullsManager.isWin = false;
                skullsManager.isGameOver = false;
                skullsManager.speedBonusTimer = 60.0f;
                skullsManager.timerActive = false;
                ceiling.stage = 1;
                ceiling.shots = 0;
                skullsManager.Spawn(level); //Spawns the level
                skullsManager.LoadRandomSkull(slingshot);
                skullsManager.LoadRandomSkull(slingshot);
            }
        }
    }
    //this draws the game over screen
    }else if (gameState == GAME_OVER) {

         //Draw background and dark overlay
        DrawTexturePro(bgTexture, Rectangle{0, 0, (float)bgTexture.width, (float)bgTexture.height},
            Rectangle{0, 0, SCREEN_W, SCREEN_H}, Vector2{0, 0}, 0, WHITE);
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 150}); //dark overlay

        //Game over text
          if (gameComplete)
        {
        DrawText("YOU WON THE GAME!", SCREEN_W / 2 - MeasureText("YOU WON THE GAME!", 40) / 2, SCREEN_H / 2 - 80, 40, GOLD);
        }
    else
        {
        DrawText("GAME OVER", SCREEN_W / 2 - MeasureText("GAME OVER", 60) / 2, SCREEN_H / 2 - 80, 60, RED);
        }

        //Main menu button
        Rectangle menuBtn = {(float)SCREEN_W / 2 - 100, (float)SCREEN_H / 2 + 20, 200, 50};
        DrawRectangleRec(menuBtn, DARKGRAY);
        DrawText("MAIN MENU", (int)menuBtn.x + 18, (int)menuBtn.y + 12, 24, WHITE);

        //If main menu button is clicked, reset everything and go back to menu
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), menuBtn))
        {
            gameComplete = false;
            skullsManager.skulls.clear(); //clear all skulls
            skullsManager.score = 0;
            skullsManager.stage = 1;
            skullsManager.isWin = false;
            skullsManager.isGameOver = false;
            skullsManager.speedBonusTimer = 60.0f;
            skullsManager.timerActive = false;
            ceiling.stage = 1;
            ceiling.shots = 0;
            level = 0;
            skullsManager.Spawn(level);
            skullsManager.LoadRandomSkull(slingshot); //seed next skull
            skullsManager.LoadRandomSkull(slingshot); //load first active skull
            gameState = MENU; //switch back to main menu
        }
    }
        EndDrawing();
    }

    // Cleanup music as well
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
