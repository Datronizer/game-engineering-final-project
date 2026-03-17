#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <string>
#include <vector>

using namespace std;

// Consts
const int SCREEN_W = 600;
const int SCREEN_H = 800;

const int TILE_SIZE = 32;
const int SKULL_RADIUS = 16; // This is also half the height of each row, and half the sprite size
const int SKULL_DIAMETER = SKULL_RADIUS * 2;  // Row height

const int MAX_SKULLS_PER_ROW = 10;

// This is in case we're gonna use a texture for the skulls
// For now, we'll just cast this to Raylib colors
enum SkullColor
{
    SKULL_RED,
    SKULL_GREEN,
    SKULL_BLUE,
    SKULL_YELLOW,
    SKULL_PURPLE,
    SKULL_ORANGE,
    SKULL_WHITE,
    SKULL_BLACK,

    SKULL_MAGENTA,
    SKULL_WALL,  // lmao funny name
};

Color SkullColorToRaylib(SkullColor color)
{
    switch (color)
    {
    case SKULL_RED:
        return RED;
    case SKULL_GREEN:
        return GREEN;
    case SKULL_BLUE:
        return BLUE;
    case SKULL_YELLOW:
        return YELLOW;
    case SKULL_PURPLE:
        return PURPLE;
    case SKULL_ORANGE:
        return ORANGE;
    case SKULL_WHITE:
        return WHITE;
    case SKULL_BLACK:
        return BLACK;
    default:
        return MAGENTA;
    }
}

// Converts a char from the level file to a Color
SkullColor ColorCharToSkullColor(char c)
{
    switch (c)
    {
    case 'R':
        return SKULL_RED;
    case 'G':
        return SKULL_GREEN;
    case 'B':
        return SKULL_BLUE;
    case 'Y':
        return SKULL_YELLOW;
    case 'P':
        return SKULL_PURPLE;
    case 'O':
        return SKULL_ORANGE;
    case 'W':
        return SKULL_WHITE;
    case 'K':
        return SKULL_BLACK;
    default:
        return SKULL_MAGENTA; // just so we know something went wrong
    }
}

// Forward declarations (or Slingshot will complain that Skull isn't declared yet)
class Skull;
class SkullsManager;
class Slingshot;




/**
 * A wall is a special skull that is not affected by the slingshot
 * It is indestructible
 *
 * This is like League using minions for literally everything (nice spaghetti code buddy)
 */
class Wall : public Skull
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
    public:
    /**
     * Pops neighboring skulls of the same color and chain the pop to the next skull
     */
    void Pop()
    {

    }
};

class SkullsManager
{
public:
    // I'm gonna assume the width of the rows is like 10 skulls max
    vector<Skull> skulls;

    // Spawns skulls based on a predefined pattern
    // Patterns are imported from level files
    void Spawn(int level)
    {
        // Read the level file
        FILE *file = fopen("levels/level-01", "r");
        if (file == NULL)
        {
            printf("Error opening file\n");
            return;
        }

        /**
         * Each skull is represented by a character (R, G, B, Y, P, W, K, or B)
         * Each character is either a space, a wall, or a skull (Space = 0, Wall = 1)
         *
         * Spaces in the file are just for readability, so we can ignore them
         */
        char c;

        
        // Top row is always the ceiling
        for (int i = 0; i < SKULL_RADIUS; i++)
        {
            Skull skull;
            skull.color = SKULL_WALL;
            skull.position.x = SKULL_DIAMETER;  // Unlike other skulls, this always start from top left
            skull.position.y = SKULL_DIAMETER;

            skulls.push_back(skull);
        }


        // Start drawing from the middle top, but shifted by half the number of
        // max skulls in a row (default: 5)
        const int DEFAULT_X = SCREEN_W / 2 - (SKULL_DIAMETER * (MAX_SKULLS_PER_ROW / 2));

        int x = SCREEN_W / 2 - (SKULL_DIAMETER * (MAX_SKULLS_PER_ROW / 2));
        int y = SKULL_RADIUS * 3;
        int row = 0;

        while ((c = fgetc(file)) != EOF)
        {

            // If the character is a newline, go next line
            // If row is odd, stagger the next row by half the radius (odd rows have 2 less skulls)
            if (c == '\n')
            {
                row++;
                printf("Newline\n");
                if (row % 2 == 0)
                {
                    printf("Even row\n");
                    x = DEFAULT_X;
                }
                else
                {
                    printf("Odd row\n");
                    x = DEFAULT_X + SKULL_RADIUS;
                }
                y += SKULL_DIAMETER - 4; // move down a row (diameter = 2x radius)
                continue;
            }

            // If the character is a space, ignore it and go next
            if (c == ' ')
            {
                continue;
            }

            // If the character is air, shift the next skull by the diameter to simulate a gap
            if (c == '0')
            {
                x += SKULL_RADIUS;
            }

            // TODO: Fix
            // // If the character is a wall, spawn a wall
            // if (c == '1')
            // {
            //     // Spawn a new wall
            //     Wall wall;
            //     wall.position.x = SCREEN_W / 2;
            //     wall.position.y = SCREEN_H - (x * SKULL_DIAMETER);

            //     skulls.push_back(wall);
            // }

            // If the character is a skull, spawn a skull (which should be literally anything that wasn't
            // already if-checked earlier)
            //
            // Spawn a new skull
            // printf("Spawning color %c at %d, %d\n", c, x, y);

            Skull skull;
            skull.color = ColorCharToSkullColor(c);
            skull.position.x = x;
            skull.position.y = y;

            skulls.push_back(skull);

            x += SKULL_DIAMETER;
        }
    }

