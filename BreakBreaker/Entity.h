#pragma once
#include "vector2.h"

class Entity {
public:
    static Vector2 environmentUpperBounds;

    uint8_t entityID;
    Vector2 lowerBounds;
    Vector2 upperBounds;
    Vector2 position;
    Vector2 size;

    void updateBounds();
    bool isCollidingWith(const Entity& other);
    Entity(Vector2 position, Vector2 size);
};