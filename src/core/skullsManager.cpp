#include <algorithm>
#include "core/consts.h"
#include "core/skullsManager.h"
#include "objects/slingshot.h"
#include "skullsManager.h"

// get the connected group, finding all skulls of the same color that are touching each other in a chain
vector<int> SkullsManager::GetConnectedGroup(int startIndex)
{
    // List of skull indices that are connected to the starting skull
    vector<int> group;

    // Track which skulls we've already processed to avoid infinite loops yucky
    vector<bool> visited(skulls.size(), false);

    // To-do list of skulls to check next, starting with the newly placed skull
    vector<int> stack = {startIndex};

    // Only spread to skulls that match this colour
    SkullColor targetColor = skulls[startIndex].color;

    // Keep checking until there are no more skulls to process
    while (!stack.empty())
    {
        int i = stack.back();
        stack.pop_back();

        // If already processed, skip!
        if (visited[i])
            continue;

        // Mark as processed and add to the connected group
        visited[i] = true;
        group.push_back(i);

        for (int j = 0; j < (int)skulls.size(); j++)
        {
            if (visited[j])
                continue;
            // Skip skulls that don't match our target color
            if (skulls[j].color != targetColor)
                continue;

            // Check if this skull is close enough to be a neighbour
            // SKULL_DIAMETER + 6 covers both direct (32px) and staggered diagonal (~35.8px) neighbours
            float dist = Vector2Distance(skulls[i].position, skulls[j].position);

            if (dist < SKULL_DIAMETER + 6)
                stack.push_back(j);
        }
    }
    return group;
}

// See if there is 4 connected and then erase them!
void SkullsManager::CheckPop(int newSkullIndex)
{
    vector<int> group = GetConnectedGroup(newSkullIndex);

    if (group.size() >= 3)
    {
        // Sort descending so erasing by index doesn't shift earlier indices
        sort(group.rbegin(), group.rend());
        for (int i : group)
            skulls.erase(skulls.begin() + i);

        score += group.size(); // 1 point per skull popped
    }
}

// Spawns skulls based on a predefined pattern
// Patterns are imported from level files
void SkullsManager::Spawn(int level)
{
    // Read the level file
    FILE *file = fopen("src/levels/level-01", "r");
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
        skull.position.x = SKULL_DIAMETER; // Unlike other skulls, this always start from top left
        skull.position.y = SKULL_DIAMETER;

        //     skulls.push_back(skull);
        // }

        // Start drawing from the middle top, but shifted by half the number of
        // max skulls in a row (default: 5)
        const int DEFAULT_X = SCREEN_W / 2 - (SKULL_DIAMETER * (MAX_SKULLS_PER_ROW / 2)) + SKULL_RADIUS;

        int x = SCREEN_W / 2 - (SKULL_DIAMETER * (MAX_SKULLS_PER_ROW / 2)) + SKULL_RADIUS;
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
}

// for collision with the skulls
bool SkullsManager::CheckCollision(ActiveSkull &activeSkull)
{

    if (activeSkull.position.y <= SKULL_RADIUS * 3)
    {
        collidedIndex = -2; // special value meaning "hit ceiling"
        return true;
    }

    for (int i = 0; i < (int)skulls.size(); i++)
    {
        float dist = Vector2Distance(activeSkull.position, skulls[i].position);
        if (dist < SKULL_DIAMETER + 2)
        {
            collidedIndex = i; // store index
            return true;
        }
    }
    collidedIndex = -1;
    return false;
}

