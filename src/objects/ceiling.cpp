#include "objects/ceiling.h"

Ceiling::Ceiling(SkullsManager *skullsManager)
{
    this->skullsManager = skullsManager;
}

void Ceiling::Draw()
{
    DrawRectangle(0, 0, SCREEN_W, stage * SKULL_DIAMETER, BLACK);
}

void Ceiling::GoDown()
{
    
}