#pragma once

#include "Registry.hpp"
#include "Components/Vec2.hpp"
#include "Components/StructComponents.hpp"

class TileMap
{
public:
	TileMap(Registry* registryPtr,Vec2 size, Vec2 position,SpriteSheet* spriteSheet, int SPRITESIZE);
private:
	Registry* m_RegistryPtr;
	Vec2 size; //Tilemap size in tiles
	Vec2 position;
	int SPRITESIZE; //Tile size in pixels (Tiles are square)
	SpriteSheet* spriteSheet; 
};