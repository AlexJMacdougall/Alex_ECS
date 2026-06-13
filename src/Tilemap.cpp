#include "Tilemap.hpp"

TileMap::TileMap(Registry* registryPtr, Vec2 size, Vec2 position, SpriteSheet* spriteSheet, int SPRITESIZE)
{
	m_RegistryPtr = registryPtr;
	this->size = size;
	this->SPRITESIZE = SPRITESIZE;
	this->position = position;
	this->spriteSheet = spriteSheet;
}
