#pragma once

#include <memory>
#include <vector>
#include <array>
#include <math.h>
#include <iostream>

#include <raylib.h>

#include "Registry.hpp"
#include "Components/ScriptComponent.hpp"

#include "Scripts/PlayerScript.hpp"

using Entity = std::uint32_t;

const int NUM_OF_SPRITESHEETS = 1;
const int NUM_OF_LAYERS = 4;
const int SPRITE_SIZE = 32;

const float GRAVITY = 100.0f;

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
	void GetNewEntities();

	SpriteSheet* GetSpriteSheet(std::string ID);

	float LinearInterp(float pos, Vec2 start, Vec2 end);
	float RayCast(Vec2 start, Vec2 end,int steps,float radius);

	Camera2D* GetCamera();

	std::vector<Entity> AABB_Collision(Entity entity);
	std::vector<Entity> Circle_To_AABB_Collision(Entity entity);
	std::vector<Entity> CircleCollision(Entity entity);

	float GetDistanceSquared(Vec2 pos1, Vec2 pos2);
	float GetDistance(Vec2 pos1, Vec2 pos2);
	float Clamp(float value, float min, float max);
	float GetVectorMagnitude(Vec2 vector);
	float GetVectorMagnitudeSquared(Vec2 vector);

private:
	Registry* m_RegistryPtr;

	std::vector<Impulse> m_impulseData;

	Camera2D camera = { 0 };

	int screenWidth = 1280;
	int screenHeight = 720;

	//SpriteSheets
	std::vector<SpriteSheet> m_SpriteSheets;
};
