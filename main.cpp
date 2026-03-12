#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <string>
#include <vector>

using namespace std;

// Consts
const int SCREEN_W = 600;
const int SCREEN_H = 800;

const int SKULL_DIAMETER = 20; // This is also the height of each row

const enum SkullColor {
    SKULL_RED,
    SKULL_GREEN,
    SKULL_BLUE,
    SKULL_YELLOW,
    SKULL_PURPLE,
    SKULL_ORANGE,
    SKULL_WHITE,
    SKULL_BLACK
};

/**
 * This is what the player will be controlling to shoot skulls
 */
class Slingshot
{
public:
    Vector2 position = {SCREEN_W / 2, SCREEN_H - 100};
    float aimAngle = -PI / 2; // Point upwards
    const float CLAMP_ANGLE = 10;
    const float AIM_SPEED = 0.03f;
    const float MIN_ANGLE = -PI + (CLAMP_ANGLE * PI / 180); // -PI + 10deg
    const float MAX_ANGLE = -(CLAMP_ANGLE * PI / 180);      // -10deg

    void Update()
    {
        // Aim and clamp aim angle to 10 degrees from horizon
        if (IsKeyDown(KEY_LEFT))
        {
            aimAngle -= AIM_SPEED;
            if (aimAngle < MIN_ANGLE)
                aimAngle = MIN_ANGLE;
        }
        else if (IsKeyDown(KEY_RIGHT))
        {
            aimAngle += AIM_SPEED;
            if (aimAngle > MAX_ANGLE)
                aimAngle = MAX_ANGLE;
        }
    }

    void Draw()
    {
        Vector2 target = GetAimTarget();
        DrawCircle(position.x, position.y, 20, DARKBLUE); // base of the slingshot (replace with texture later)
        DrawLine(position.x - 1, position.y, target.x, target.y, DARKBLUE);
    }

    // Returns the endpoint of the aim line
    Vector2 GetAimTarget()
    {
        Vector2 target = position;
        target.x += cos(aimAngle) * SCREEN_W / 2;
        target.y += sin(aimAngle) * SCREEN_W / 2;
        target.x = Clamp(target.x, 0, SCREEN_W);
        return target;
    }
};

class Skull
{
public:
    SkullColor color;
    Vector2 position;
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
 * Spawns skulls from the top of the screen
 * Pushes previous rows downwards and spawns new rows on top
 *
 * ONLY available on "Inifinite" mode
 */
class Spawner
{
    // TODO
    // void SpawnRow()
    // {
    //     // Spawn a new row of skulls
    //     for (int i = 0; i < SKULL_DIAMETER; i++)
    //     {
    //         // Spawn a new skull
    //         Skull skull;
    //         skull.color = static_cast<SkullColor>(i);
    //         skull.position.x = SCREEN_W / 2;
    //         skull.position.y = SCREEN_H - (i * SKULL_DIAMETER);

    //         skulls.push_back(skull);
    //     }
    // }
};

/**
 * Compresses the play area, one row at a time
 *
 * Default mode
 */
class Ceiling
{
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

    void Draw()
    {
        // Draw the ceiling
        for (int i = 0; i < SKULL_DIAMETER; i++)
        {
            DrawRectangle(0, SCREEN_H - (i * SKULL_DIAMETER), SCREEN_W, SKULL_DIAMETER, BLACK);
        }
    }
};

class SkullRow
{
public:
    static int rowIndex;
    vector<Skull> skulls;
};

int main()
{
    InitWindow(SCREEN_W, SCREEN_H, "Bust-a-Move (but cooler)");

    SetTargetFPS(60);

    Slingshot slingshot;
    vector<SkullRow> skulls;

    Ceiling ceiling; // Default mode
    Spawner spawner; // Infinite mode

    while (!WindowShouldClose())
    {
        // Update
        slingshot.Update();

        if (IsKeyPressed(KEY_SPACE))
        {
            // Shoot skull
        }

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText(("angle: " + to_string(slingshot.aimAngle)).c_str(), 10, 10, 20, BLACK);
        slingshot.Draw();

        // DrawText("Press [SPACE] to shoot!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}