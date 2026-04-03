#include <algorithm>
#include "core/consts.h"
#include "core/skullsManager.h"
#include "objects/slingshot.h"

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
            // if its a skull wall, skip it!
            if (skulls[j].color != targetColor || skulls[j].color == SKULL_WALL)
                continue;

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

// Walks the skull graph upward to check if a skull is anchored to the ceiling.
// Uses depth-first search, if any path leads to the top row, the skull is supported.
// 'visited' is passed in so we don't revisit skulls across multiple calls in CheckPop.
bool SkullsManager::IsConnectedToCeiling(int index, vector<bool> &visited)
{
    // Already checked this skull in a previous path, skip it
    if (visited[index])
        return false;
    visited[index] = true;

    // If this skull is in the top row (close enough to the ceiling), it's anchored
    if (skulls[index].position.y <= stage * SKULL_DIAMETER + SKULL_RADIUS + 4)
        return true;

    // Recursively check all neighbours, if any of them are ceiling-connected, so is this one
    for (int j = 0; j < (int)skulls.size(); j++)
    {
        if (visited[j])
            continue;
        float dist = Vector2Distance(skulls[index].position, skulls[j].position);
        if (dist < SKULL_DIAMETER + 6)
            if (IsConnectedToCeiling(j, visited))
                return true;
    }
    return false;
}

// Called after every skull lands. Checks for a match of 3+, clears them
// then drops any skulls that are no longer connected to the ceiling
void SkullsManager::CheckPop(int newSkullIndex)
{
    // finds all neighbouring skulls of the same colour connected to the one i placed
    vector<int> group = GetConnectedGroup(newSkullIndex);

    // Need at least 3 of the same colour touching to trigger a clear
    if ((int)group.size() >= 3)
    {
        // 10 points for each skull directly cleared by the match
        score += (int)group.size() * 10;

        // Erase in descending index order so earlier indices don't shift mid-loop
        // erase from the back of the vector first so it doesn't affect lower indices (go left through indices)
        sort(group.rbegin(), group.rend());

        for (int i : group)
            skulls.erase(skulls.begin() + i);

        // After the pop, some skulls may be hanging in mid-air with nothing holding them up
        // Dropped skulls score 20 for the first, then double for each after: 20, 40, 80, 160
        // This rewards setting up big chain drops over just matching 3
        DropFloating();
    }
}

void SkullsManager::DropFloating()
{
    // Exit early if there are no skulls to process
    if (skulls.empty())
        return;

    // One boolean per skull.,. will this skull survive (is it connected to the ceiling)?
    vector<bool> connected(skulls.size(), false);
    // Explicit stack for an iterative depth-first flood fill (avoids recursion overhead)
    vector<int> stack;

    // Check every skull to see if it's touching the ceiling.
    // 'stage' tracks which row the ceiling is on; we convert that to an actual
    // Y position on screen, then add 4 pixels of wiggle room so skulls that
    // are just barely below the line still count as attached.
    for (int i = 0; i < (int)skulls.size(); i++)
    {
        // Find skulls touching the ceiling (if it's y position is close enough to the ceiling line, if it is marked connected, then it gets added to a to do list and gets marked connected)
        if (skulls[i].position.y <= stage * SKULL_DIAMETER + SKULL_RADIUS + 4)
        {
            stack.push_back(i);  // This skull touches the ceiling
            connected[i] = true; // Mark it as connected so it won't be added again
        }
    }

    // Spread out from those ceiling skulls
    // if one skull is safe, check its neighbours, for each skull, check all others and if close enough, they're connected too
    while (!stack.empty())
    {
        int i = stack.back(); // grabs last item in stack
        stack.pop_back();     // removes it from stack

        for (int j = 0; j < (int)skulls.size(); j++)
        {
            if (connected[j])
                continue; // if it is on the ceiling, skip it

            float dist = Vector2Distance(skulls[i].position, skulls[j].position); // 2D positions and returns the straight-line distance between them in pixels.

            // if it's close enough to be a neighbour, mark it as connected and add it to the to do list
            if (dist < SKULL_DIAMETER + 6)
            {
                connected[j] = true;
                stack.push_back(j);
            }
        }
    }

    // Drop anything that wasn't reached, any skull that never got marked as connected has no path back to the ceiling, meaning it's floating in mid-air.
    // score and erase
    int dropPoints = 20; // Base score awarded for the first floating skull dropped

    for (int i = (int)skulls.size() - 1; i >= 0; i--) // avoids shifting index problem!
    {
        // If a skull is not connected to ceiling, it falls
        if (!connected[i])
        {
            score += dropPoints;
            dropPoints *= 2;                  // doubles score the more
            skulls.erase(skulls.begin() + i); // erases them from skulls
        }
    }
}

