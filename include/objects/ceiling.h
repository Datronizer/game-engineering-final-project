#include <raylib.h>
#include "objects/skull.h"
#include "core/skullsManager.h"
#include "core/consts.h"

/**
 * Compresses the play area, one row at a time
 *
 * Default mode
 */
class Ceiling
{
public:
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
    Ceiling(SkullsManager *skullsManager);

    void Draw();
    void GoDown();

    SkullsManager *skullsManager;

        int stage = 1; // Each stage pushes the ceiling down by SKULL_DIAMETER
    int shots = 0;
};