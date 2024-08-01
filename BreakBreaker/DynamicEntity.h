#pragma once
#include "Entity.h"

class DynamicEntity : public Entity {
	public:
		Vector2 velocity;
		unsigned int lastUpdatedTime = 0;
		unsigned int scheduledUpdateTime = 0;
		bool canCollide = true;
		Vector2 difference;
		
		bool isExeedingBoundary();
		
		void setCanCollide(bool canCollide);
		void updateVelocity(Vector2 velocity);
		void schedulePositionUpdate(unsigned int elapsedTime);
		void updatePosition(Vector2 position);
		using Entity::Entity;
};



