#include "Registry.hpp"
#include "iostream"

Registry::Registry()
{
	m_EntityCount = 0;
	for (Entity ID = 0; ID < MAX_ENTITIES; ID++)
	{
		m_AvailableEntities.push(ID);
	}

	m_Signatures->fill(Signature{ 0 });
	mNextComponentType = 0;
}

Registry::~Registry() {};

Entity Registry::CreateEntity(std::string tag)
{
	//Check if there is a available entity 
	//If you failed this assertion you ran out of availabile entity IDs. Either increase the maximum or check if you are creating entities that dont get destroyed when they should
	assert(!m_AvailableEntities.empty());

	Entity entity = m_AvailableEntities.front();
	m_AvailableEntities.pop();	

	//Add entity to active entity set
	m_ActiveEntities.insert(entity);

	m_Tags->at(entity) = tag;

	m_EntityCount += 1;
	return entity;
}

void Registry::DestroyEntity(Entity entity)
{
	//Check if destroyed entity is active
	//If you failed this assertion you tried to destroy an inactive entity, or an entity that cannot be created
	assert(m_ActiveEntities.find(entity) != m_ActiveEntities.end());

	Signature entitySignature = m_Signatures->at(entity);

	//Check entity signature and remove any entities it has
	for (int compId = 0; compId < MAX_COMPONENTS; compId++) {
		if(entitySignature.test(compId))
		{
			const char* typeName = m_componentTypesToTypename[compId];
			m_ComponentArrays[typeName]->RemoveComponent(entity);
		}
	}

	//Remove entity from active entity set
	m_ActiveEntities.erase(entity);

	//Push entity to back of available entity queue and reset signature
	m_AvailableEntities.push(entity);
	m_Signatures->at(entity) = Signature{ 0 };
	m_Tags->at(entity) = "";

	m_EntityCount -= 1;
}

Signature Registry::GetSignature(Entity entity)
{
	return m_Signatures->at(entity);
}

std::string Registry::GetTag(Entity entity)
{
	return m_Tags->at(entity);
}
