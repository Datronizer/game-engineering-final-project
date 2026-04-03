#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <raylib.h>

#include "core/consts.h"
#include "core/skullsManager.h"
#include "objects/slingshot.h"
#include "objects/ceiling.h"

enum GameState
{
    MENU,
    PLAYING,
    GAME_OVER
};

class GameManager
{
public:
    GameManager();
    GameManager(
        SkullsManager *skullsManager, 
        Slingshot *slingshot, 
        Ceiling *ceiling
    );

    void LoadAssets(
        Texture2D *bgTexture, 
        Texture2D *skullMinion, 
        Texture2D *skullTexture, 
        RenderTexture2D *skullRenderTexture, 
        Sound *warningSound, 
        Music *music
    );

    void Update();
    void Draw();

    void DrawBackground(bool centered = false);
    void DrawMinion(bool rightSide = false);

    Slingshot *m_slingshot;
    SkullsManager *m_skullsManager;
    Ceiling *m_ceiling;

private:
    GameState m_gameState;

    // Graphics
    Texture2D *m_bgTexture;
    Texture2D *m_skullMinion;
    Texture2D *m_skullTexture;
    RenderTexture2D *m_skullRenderTexture;
    int m_minionFrame = 0;      // tracks which frame we're currently on
    float m_minionTimer = 0.0f; // counts time between frame changes

    // Sounds
    Sound *m_warningSound;
    Music *m_music;

    // Debug should always start at level 0
    // Normal gameplay starts at 1
    int m_level = DEBUG ? 0 : 1;

    // If you beat all 10 levels
    bool m_gameComplete = false;
};

#endif // GAMEMANAGER_H