// Spawns skulls based on a predefined pattern
// Patterns are imported from level files
void SkullsManager::Spawn(int level)
{
    // Read the level file
    // If level is less than 10, add a leading 0 to the level number
    string levelFileName;
    if (level < 10)
        levelFileName = "src/levels/level-0" + to_string(level);
    else
        levelFileName = "src/levels/level-" + to_string(level);

    printf("Loading level %s\n", levelFileName.c_str());
    FILE *file = fopen(levelFileName.c_str(), "r");
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

    const int DEFAULT_X = SCREEN_W / 2 - (SKULL_DIAMETER * (MAX_SKULLS_PER_ROW / 2)) + SKULL_RADIUS;
    int x = DEFAULT_X;
    int y = SKULL_RADIUS * 3;
    int row = 0;

    while ((c = fgetc(file)) != EOF)
    {
        switch (c)
        {
        case '\n':
            row++;
            x = row % 2 == 0 ? DEFAULT_X : DEFAULT_X + SKULL_RADIUS;
            y += SKULL_DIAMETER;
            continue;

        case ' ':
            continue;

        case '0':
            x += SKULL_DIAMETER;
            continue;

        case '1':
        {
            Skull wall;
            wall.color = SKULL_WALL;
            wall.position.x = x - SKULL_RADIUS;
            wall.position.y = y - SKULL_RADIUS;
            skulls.push_back(wall);
            x += SKULL_DIAMETER;
            continue;
        }
        }

        Skull skull;
        skull.color = ColorCharToSkullColor(c);
        skull.position.x = x;
        skull.position.y = y;
        skulls.push_back(skull);
        x += SKULL_DIAMETER;
    }

    fclose(file);
}

// for collision with the skulls
bool SkullsManager::CheckCollision(ActiveSkull &activeSkull)
{
    if (activeSkull.position.y <= stage * SKULL_DIAMETER + SKULL_RADIUS)
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
    // Funny chase condition
    // Hit ceiling, snap to nearest x grid position at top row
    if (collidedIndex == -2)
    {
        printf("Hit ceiling\n");

        Skull newSkull;
        newSkull.color = activeSkull.color;
        newSkull.position.x = round(activeSkull.position.x / SKULL_DIAMETER) * SKULL_DIAMETER;
        newSkull.position.y = stage * SKULL_DIAMETER + SKULL_RADIUS;
        skulls.push_back(newSkull);
        CheckPop(skulls.size() - 1);
        return;
    }

    if (collidedIndex < 0)
        return;

    // which direction did it come from
    float dx = activeSkull.position.x - skulls[collidedIndex].position.x;
    float dy = activeSkull.position.y - skulls[collidedIndex].position.y;

    // define all possible snap slots (hex grid: no direct above/below)
    Vector2 offsets[] = {
        {-(float)SKULL_DIAMETER, 0},
        {(float)SKULL_DIAMETER, 0},
        {-(float)SKULL_RADIUS, -(float)SKULL_DIAMETER},
        {(float)SKULL_RADIUS, -(float)SKULL_DIAMETER},
        {-(float)SKULL_RADIUS, (float)SKULL_DIAMETER},
        {(float)SKULL_RADIUS, (float)SKULL_DIAMETER},
    };

    // pick the best slot using dot product (It figures out which of the 6 hex slots the incoming skull should snap to based on the direction it came from)
    Vector2 bestOffset = offsets[0];
    float bestDot = -FLT_MAX;

    for (Vector2 offset : offsets)
    {
        // So it loops through all 6 slots, calculates the dot product for each, and picks the one with the highest score, meaning the slot that most closely matches where the skull came from
        float dot = dx * offset.x + dy * offset.y;
        if (dot > bestDot)
        {
            bestDot = dot;
            bestOffset = offset;
        }
    }

    // creating the new static skull that replaces the active (flying) skull once it lands
    Skull newSkull;
    newSkull.color = activeSkull.color; // same colour
    newSkull.position.x = skulls[collidedIndex].position.x + bestOffset.x;
    newSkull.position.y = skulls[collidedIndex].position.y + bestOffset.y; // places it at the collided skull's position plus the bestOffset, which is the winning hex slot calculated just before this code

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

void SkullsManager::LoadRandomSkull(Slingshot *slingshot)
{
    if (skulls.empty())
        return; // safety check

    slingshot->activeSkull.color = slingshot->nextSkullColor;
    slingshot->activeSkull.position = slingshot->position;
    slingshot->activeSkull.velocity = {0, 0};
    slingshot->activeSkull.isFlying = false;

    // Pick next preview from any skull in the grid
    slingshot->nextSkullColor = skulls[rand() % skulls.size()].color;
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
    // Visually show the danger line
    DrawLine(0, slingshot.position.y - SKULL_RADIUS - 4, SCREEN_W, slingshot.position.y - SKULL_RADIUS - 3, RED);

    for (Skull &skull : skulls)
    {
        // If any skull crosses the danger line, set the game over flag
        if (skull.position.y > slingshot.position.y - SKULL_RADIUS - 4)
            isGameOver = true;
    }
}

void SkullsManager::CheckWinCondition()
{
    if (skulls.empty())
        isWin = true;
}

#pragma region Draw
void SkullsManager::Draw(Texture2D *skullTexture)
{
    for (Skull skull : skulls)
    {
        skull.Draw(skullTexture);
    }
}
void SkullsManager::Draw(RenderTexture2D *skullTexture)
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
