#include "Scripts/PlayerScript.hpp"

PlayerScript::PlayerScript(Entity entity,Registry* registryPtr, Camera2D* Camera):
	Script(entity,registryPtr)
{
	cameraPtr = Camera;
}

void PlayerScript::update(float dt)
{
	int xMovementInput = 0;
	int yMovementInput = 0;

	xMovementInput += IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
	yMovementInput += IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
	
	Physics2D* physicsComponent = m_RegistryPtr->GetComponent<Physics2D>(m_AttachedEntity);

	physicsComponent->acceleration.x = xMovementInput * playerAcceleration;
	physicsComponent->acceleration.y = yMovementInput * playerAcceleration;

	//Set drag to zero while moving
	if (xMovementInput != 0 || yMovementInput != 0)
	{
		physicsComponent->drag = 0;
	}
	else 
	{
		physicsComponent->drag = playerDrag;
	}
	
	//Update camera pos
	Vec2 playerPos = m_RegistryPtr->GetComponent<Transform2D>(m_AttachedEntity)->position;
	cameraPtr->target = Vector2{playerPos.x, playerPos.y };
}

void PlayerScript::onCollision(std::vector<Entity> collisions)
{
	if (collisions.size() != 0) { std::cout << "Collision!" << std::endl; }
}
