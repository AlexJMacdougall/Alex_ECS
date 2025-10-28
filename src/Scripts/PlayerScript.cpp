#include "Scripts/PlayerScript.hpp"

PlayerScript::PlayerScript(Entity entity,Registry* registryPtr):
	Script(entity,registryPtr)
{

}

void PlayerScript::update(float dt)
{
	int xMovementInput = 0;

	xMovementInput += IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
	
	Physics2D* physicsComponent = m_RegistryPtr->GetComponent<Physics2D>(m_AttachedEntity);

	physicsComponent->force.x += xMovementInput * playerMoveForce;

	if(IsKeyPressed(KEY_SPACE))
	{
		physicsComponent->force.y -= playerJumpForce;
	}
}
