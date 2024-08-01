#include "DynamicEntity.h"
#include <algorithm>

bool DynamicEntity::isExeedingBoundary() {
	Vector2 upperBoundary = this->environmentUpperBounds;
	Vector2 lowerBoundary = Vector2(0, 0);

	Vector2 entUpperBounds = this->upperBounds;
	Vector2 entLowerBounds = this->lowerBounds;

	bool exeedsUpperBoundary = (entUpperBounds.x > upperBoundary.x or entUpperBounds.y > upperBoundary.y);
	bool exeedsLowerBoundary = (entLowerBounds.x < lowerBoundary.x or entLowerBounds.y < lowerBoundary.y);
	
	return exeedsLowerBoundary or exeedsUpperBoundary;
}

void DynamicEntity::setCanCollide(bool canCollide){
	this->canCollide = canCollide;
}

void DynamicEntity::updateVelocity(Vector2 velocity) {
	this->velocity = velocity;
	this->scheduledUpdateTime = 0;
}

void DynamicEntity::schedulePositionUpdate(unsigned int elapsedTime) {
	int smallest = std::min(this->velocity.x, this->velocity.y)/20;
	int updatesPerSecond = std::max(1, std::min(smallest, 20));

	this->difference = Vector2(
		std::max(1, this->velocity.x/updatesPerSecond),
		std::max(1, this->velocity.y/updatesPerSecond)
	);
	this->scheduledUpdateTime = elapsedTime + 1000/updatesPerSecond;
}

void DynamicEntity::updatePosition(Vector2 position) {
	Vector2 lastPosition = this->position;

	this->position = position;
	this->updateBounds();
}
