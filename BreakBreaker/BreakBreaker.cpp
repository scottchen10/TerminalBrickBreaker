#define NOMINMAX

#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

#include "DynamicEntity.h"
#include "Entity.h"
#include "Vector2.h"
#include <array>
#include <math.h>

const uint8_t PADDLE_SPEED = 5;
const uint8_t BALL_SPEED = 5;
const Vector2 DISPLAY_SIZE = Vector2(127, 63);

enum inputDirection : int8_t {
    LEFT = -1,
    STOP,
    RIGHT
};

inputDirection getUserInput() {
    inputDirection input = STOP;

    if ((GetKeyState('A') & 0x8000)) {
        input = LEFT;
    }
    else if (GetKeyState('D') & 0x8000) {
        input = RIGHT;
    }
    return input;
};

void render(std::array<bool, 8192>& screen, const std::vector<Entity*>& entities) {
    for (int index = 0; index < 8192; index++) {
        screen[index] = false;
    }

    for (Entity* ent : entities) {
        for (int x = ent->lowerBounds.x; x <= ent->upperBounds.x; x++) {
            for (int y = ent->lowerBounds.y; y <= ent->upperBounds.y; y++) {
                int index = (y * 128 + x);

                if (index > 8191 or index < 0) {;
                    continue;
                }
                screen[y * 128 + x] = true;
            }
        }
    }

};

void display(const std::array<bool, 8192>& screen) {
    system("cls");

    std::string display = "";

    for (int x = 63; x >= 0; x--) {
        std::string text = "";
        for (int y = 127; y >= 0; y--) {
            if (screen[x * 128 + y]) {
                text += "$";
            }
            else {
                text += ".";
            }
        }
        text += "\n";
        display += text;
    }
    std::cout << display;
}

void updatePhysics(unsigned int elapsedTime, const std::vector<DynamicEntity*>& dynamicEntities) {
    for (DynamicEntity* ent : dynamicEntities) {
        if (ent->scheduledUpdateTime = 0) {
            ent->schedulePositionUpdate(elapsedTime);
        }

        if (ent->scheduledUpdateTime < elapsedTime) {
            ent->updatePosition(
                Vector2(
                    ent->position.x + ent->velocity.x,
                    ent->position.y + ent->velocity.y
                )
            );
            ent->schedulePositionUpdate(elapsedTime);
        }
    }
}

std::vector<Entity*> resolveCollisions(const std::vector<DynamicEntity*>& dynamicEntities, const std::vector<Entity*>& entities) {
    std::vector<Entity*> collisions;
    collisions.reserve(1);

    for (DynamicEntity* ent : dynamicEntities) {
        if (ent->canCollide == false) {
            continue;
        }

        if (ent->isExeedingBoundary()) {
            int verticalDist = std::min(
                std::abs(ent->upperBounds.y),
                std::abs(ent->lowerBounds.y - DISPLAY_SIZE.y)
            );

            int horizontalDist = std::min(
                std::abs(ent->upperBounds.x),
                std::abs(ent->lowerBounds.x - DISPLAY_SIZE.x)
            );

            Vector2 velocity = Vector2(
                verticalDist > horizontalDist ? -ent->velocity.x : ent->velocity.x,
                verticalDist < horizontalDist ? -ent->velocity.y : ent->velocity.y
            );

            ent->updateVelocity(velocity);
            ent->updatePosition(
                Vector2(
                    ent->position.x + ent->velocity.x,
                    ent->position.y + ent->velocity.y
                )
            );
        }

        for (Entity* otherEnt : entities) {
            if (otherEnt == ent) {
                continue;
            }

            if (not ent->isCollidingWith(*otherEnt)) {
                continue;
            }

            int verticalDist = std::min(
                std::abs(ent->upperBounds.y - otherEnt->lowerBounds.y),
                std::abs(ent->lowerBounds.y - otherEnt->upperBounds.y)
            );

            int horizontalDist = std::min(
                std::abs(ent->upperBounds.x - otherEnt->lowerBounds.x),
                std::abs(ent->lowerBounds.x - otherEnt->upperBounds.x)
            );

            Vector2 velocity = Vector2(
                horizontalDist < verticalDist ? -ent->velocity.x : ent->velocity.x,
                verticalDist < horizontalDist ? -ent->velocity.y : ent->velocity.y
            );
            ent->updateVelocity(velocity);
            ent->updatePosition(
                Vector2(
                    ent->position.x + ent->velocity.x,
                    ent->position.y + ent->velocity.y
                )
            );

            collisions.emplace_back(otherEnt);
        }
    }
    
    return collisions;
}

int main() {
    bool running = true;
    unsigned int timeElapsed = 0;
    std::array<bool, 8192> screen;

    std::vector<Entity*> entities;
    std::vector<DynamicEntity*> dynamicEntities;

    entities.reserve(66);
    dynamicEntities.reserve(2);

    // Initialize all entities
    DynamicEntity paddle = DynamicEntity(
        Vector2(64, 4),
        Vector2(20, 2)
    );
    paddle.setCanCollide(false);

    DynamicEntity ball = DynamicEntity(
        Vector2(64, 8),
        Vector2(2, 2)
    );
    dynamicEntities.emplace_back(&ball);
    dynamicEntities.emplace_back(&paddle);

    for (int16_t column = 0; column < 16; column++) {
        for (int16_t row = 0; row < 4; row++) {
            Entity* block = new Entity(
                Vector2(3 + column * 8, 60 - row * 6),
                Vector2(7, 3)
            );

            entities.emplace_back(block);
        }
    }
    entities.emplace_back(&paddle);
    entities.emplace_back(&ball);

    ball.updateVelocity(Vector2(1, 1));
    
    inputDirection lastInput = STOP;
    while (running) {
        // update paddle velocity depending on input
        inputDirection input = getUserInput();

        if (input != lastInput) {
            lastInput = input;
            paddle.updateVelocity(Vector2(-input * PADDLE_SPEED, 0));
        }
        
        updatePhysics(timeElapsed, dynamicEntities);
        std::vector<Entity*> collisions = resolveCollisions(dynamicEntities, entities);

        for (Entity* collision : collisions) {
            if (collision == &paddle) {
                continue;
            }
            //delete entities when collided with
            auto it = std::find(entities.begin(), entities.end(), collision);

            if (it != entities.end()) {
                entities.erase(it);
            }
        }

        render(screen, entities);
        display(screen);

        Sleep(50);
        timeElapsed += 50;
    }


    return 0;
}