#pragma once

#include <bitset>
#include <memory>
#include <queue>
#include <unordered_map>
#include <set>

#include "ComponentArray.hpp"

using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 16;

using Signature = std::bitset<MAX_COMPONENTS>;

class Registry {
public:
	Registry();
	~Registry();
	//Returns new Entity
	Entity CreateEntity(std::string tag = "");
	//Destroys an entity and its components
	void DestroyEntity(Entity ID);

	//Add and Remove components from entities
	template<typename T>
	void AddComponent(Entity entity,T component);

	template<typename T>
	void RemoveComponent(Entity entity);

	//Get a pointer to ComponentArray of typename T
	template<typename T>
	std::shared_ptr<ComponentArray<T>> GetComponentArray();

	//Add new component type
	template<typename T>
	void RegisterComponent();

	//Getters
	Signature GetSignature(Entity entity);

	std::string GetTag(Entity entity);

	template<typename T>
	T* GetComponent(Entity entity);

	template<typename T>
	bool CheckEntityHasComponent(Entity entity);

	template<typename T>
	std::set<Entity> GetEntitiesWithComponent();

	//template<typename T>
	bool CheckForEntityChanges();

private:
	//Current number of entities and available entity IDs
	Entity m_EntityCount;
	std::queue<Entity> m_AvailableEntities;
	std::set<Entity> m_ActiveEntities;

	//Holds entity signatures that keep track of what components it has
	std::array<Signature, MAX_ENTITIES>* m_Signatures = new std::array<Signature, MAX_ENTITIES>;
	std::array<std::string, MAX_ENTITIES>* m_Tags = new std::array<std::string, MAX_ENTITIES>;

	//Map from typeName to componentArray - holds all component data
	std::unordered_map<const char*, std::shared_ptr<IComponentArray>> m_ComponentArrays;
	//Map from typeName to Component type
	std::unordered_map<const char*, ComponentType> m_typenameToComponentTypes;
	//Map from Component type to typeName
	std::unordered_map<ComponentType,const char*> m_componentTypesToTypename;

	//Counter variable used to assign component type variables
	ComponentType mNextComponentType;

	bool entityChanged = false; //When any changes are made to an entity; this variable is set to true
};

template<typename T>
inline void Registry::AddComponent(Entity entity,T component)
{
	const char* typeName = typeid(T).name();

	//Check entity doesnt have a component of this type already
	//This assertion failing may also mean a component of a type that hasnt been registered has been added
	assert(!m_Signatures->at(entity).test(m_typenameToComponentTypes[typeName]));

	GetComponentArray<T>()->AddComponent(entity,component);

	//Update entity signature
	m_Signatures->at(entity).set(m_typenameToComponentTypes[typeName], true);

	//Set flag variable to true
	entityChanged = true;
}

template<typename T>
inline void Registry::RemoveComponent(Entity entity)
{
	const char* typeName = typeid(T).name();

	assert(m_Signatures->at(entity).test(m_typenameToComponentTypes[typeName]));

	GetComponentArray<T>()->RemoveComponent(entity);

	//Update entity signature
	m_Signatures->at(entity).set(m_typenameToComponentTypes[typeName], false);

	//Set flag variable to true
	entityChanged = true; 
}

template<typename T>
inline std::shared_ptr<ComponentArray<T>> Registry::GetComponentArray()
{
	const char* typeName = typeid(T).name();
	//Returns new std::shared_ptr to ComponentArray of type T
	return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeName]);
}

template<typename T>
inline void Registry::RegisterComponent()
{
	//Get typename of component to use as pointer to its Signature ID  
	const char* typeName = typeid(T).name();

	//Insert pointer to new componentArray of type T mapped to typeName into m_ComponentArrays
	m_ComponentArrays.insert({ typeName,std::make_shared<ComponentArray<T>>()});

	//Get typename from componentID
	m_componentTypesToTypename.insert({ mNextComponentType,typeName });
	//Get componentID from typename
	m_typenameToComponentTypes.insert({ typeName,mNextComponentType });

	//Increment mNextComponentType by 1 for next component
	mNextComponentType += 1;
}

template<typename T>
inline T* Registry::GetComponent(Entity entity)
{
	//Check if entity has component of type T
	//If you failed this assertion you tried to get a component from an entity that doesnt have one of that type
	assert(CheckEntityHasComponent<T>(entity));

	return GetComponentArray<T>()->GetComponent(entity);
}

template<typename T>
bool Registry::CheckEntityHasComponent(Entity entity)
{
	//Check if entity has component of type T
	return m_Signatures->at(entity).test(m_typenameToComponentTypes[typeid(T).name()]);
}

template<typename T>
inline std::set<Entity> Registry::GetEntitiesWithComponent()
{
	const char* typeName = typeid(T).name();

	//Check that a component type exists for this typename
	//If you failed this assertion you tried to get all entities with a component that is not registered
	assert(m_typenameToComponentTypes[typeName]);

	ComponentType type = m_typenameToComponentTypes[typeName];

	std::set<Entity> entities;

	for (Entity entity : m_ActiveEntities)
	{
		if (m_Signatures->at(entity).test(type)) { entities.insert(entity); }
	}

	return entities;
}