#pragma once

#include <vector>
#include <map>

#include <raylib.h>

#include "Components/Vec2.hpp"

struct Transform2D
{
	Vec2 position;
	Vec2 scale;
};

struct Physics2D 
{
	float mass;
	float drag;
	bool enableGravity = true;
	Vec2 velocity = { 0,0 };
	Vec2 acceleration = { 0,0 };
	Vec2 force = { 0,0 };
};

struct Impulse
{
	Entity entity;
	Vec2 force;
	float time;
};

struct BoxCollider
{
	float width;
	float height;
};

struct CircleCollider
{
	float radius;
};

struct Sprite
{
	Vec2 UV;
	std::string SpriteSheetID;
	int Layer = 0; //Default layer
	bool hide = false;
};

struct SpriteSheet {
	std::string ID; //Used to referance specific spritesheet
	Texture textures; //Raylib struct that contains the image 
	Vec2 size; //X and y size in sprites
	float spriteSize; //Size of individual sprite in pixels
};

struct Animation
{
	Vec2 startRect;
	int frames;
	int currentFrame = 0;
};

struct AnimatedSprite {
	Sprite* entitySprite;
	float frameTime;
	float currentFrameTime = 0;
	std::map<std::string, Animation> animationData; //Bad, Fix
	std::string currentAnimation;
	std::string lastAnimationFrame;
	bool finishedAnimation = false;
};

struct Navmesh {
	Vec2 pos;
};