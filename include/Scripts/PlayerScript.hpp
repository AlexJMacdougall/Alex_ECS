#pragma once

#include "Components/ScriptComponent.hpp"

class PlayerScript : public Script
{
public:
	PlayerScript::PlayerScript(Entity entity, Registry* registryPtr);
	void PlayerScript::update(float dt) override;

private:
	float playerAcceleration = 300;
	float playerDrag = 0.05;
};