#include <raylib.h>
#include <string>

#include "objects/slingshot.h"

using namespace std;

Slingshot::Slingshot(SkullsManager *skullsManager)
{
    m_skullsManager = skullsManager;
}

void Slingshot::Update()
{
    // move the skull every frame if it is flying
    if (activeSkull.isFlying)
    {
        float dt = GetFrameTime(); // delta time for framerate independent movement

        activeSkull.position.x += activeSkull.velocity.x * dt;
        activeSkull.position.y += activeSkull.velocity.y * dt;

        // Bounce off side walls
        if (activeSkull.position.x <= SKULL_RADIUS + WALL_WIDTH) // hit left wall
        {
            activeSkull.velocity.x *= -1.1f; // goes a little faster when it hits a wall
        }
        else if (activeSkull.position.x >= SCREEN_W  - WALL_WIDTH - SKULL_RADIUS) // hit right wall
        {
            activeSkull.velocity.x *= -1.1f; // goes a little faster when it hits a wall
        }

        // Stick to grid skulls on contact
        if (m_skullsManager && m_skullsManager->CheckCollision(activeSkull))
        {
            activeSkull.isFlying = false;
            m_skullsManager->SnapSkull(activeSkull);
            m_skullsManager->LoadRandomSkull(this);
        }

        // Flew off screen
        if (activeSkull.position.y < 0 ||
            activeSkull.position.x < 0 ||
            activeSkull.position.x > SCREEN_W)
        {
            activeSkull.isFlying = false;
            if (m_skullsManager)
                m_skullsManager->LoadRandomSkull(this);

            // TODO: we should free the skull if it flies off screen
        }
    }

    // Aim and clamp angle to 10 degrees from horizon
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

void Slingshot::Draw(Texture2D *skullTexture)
{
    target = GetAimTarget();

    // Draw slingshot base and aim line
    DrawCircle(position.x, position.y, 20, DARKBLUE);
    DrawLine(position.x - 1, position.y, target.x, target.y, DARKBLUE);

    // Draw active skull on top of the slingshot circle
    if (!activeSkull.isFlying)
        activeSkull.position = position;
    activeSkull.Draw(skullTexture);

    // Draw next skull preview to the left
    Skull nextPreview;
    nextPreview.color = nextSkullColor;
    nextPreview.position = {position.x - 60, position.y};
    nextPreview.Draw(skullTexture);
    DrawText("Next:", (int)position.x - 90, (int)position.y - 20, 16, DARKGRAY);
}

// --- Game Logic ---
// Returns the endpoint of the aim line
Vector2 Slingshot::GetAimTarget()
{
    Vector2 t = position;
    t.x += cos(aimAngle) * SCREEN_W / 2;
    t.y += sin(aimAngle) * SCREEN_W / 2;
    t.x = Clamp(t.x, 0, SCREEN_W);
    return t;
}

// To shoot
void Slingshot::Shoot()
{
    if (activeSkull.isFlying)
        return;

    activeSkull.isFlying = true;

    // allow the skull to store motion
    float speed = 400;
    activeSkull.velocity.x = cos(aimAngle) * speed;
    activeSkull.velocity.y = sin(aimAngle) * speed;

    // Shoot the skull (Send it flying based on the slingshot's aim angle)
    activeSkull.position = position;
}