#include "Entity.h"

Vector2 Entity::environmentUpperBounds = Vector2(127, 63);

void Entity::updateBounds() {
    Vector2 size = this->size;
    Vector2 position = this->position;

    bool xIsEven = (size.x % 2) == 0;
    bool yIsEven = (size.y % 2) == 0;
    int16_t xRadius = std::max(1, size.x / 2);
    int16_t yRadius = std::max(1, size.y / 2);

    int16_t xOffset = 0;
    int16_t yOffset = 0;

    if (xIsEven) {
        xOffset = 1;
    }

    if (yIsEven) {
        yOffset = 1;
    }

    this->upperBounds = Vector2(
        position.x + xRadius - xOffset,
        position.y + yRadius - yOffset
    );

    this->lowerBounds = Vector2(
        position.x - xRadius,
        position.y - yRadius
    );
}

bool Entity::isCollidingWith(const Entity& other) {
    bool xAxisOverlaps = (this->upperBounds.x >= other.lowerBounds.x and this->lowerBounds.x <= other.upperBounds.x);
    bool yAxisOverlaps = (this->upperBounds.y >= other.lowerBounds.y and this->lowerBounds.y <= other.upperBounds.y);

    return xAxisOverlaps and yAxisOverlaps;
};

Entity::Entity(Vector2 position, Vector2 size) {
    this->position = position;
    this->size = size;
    this->updateBounds();
}