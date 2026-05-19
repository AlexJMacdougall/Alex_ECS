#include "Tilemap.hpp"

TileMap::TileMap(Registry* registryPtr, Vec2 size, int SPRITESIZE)
{
	m_RegistryPtr = registryPtr;
	this->size = size;
	this->SPRITESIZE = SPRITESIZE;
}
