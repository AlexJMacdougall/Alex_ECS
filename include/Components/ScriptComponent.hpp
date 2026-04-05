#pragma once

#include <raylib.h>

#include "Registry.hpp"
#include "Components/StructComponents.hpp"

#include <iostream>

//Script class that provides a template to be overwritten by other scripts
class Script
{
public:
	Script::Script(Entity entity,Registry* registryPtr) : 
	m_AttachedEntity(entity),
	m_RegistryPtr(registryPtr)
	{};

	Script::~Script()
	{
		std::cout << "Deleting script object for entity "<< m_AttachedEntity << std::endl;
		for (Entity entity : m_InternalEntities) { m_RegistryPtr->DestroyEntity(entity); }
	}

	virtual void update(float dt) = 0;
	virtual void onCollision(std::vector<Entity> collisions) = 0;

protected:
	Entity m_AttachedEntity;
	
	Registry* m_RegistryPtr;
	/*
	float GetDistance(Vec2 pos1, Vec2 pos2)
	{
		//Calculate x and y distance
		float xDist = pos1.x - pos2.x;
		float yDist = pos1.y - pos2.y;

		//Use pythagoras to calculate Distance
		float dist = sqrt((xDist * xDist) + (yDist * yDist));
		return abs(dist);
	}
	*/
	//Set of all entities made by this script so they can be destroyed when the script is deleted
	std::set<Entity> m_InternalEntities;
};

//Component that can be added to entities to allow scripts to be attached
class ScriptComponent
{
public:
	ScriptComponent() = default;

	void update(float dt) { if (m_Script) { m_Script->update(dt); } }; //Runs attached script's update function

	void onCollision(std::vector<Entity> collisions) { if (m_Script) { m_Script->onCollision(collisions); }
}; //Runs attached script's update function

	template<typename T,typename ... Args>
	void attachScript(Args&& ...args)
	{
		T* scriptPtr = new T(std::forward<Args>(args)...); //Create a pointer to a new script and pass in the variadic arguments
		m_Script = scriptPtr; //Set script to T
	}

	template<typename T>
	T* GetScript()
	{
		return reinterpret_cast<T*>(m_Script);
	}

	void deleteScript()
	{
		std::cout << "Deleting script"<< std::endl;
		assert(m_Script != nullptr);
		delete m_Script;
		m_Script = nullptr;
	}

private:
	Script* m_Script{ nullptr }; //Pointer to script object, set as nullptr until assigned
};
