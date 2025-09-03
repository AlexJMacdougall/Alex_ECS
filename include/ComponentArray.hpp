#include <cstdint>
#include <array>
#include <cassert>

using Entity = std::uint32_t;
const Entity MAX_ENTITIES = 1000;

//Virtual class for ComponentArrays to inherit from
//Allows mapping to different types of ComponentArray
class IComponentArray
{
public:
	virtual void RemoveComponent(Entity entity) = 0;
};

template <typename T> 
class ComponentArray : public IComponentArray
{
public:
	ComponentArray()
	{
		mSize = 1;
	}

	void AddComponent(Entity entity,T component)
	{
		//Check the componentarray of type T exists
		assert(this != nullptr);

		size_t newIndex = mSize;

		//Append Component to dense array at highest location
		mComponentArray[newIndex] = component;
		//Add index of component to sparse array at the Entity ID's index
		mSparseArray[entity] = newIndex;

		mSize += 1;

		/*//DEBUG
		std::cout << "Addcomponent for entity "<< entity <<" at position " 
			<< mSparseArray[entity] << " {" << std::endl;
		for (int i = 0; i < mSize; i++)
		{
			std::cout <<i<<") " << typeid(mComponentArray[i]).name() << std::endl;
		}
		std::cout << "}" << std::endl;
		*/
	}

	void RemoveComponent(Entity entity) override
	{
		if ((std::string)typeid(T).name() == (std::string)("class ScriptComponent"))
		{
			auto comp = GetComponent(entity);
			reinterpret_cast<ScriptComponent*>(comp)->deleteScript();
		}

		//Cannot remove a component if the size is zero
		assert(mSize > 1);
		//Move last item to deleted item's spot
		uint32_t indexOfRemovedComponent = mSparseArray[entity];
		uint32_t indexOfLastComponent = mSize - 1;
		mComponentArray[indexOfRemovedComponent] = mComponentArray[indexOfLastComponent];

		//Update sparse array to point to correct index
		auto iter = std::find(mSparseArray.begin(), mSparseArray.end(), indexOfLastComponent);
		Entity entityOfLastComponent = std::distance(mSparseArray.begin(), iter);
		mSparseArray[entityOfLastComponent] = indexOfRemovedComponent;

		mSize -= 1;

		//Set sparseArray of removed entity to 0 to avoid finding
		mSparseArray[entity] = 0;

		/*//DEBUG
		std::cout << "RemoveComponent for entity " << entity << std::endl;
		std::cout << "Entity " << entityOfLastComponent << "'s component moved from pos " 
			<< indexOfLastComponent << " to " << indexOfRemovedComponent << std::endl;
		std::cout << "SparseArray location " << entityOfLastComponent << " is now "
			<< mSparseArray[entityOfLastComponent] << "{" << std::endl;
		for (int i = 0; i < mSize; i++)
		{
			std::cout << typeid(mComponentArray[i]).name() << std::endl;
		}
		std::cout << "}" << std::endl;
		*/
	}

	T* GetComponent(Entity entity) 
	{
		return &(mComponentArray[mSparseArray[entity]]);
	}

private:
	//Sparse Array
	std::array<uint32_t, MAX_ENTITIES> mSparseArray;
	//Dense Array
	std::array<T, MAX_ENTITIES> mComponentArray;
	uint32_t mSize;
};
