#ifndef SKULLSMANAGER_H
#define SKULLSMANAGER_H

#include <vector>
#include <cfloat>

#include "objects/skull.h"
#include <raymath.h>


using namespace std;

class Slingshot;  // Again, fuck you forward declarations

class SkullsManager
{
public:
    // I'm gonna assume the width of the rows is like 10 skulls max
    vector<Skull> skulls;

    // storing collided skulls index
    int collidedIndex = -1;

    // score count!!
    int score = 0;

    // Main logic
    vector<int> GetConnectedGroup(int startIndex);
    void CheckPop(int newSkullIndex);
    void Spawn(int level);
    bool CheckCollision(ActiveSkull &activeSkull);
    void SnapSkull(ActiveSkull &activeSkull);

    // Helpers
    SkullColor GetRandomSkullColor();
    void LoadRandomSkull(Slingshot &slingshot); // defined after Slingshot
    void SpawnRow();
    void GoDown();
    void CheckLoseCondition(Slingshot &slingshot);

    // Draw
    void Draw(Texture2D skullTexture);
    void Draw(RenderTexture2D skullTexture);
};

#endif // SKULLSMANAGER_H