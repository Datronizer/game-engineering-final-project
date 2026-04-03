#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <raylib.h>

#include "core/consts.h"
#include "core/skullsManager.h"
#include "objects/slingshot.h"
#include "objects/ceiling.h"

class GameManager
{
public:
    GameManager();
    ~GameManager();

    void Update();
    void Draw();

    void DrawBackground(bool centered = false);

    static Slingshot *m_slingshot;
    static SkullsManager *m_skullsManager;
    static Ceiling *m_ceiling;

private:
    GameState m_gameState;
    Texture2D m_bgTexture;
};

#endif // GAMEMANAGER_H