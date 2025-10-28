#pragma once

#include <memory>
#include <vector>
#include <array>
#include <math.h>
#include <iostream>

#include <raylib.h>

#include "Registry.hpp"
#include "Components/ScriptComponent.hpp"

using Entity = std::uint32_t;

const int NUM_OF_SPRITESHEETS = 1;
const int NUM_OF_LAYERS = 4;
const int SPRITE_SIZE = 32;

const float GRAVITY = 100.0f;
const float AIR_DENSITY = 0.05f;

class SystemManager
{
public:
	SystemManager(Registry* registryPtr, int screenWidth, int screenHeight);
	~SystemManager();

	void Update(float dt);
	void PhysicsUpdate(float dt);
	void RunScripts(float dt);
	void Draw();
	void Animate(float dt);
	void ResetGame();

	SpriteSheet* GetSpriteSheet(std::string ID);

	float GetDistance(Entity entity1, Entity entity2);

	void AddForce(Entity entity, Vec2 force);
	void SetConstantForce(Entity entity, Vec2 force);

	float LinearInterp(float pos, Vec2 start, Vec2 end);
	float RayCast(Vec2 start, Vec2 end,int steps);

	Camera2D* GetCamera();

	std::vector<std::pair<Entity,Vec2>> Check_AABB_Collision(Entity entity);
	float AABB_Overlap(Entity colliderA,Entity colliderB);
	std::set<Entity> circleCollision(Entity entity);

private:
	Registry* m_RegistryPtr;

	std::vector<Impulse> m_impulseData;

	Camera2D camera = { 0 };

	int screenWidth = 1280;
	int screenHeight = 720;


	//SpriteSheets
	std::vector<SpriteSheet> m_SpriteSheets;
};
