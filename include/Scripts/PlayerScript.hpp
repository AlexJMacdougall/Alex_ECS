#pragma once

#include "Components/ScriptComponent.hpp"

class PlayerScript : public Script
{
public:
	PlayerScript::PlayerScript(Entity entity, Registry* registryPtr, Camera2D* camera);
	void PlayerScript::update(float dt) override;
	void PlayerScript::onCollision(std::vector<Entity> collision) override;

private:
	float playerAcceleration = 300;
	float playerDrag = 0.05;

	Camera2D* cameraPtr;
};