// Where to place the skull when it hits another one
void SkullsManager::SnapSkull(ActiveSkull &activeSkull)
{
    if (collidedIndex < 0)
        return;

    // Hit ceiling, snap to nearest x grid position at top row
    if (collidedIndex == -2)
    {
        Skull newSkull;
        newSkull.color = activeSkull.color;
        newSkull.position.x = round(activeSkull.position.x / SKULL_DIAMETER) * SKULL_DIAMETER;
        newSkull.position.y = SKULL_RADIUS * 3;
        skulls.push_back(newSkull);
        CheckPop(skulls.size() - 1);
        return;
    }

    // which direction did it come from
    float dx = activeSkull.position.x - skulls[collidedIndex].position.x;
    float dy = activeSkull.position.y - skulls[collidedIndex].position.y;

    // define all possible snap slots
    Vector2 offsets[] = {
        {0, -(float)SKULL_DIAMETER},
        {0, (float)SKULL_DIAMETER},
        {-(float)SKULL_DIAMETER, 0},
        {(float)SKULL_DIAMETER, 0},
        {-(float)SKULL_RADIUS, -(float)SKULL_DIAMETER},
        {(float)SKULL_RADIUS, -(float)SKULL_DIAMETER},
        {-(float)SKULL_RADIUS, (float)SKULL_DIAMETER},
        {(float)SKULL_RADIUS, (float)SKULL_DIAMETER},
    };

    // pick the best slot using dot product
    Vector2 bestOffset = offsets[0];
    float bestDot = -FLT_MAX;

    for (Vector2 offset : offsets)
    {
        float dot = dx * offset.x + dy * offset.y;
        if (dot > bestDot)
        {
            bestDot = dot;
            bestOffset = offset;
        }
    }

    Skull newSkull;
    newSkull.color = activeSkull.color;
    newSkull.position.x = skulls[collidedIndex].position.x + bestOffset.x;
    newSkull.position.y = skulls[collidedIndex].position.y + bestOffset.y;

    for (Skull &skull : skulls)
    {
        // check the slot isn't already occupied
        if (Vector2Distance(skull.position, newSkull.position) < SKULL_DIAMETER - 2)
            return;
    }

    // place the skull and check for a pop (3 in a row lol)
    skulls.push_back(newSkull);
    CheckPop(skulls.size() - 1);
}

SkullColor SkullsManager::GetRandomSkullColor()
{
    // Pick random color from the enum of colors, minus the wall and magenta
    SkullColor validColors[] = {SKULL_RED, SKULL_GREEN, SKULL_BLUE, SKULL_YELLOW, SKULL_PURPLE, SKULL_ORANGE, SKULL_WHITE, SKULL_BLACK};

    // TODO: Number of colors to choose should scale based on levels
    return validColors[rand() % 8];
}

void SkullsManager::LoadRandomSkull(Slingshot &slingshot)
{
    if (skulls.empty())
        return; // safety check

    slingshot.activeSkull.color = slingshot.nextSkullColor;
    slingshot.activeSkull.position = slingshot.position;
    slingshot.activeSkull.velocity = {0, 0};
    slingshot.activeSkull.isFlying = false;

    // Pick next preview from any skull in the grid
    slingshot.nextSkullColor = skulls[rand() % skulls.size()].color;
}

void SkullsManager::GoDown()
{
    // Compress the ceiling
    for (Skull &skull : skulls)
    {
        skull.position.y += SKULL_DIAMETER;
    }
}

void SkullsManager::CheckLoseCondition(Slingshot &slingshot)
{
    // Draw an imaginary horizontal line at the slingshot, anything below it is a hitbox
    // If anything touches or enters the hitbox, the game is over
    Rectangle hitBox = {0, slingshot.position.y - (2 * SKULL_RADIUS), SCREEN_W, 200};

    for (Skull &skull : skulls)
    {
        if (skull.position.y > hitBox.y)
        {
            printf("Game Over\n");
            DrawText("Game Over", 10, 10, 20, RED);
        }
    }

    // Visually show the hitbox
    DrawLine(0, slingshot.position.y - SKULL_RADIUS - 4, SCREEN_W, slingshot.position.y - SKULL_RADIUS - 3, RED);
}

#pragma region Draw
void SkullsManager::Draw(Texture2D skullTexture)
{
    for (Skull skull : skulls)
    {
        skull.Draw(skullTexture);
    }
}
void SkullsManager::Draw(RenderTexture2D skullTexture)
{
    for (Skull skull : skulls)
    {
        skull.Draw(skullTexture);
    }
}
#pragma endregion

// void SpawnRow()
// {
//     // Spawn a new row of skulls
//     for (int i = 0; i < SKULL_RADIUS; i++)
//     {
//         // Spawn a new skull
//         Skull skull;
//         skull.color = static_cast<SkullColor>(i);
//         skull.position.x = SCREEN_W / 2;
//         skull.position.y = SCREEN_H - (i * SKULL_RADIUS);

//         skulls.push_back(skull);
//     }
// }
