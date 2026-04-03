#include "core/gameManager.h"

GameManager::GameManager(
    SkullsManager *skullsManager,
    Slingshot *slingshot,
    Ceiling *ceiling)
{
    m_gameState = MENU; // Starts off in main menu

    m_skullsManager = skullsManager;
    m_slingshot = slingshot;
    m_ceiling = ceiling;

    m_skullsManager->Spawn(m_level);
    m_skullsManager->LoadRandomSkull(m_slingshot); // seeds nextSkullColor
    m_skullsManager->LoadRandomSkull(m_slingshot); // activeSkull gets the real first color
}

void GameManager::LoadAssets(
    Texture2D *bgTexture,
    Texture2D *skullMinion,
    Texture2D *skullTexture,
    RenderTexture2D *skullRenderTexture,
    Sound *warningSound,
    Music *music)
{
    m_bgTexture = bgTexture;
    m_skullMinion = skullMinion;
    m_skullTexture = skullTexture;
    m_skullRenderTexture = skullRenderTexture;
    m_warningSound = warningSound;
    m_music = music;

    if (m_skullTexture->id != 0)
    {
        // Insane performance hack from Raylib docs (wtf do you mean a texture works better than a circle)
        *skullRenderTexture = LoadRenderTexture(SKULL_RADIUS * 2, SKULL_RADIUS * 2);

        BeginTextureMode(*skullRenderTexture);
        DrawCircle(SKULL_RADIUS, SKULL_RADIUS, SKULL_RADIUS, WHITE);
        DrawCircleLines(SKULL_RADIUS, SKULL_RADIUS, SKULL_RADIUS, BLACK);
        EndTextureMode();
    }
}

void GameManager::DrawBackground(bool centered)
{
    float offset = centered ? 0 : SKULL_DIAMETER;

    // Center the background
    DrawTexturePro(
        *m_bgTexture, Rectangle{0, 0, (float)m_bgTexture->width, (float)m_bgTexture->height},
        Rectangle{0, 0, SCREEN_W, SCREEN_H + offset},
        Vector2{0, 0}, 0, WHITE);
}

/**
 * Draws a minion on the left or right side of the screen.
 * Minions are only cosmetic.
 */
void GameManager::DrawMinion(bool rightSide)
{
    if (rightSide)
    {
        // Right minion
        DrawTexturePro(
            *m_skullMinion,
            Rectangle{(float)(m_minionFrame * 64) + 64, 3 * 64, -64, 64},
            Rectangle{(float)(SCREEN_W / 2 + PLAY_AREA_WIDTH / 2 - 20), (float)(SCREEN_H - 250), 160, 160},
            Vector2{0, 0}, 0, WHITE);
    }
    else
    {
        // Left minion (which is mirrored)
        DrawTexturePro(
            *m_skullMinion,
            Rectangle{(float)(m_minionFrame * 64), 3 * 64, 64, 64},
            Rectangle{(float)(WALL_WIDTH - 140), (float)(SCREEN_H - 250), 160, 160},
            Vector2{0, 0}, 0, WHITE);
    }
}

void GameManager::Update()
{
}

