#include <raylib.h>


/**
 * This is what the player will be controlling to shoot skulls
 */
class Slingshot
{

};


class Skull
{   
};

/**
 * This is a static skull that will not move
 */
class StaticSkull : public Skull
{

};

/**
 * This is a skull that is shot by the player
 */
class ActiveSkull : public Skull
{

};

/**
 * Spawns skulls
 */
class Spawner
{

};


int main()
{
    InitWindow(600, 800, "Strike-A-Pose (but cooler)");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Hello World!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}