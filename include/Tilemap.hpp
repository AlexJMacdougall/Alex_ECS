#pragma once

#include "Registry.hpp"
#include "Components/Vec2.hpp"

class TileMap
{
public:
	TileMap(Registry* registryPtr,Vec2 size,int SPRITESIZE);
private:
	Registry* m_RegistryPtr;
	Vec2 size;
	int SPRITESIZE;
};