    void LoadRandomSkull(Slingshot &slingshot); // defined after Slingshot

    void Draw(Texture2D skullTexture)
    {
        for (Skull skull : skulls)
        {
            skull.Draw(skullTexture);
        }
    }
    void Draw(RenderTexture2D skullTexture)
    {
        for (Skull skull : skulls)
        {
            skull.Draw(skullTexture);
        }
    }

    void SpawnRow()
    {
        // Spawn a new row of skulls
        for (int i = 0; i < SKULL_RADIUS; i++)
        {
            // Spawn a new skull
            Skull skull;
            skull.color = static_cast<SkullColor>(i);
            skull.position.x = SCREEN_W / 2;
            skull.position.y = SCREEN_H - (i * SKULL_RADIUS);

            skulls.push_back(skull);
        }
    }
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

    Vector2 target = {0, 0};

    SkullsManager *skullsManager;
    Skull activeSkull;

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
        target = GetAimTarget();
        DrawCircle(position.x, position.y, 20, DARKBLUE); // base of the slingshot (replace with texture later)
        DrawLine(position.x - 1, position.y, target.x, target.y, DARKBLUE);

        DrawText(("Loaded: " + activeSkull.ToString()).c_str(), 10, 150, 20, BLACK);
    }

    // --- Game Logic ---
    // Returns the endpoint of the aim line
    Vector2 GetAimTarget()
    {
        Vector2 target = position;
        target.x += cos(aimAngle) * SCREEN_W / 2;
        target.y += sin(aimAngle) * SCREEN_W / 2;
        target.x = Clamp(target.x, 0, SCREEN_W);
        return target;
    }

    // TODO: Why won't you work
    void Shoot(SkullsManager &skullsManager)
    {
        DrawText("pew pew", position.x, position.y - 100, 20, BLACK);

        // Shoot the skull (Send it flying based on the slingshot's aim angle)
        activeSkull.position = target;
        // activeSkull->position.y += sin(aimAngle) * SCREEN_W / 2;

        // Skull gone, load a new one
        skullsManager.LoadRandomSkull(*this);
    }
};

// fuck you forward declarations
void SkullsManager::LoadRandomSkull(Slingshot &slingshot)
{
    Skull skull;
    skull.color = static_cast<SkullColor>(skulls[rand() % skulls.size()].color);
    skull.position = slingshot.position;
    slingshot.activeSkull = skull;
}

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
        for (int i = 0; i < SKULL_RADIUS; i++)
        {
            DrawRectangle(0, SCREEN_H - (i * SKULL_RADIUS), SCREEN_W, SKULL_RADIUS, BLACK);
        }
    }
};

int main()
{
    InitWindow(SCREEN_W, SCREEN_H, "Bust-a-Move (but cooler)");

    SetTargetFPS(60);

    int level = 1;
    Slingshot slingshot;

    SkullsManager skullsManager;
    Ceiling ceiling; // Default mode

    skullsManager.Spawn(level);
    skullsManager.LoadRandomSkull(slingshot);
    
    // Actual texture
    Texture2D skullTexture = LoadTexture("assets/skull_jelly_32x32.png");

    // Insane performance hack from Raylib docs (wtf do you mean a texture works better than a circle)
    RenderTexture2D skullRenderTexture = LoadRenderTexture(SKULL_RADIUS * 2, SKULL_RADIUS * 2);
    BeginTextureMode(skullRenderTexture);
    DrawCircle(SKULL_RADIUS, SKULL_RADIUS, SKULL_RADIUS, WHITE);
    DrawCircleLines(SKULL_RADIUS, SKULL_RADIUS, SKULL_RADIUS, BLACK);
    EndTextureMode();

    

    while (!WindowShouldClose())
    {
        // Update
        slingshot.Update();

        if (IsKeyPressed(KEY_SPACE))
        {
            // Shoot skull
            slingshot.Shoot(skullsManager);
        }

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText(("angle: " + to_string(slingshot.aimAngle)).c_str(), 10, 10, 20, BLACK);
        DrawText(("FPS: " + to_string(GetFPS())).c_str(), 10, 30, 20, BLACK);
        slingshot.Draw();

        if (skullTexture.id != 0)
        {
            skullsManager.Draw(skullTexture);
        }
        else
        {
            skullsManager.Draw(skullRenderTexture);
        }

        // DrawText("Press [SPACE] to shoot!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}