void GameManager::Draw()
{
    if (m_gameState == MENU)
    {
        // Draw background!
        DrawBackground();

        // Title
        DrawText("BREAK THE SKULL", SCREEN_W / 2 - MeasureText("BREAK THE SKULL", 40) / 2, SCREEN_H / 2 - 80, 40, RED);

        // Play button
        Rectangle playBtn = {(float)SCREEN_W / 2 - 80, (float)SCREEN_H / 2, 160, 50};
        DrawRectangleRec(playBtn, DARKGRAY);
        DrawText("PLAY", (int)playBtn.x + 50, (int)playBtn.y + 12, 28, WHITE);

        // When the left mouse button is clicked and the mouse position is inside the play button rectangle, switch the game state to PLAYING
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), playBtn))
            m_gameState = PLAYING;
    }
    else if (m_gameState == PLAYING)
    {
        // Center the background
        DrawBackground(true);

        m_slingshot->Draw(m_skullTexture);

        // Draw play area
        DrawRectangle(0, 0, WALL_WIDTH, SCREEN_H, BLACK);                                // From left (0) to half play area width, centered horizontally
        DrawRectangle(SCREEN_W / 2 + PLAY_AREA_WIDTH / 2, 0, SCREEN_W, SCREEN_H, BLACK); // From right (SCREEN_W - PLAY_AREA_WIDTH) to right

        DrawMinion(true);
        DrawMinion();

        m_ceiling->Draw();

        // Draw skulls, either from texture or fallback renderTexture
        if (m_skullTexture->id != 0)
            m_skullsManager->Draw(m_skullTexture);
        else
            m_skullsManager->Draw(m_skullRenderTexture);

        // Play warning sound when any skull gets close to the danger line
        bool nearBottom = false;
        for (Skull &skull : m_skullsManager->skulls)
        {
            if (skull.position.y > m_slingshot->position.y - SKULL_RADIUS * 8)
            {
                nearBottom = true;
                break;
            }
        }

        // this plays the warning sound if skulls are near the bottom
        if (nearBottom && !m_skullsManager->isGameOver && !m_skullsManager->isWin)
        {
            if (!IsSoundPlaying(*m_warningSound))
                PlaySound(*m_warningSound);
        }
        else
        {
            StopSound(*m_warningSound);
        }

        if (DEBUG)
        {
            DrawText(("angle: " + to_string(m_slingshot->aimAngle)).c_str(), 10, 10, 20, PURPLE);
            DrawText(("FPS: " + to_string(GetFPS())).c_str(), 10, 30, 20, PURPLE);
            DrawText(("Score: " + to_string(m_skullsManager->score)).c_str(), 10, 50, 20, PURPLE);
            DrawText(("Shots: " + to_string(m_ceiling->shots)).c_str(), 10, 70, 20, PURPLE);
            DrawText(("Stage: " + to_string(m_level + 1)).c_str(), 10, 90, 20, PURPLE);
        }

        // Show the current speed bonus the player would earn if they shot right now
        DrawText(("Speed Bonus: " + to_string(m_skullsManager->timerActive
                                                  ? (int)(50000.0f * max(0.0f, m_skullsManager->speedBonusTimer / 60.0f))
                                                  : 0))
                     .c_str(),
                 SCREEN_W - 250, 10, 20, ORANGE);

        // After all drawing, check win/lose
        m_skullsManager->CheckLoseCondition(*m_slingshot);
        m_skullsManager->CheckWinCondition();

        if (m_skullsManager->isGameOver)
            m_gameState = GAME_OVER;

        if (m_skullsManager->isWin)
        {
            m_slingshot->Update();

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
                m_level++;

                // Check if next level file exists before loading
                string nextFile = m_level < 10
                                      ? "src/levels/level-0" + to_string(m_level)
                                      : "src/levels/level-" + to_string(m_level);

                FILE *levelFile = fopen(nextFile.c_str(), "r");

                if (levelFile == NULL)
                {
                    // No more levels, show a different message instead
                    // No more levels so game complete yippee!
                    m_skullsManager->isWin = false;
                    m_skullsManager->isGameOver = false;
                    m_gameComplete = true;
                    m_gameState = GAME_OVER; // reuse game over state for now
                }
                else
                {
                    fclose(levelFile);

                    // Resets all stats
                    m_skullsManager->skulls.clear();
                    m_skullsManager->score = 0;
                    m_skullsManager->stage = 1;
                    m_skullsManager->isWin = false;
                    m_skullsManager->isGameOver = false;
                    m_skullsManager->speedBonusTimer = 60.0f;
                    m_skullsManager->timerActive = false;
                    m_ceiling->stage = 1;
                    m_ceiling->shots = 0;
                    m_skullsManager->Spawn(m_level); // Spawns the level
                    m_skullsManager->LoadRandomSkull(m_slingshot);
                    m_skullsManager->LoadRandomSkull(m_slingshot);
                }
            }
        }
    }
    // this draws the game over screen
    else if (m_gameState == GAME_OVER)
    {

        // Draw background and dark overlay
        DrawTexturePro(*m_bgTexture, Rectangle{0, 0, (float)m_bgTexture->width, (float)m_bgTexture->height},
                       Rectangle{0, 0, SCREEN_W, SCREEN_H}, Vector2{0, 0}, 0, WHITE);
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, {0, 0, 0, 150}); // dark overlay

        // Game over text
        if (m_gameComplete)
        {
            DrawText("YOU WON THE GAME!", SCREEN_W / 2 - MeasureText("YOU WON THE GAME!", 40) / 2, SCREEN_H / 2 - 80, 40, GOLD);
        }
        else
        {
            DrawText("GAME OVER", SCREEN_W / 2 - MeasureText("GAME OVER", 60) / 2, SCREEN_H / 2 - 80, 60, RED);
        }

        // Main menu button
        Rectangle menuBtn = {(float)SCREEN_W / 2 - 100, (float)SCREEN_H / 2 + 20, 200, 50};
        DrawRectangleRec(menuBtn, DARKGRAY);
        DrawText("MAIN MENU", (int)menuBtn.x + 18, (int)menuBtn.y + 12, 24, WHITE);

        // If main menu button is clicked, reset everything and go back to menu
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), menuBtn))
        {
            m_gameComplete = false;
            m_skullsManager->skulls.clear(); // clear all skulls
            m_skullsManager->score = 0;
            m_skullsManager->stage = 1;
            m_skullsManager->isWin = false;
            m_skullsManager->isGameOver = false;
            m_skullsManager->speedBonusTimer = 60.0f;
            m_skullsManager->timerActive = false;
            m_ceiling->stage = 1;
            m_ceiling->shots = 0;
            m_level = 0;
            m_skullsManager->Spawn(m_level);
            m_skullsManager->LoadRandomSkull(m_slingshot); // seed next skull
            m_skullsManager->LoadRandomSkull(m_slingshot); // load first active skull
            m_gameState = MENU;                            // switch back to main menu
        }
    }
}