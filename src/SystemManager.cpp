#include "SystemManager.hpp"

SystemManager::SystemManager(Registry* registryPtr, int screenWidth,int screenHeight)
{
	m_RegistryPtr = registryPtr;

	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	camera.target = Vector2{ 0.0f, 0.0f };
	camera.offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	m_SpriteSheets = 
	{ 
		{"TestTexture",LoadTexture("LevelSprites.png"),{1,1},SPRITE_SIZE}
	};

	SetTargetFPS(60);

	ResetGame();
}

SystemManager::~SystemManager()
{
	for (auto spriteSheet : m_SpriteSheets) { UnloadTexture(spriteSheet.textures); }
}

void SystemManager::ResetGame()
{
	Entity Player = m_RegistryPtr->CreateEntity("Player");
	Entity Player2 = m_RegistryPtr->CreateEntity("Player2");

	m_RegistryPtr->AddComponent<Transform2D>(Player, Transform2D{ Vec2{0,0},Vec2{1,1} });
	m_RegistryPtr->AddComponent<Transform2D>(Player2, Transform2D{ Vec2{0,100},Vec2{1,1} });

	Sprite playerSprite = Sprite{ {0,0},"TestTexture",0 };
	m_RegistryPtr->AddComponent<Sprite>(Player, playerSprite);
	m_RegistryPtr->AddComponent<Sprite>(Player2, playerSprite);

	Physics2D playerPhysics = Physics2D{ 10.0f,100.0f };
	playerPhysics.enableGravity = false;

	m_RegistryPtr->AddComponent<Physics2D>(Player, playerPhysics);

	m_RegistryPtr->AddComponent<BoxCollider>(Player,BoxCollider{32,32});
	m_RegistryPtr->AddComponent<BoxCollider>(Player2, BoxCollider{32,32});
}

void SystemManager::Update(float dt)
{
	//std::cout << m_RegistryPtr->GetComponent<Physics2D>(0)->velocity.x << " " << m_RegistryPtr->GetComponent<Physics2D>(0)->velocity.y << std::endl;

	Vec2 playerPos = m_RegistryPtr->GetComponent<Transform2D>(0)->position;
	Vec2 playerPos2 = m_RegistryPtr->GetComponent<Transform2D>(1)->position;

	if (IsKeyPressed(KEY_SPACE))
	{
		std::cout << "Space!" << std::endl;
		AddImpulse(0, Vec2{ 0,-10 }, 3.0f);
	}

	RayCast(Vec2{ 100,100 }, Vec2{ 500,500 }, 50);

	PhysicsUpdate(dt);

	RunScripts(dt);

	Animate(dt);

	Draw();

	if (AABB_Collision(0).size() != 0) { std::cout << "Collision!" << std::endl; }
}

void SystemManager::PhysicsUpdate(float dt)
{
	std::set<Entity> PhysObjects = m_RegistryPtr->GetEntitiesWithComponent<Physics2D>();

	//Update Physics Objects
	for(Entity entity : PhysObjects)
	{
		//Get Physics and Transform components
		Physics2D* PhysObject = m_RegistryPtr->GetComponent<Physics2D>(entity);
		Transform2D* transform = m_RegistryPtr->GetComponent<Transform2D>(entity);

		//Calculate acceleration from force
		PhysObject->acceleration.x = PhysObject->force.x / PhysObject->mass;
		PhysObject->acceleration.y = PhysObject->force.y / PhysObject->mass;

		//Apply gravity if not grounded
		if(PhysObject->enableGravity && !PhysObject->grounded)
		{
			AddConstantForce(entity, Vec2{ 0.0f,100.0f });
		}

		//Calculate Velocity from acceleration
		PhysObject->velocity.x = PhysObject->velocity.x + PhysObject->acceleration.x * dt;
		PhysObject->velocity.y = PhysObject->velocity.y + PhysObject->acceleration.y * dt;

		//Update position
		transform->position.x = transform->position.x + PhysObject->velocity.x * dt;
		transform->position.y = transform->position.y + PhysObject->velocity.y * dt;
	}
}


std::set<Entity> SystemManager::AABB_Collision(Entity entity) 	//Returns a set of entities with AABB colliders that collide with the passed entity
{
	std::set entities = m_RegistryPtr->GetEntitiesWithComponent<BoxCollider>();
	entities.erase(entity); //Remove passed entity so it wont collide with itself

	std::set<Entity> collidingEntities = {};

	auto pos1 = m_RegistryPtr->GetComponent<Transform2D>(entity)->position;
	auto collider1 = m_RegistryPtr->GetComponent<BoxCollider>(entity);

	for (Entity colliderEntity : entities)
	{
		auto pos2 = m_RegistryPtr->GetComponent<Transform2D>(colliderEntity)->position;
		auto collider2 = m_RegistryPtr->GetComponent<BoxCollider>(colliderEntity);

		//AABB positions for comparison
		float right1 = pos1.x + (collider1->width / 2);
		float left1 = pos1.x - (collider1->width / 2);

		float right2 = pos2.x + (collider2->width / 2);
		float left2 = pos2.x - (collider2->width / 2);

		float upper1 = pos1.y - (collider1->height / 2);
		float lower1 = pos1.y + (collider1->height / 2);

		float upper2 = pos2.y - (collider2->height / 2);
		float lower2 = pos2.y + (collider2->height / 2);

		if ((right1 >= left2) &&
			(left1 <= right2) &&
			(upper1 <= lower2) &&
			(lower1 >= upper2))
		{
			collidingEntities.insert(colliderEntity);
		}
	}
	return collidingEntities;
}

std::set<Entity> SystemManager::circleCollision(Entity entity) //Returns a set of entities with Circle colliders that collide with the passed entity
{
	std::set entities = m_RegistryPtr->GetEntitiesWithComponent<CircleCollider>();
	entities.erase(entity); //Remove passed entity so it wont collide with itself

	std::set<Entity> collidingEntities = {};

	auto collider1 = m_RegistryPtr->GetComponent<CircleCollider>(entity);

	for (Entity colliderEntity : entities)
	{
		auto collider2 = m_RegistryPtr->GetComponent<CircleCollider>(colliderEntity);

		if(GetDistance(entity,colliderEntity) < (collider1->radius + collider2->radius))
		{
			collidingEntities.insert(colliderEntity);
		}
	}
	return collidingEntities;
}

void SystemManager::RunScripts(float dt)
{
	//Get entities that have script components
	std::set<Entity> entities = m_RegistryPtr->GetEntitiesWithComponent<ScriptComponent>();

	//Iterate over them and call update function
	for(Entity entity:entities) 
	{
		m_RegistryPtr->GetComponent<ScriptComponent>(entity)->update(dt);
	}
}

void SystemManager::Draw()
{
	BeginDrawing();

	ClearBackground(WHITE);

	BeginMode2D(camera);

	//Get entities that are drawable
	std::set<Entity> entities = m_RegistryPtr->GetEntitiesWithComponent<Sprite>();

	Rectangle textureRect = { 0,0,SPRITE_SIZE,SPRITE_SIZE };
	Rectangle positionRect;
	Vector2 origin;

	for (int currentLayer = 0; currentLayer < NUM_OF_LAYERS; currentLayer++)
	{
		for (Entity entity : entities)
		{
			//Get entity sprite
			auto sprite = m_RegistryPtr->GetComponent<Sprite>(entity);
			//Skip drawing sprite if hidden 
			if (!sprite->hide)
			{ 
				//Check the sprites layer
				if (sprite->Layer == currentLayer)
				{
					//If the sprite is on the layer currently being drawn, fetch other data needed and draw it
					auto transform = m_RegistryPtr->GetComponent<Transform2D>(entity);

					//If scale is equal to or less than zero it will not draw.
					assert(transform->scale.x > 0 && transform->scale.y > 0);

					//Set position
					positionRect.x = transform->position.x;
					positionRect.y = transform->position.y;

					//Scale texture
					positionRect.width = SPRITE_SIZE * transform->scale.x;
					positionRect.height = SPRITE_SIZE * transform->scale.y;

					//Set origin
					origin = { positionRect.width / 2, positionRect.height / 2 };

					SpriteSheet* spriteSheet = GetSpriteSheet(sprite->SpriteSheetID);
					Texture textures = spriteSheet->textures;

					//Set textureRect
					textureRect.x = sprite->UV.x * spriteSheet->spriteSize;
					textureRect.y = sprite->UV.y * spriteSheet->spriteSize;

					//Draw Texture
					DrawTexturePro(textures, textureRect, positionRect,origin, 0.0f, WHITE);
				}
			}
		}
	}

	EndMode2D();
	EndDrawing();
}

void SystemManager::Animate(float dt)
{
	//Get entities that have animated sprites
	std::set<Entity> entities = m_RegistryPtr->GetEntitiesWithComponent<AnimatedSprite>();

	for(Entity entity : entities)
	{
		auto animatedSprite = m_RegistryPtr->GetComponent<AnimatedSprite>(entity);
		bool updateSprite = false;

		//Check if the entities animation has changed
		if(animatedSprite->currentAnimation == animatedSprite->lastAnimationFrame)
		{
			//If the same animation is still being played, check if next frame should be shown
			if(animatedSprite->currentFrameTime > 0)
			{
				//If time is not finished for current frame, decrease timer
				animatedSprite->currentFrameTime -= dt;
			}
			else
			{
				//Check if increasing frame will exceed number of frames in animation
				if(animatedSprite->animationData[animatedSprite->currentAnimation].currentFrame + 1 == animatedSprite->animationData[animatedSprite->currentAnimation].frames)
				{
					//If on last frame of animation, reset to first one
					animatedSprite->animationData[animatedSprite->currentAnimation].currentFrame = 0;
					//Update finishedAnimation bool - used to check if animations that should only play once are finished
					animatedSprite->finishedAnimation = true;
				}
				else
				{
					//Increase frame by one
					animatedSprite->animationData[animatedSprite->currentAnimation].currentFrame += 1;
				}
				//Reset timer
				animatedSprite->currentFrameTime = animatedSprite->frameTime;
				updateSprite = true;
			}
		}
		else
		{
			//Update last animated frame to currentAnimation
			animatedSprite->lastAnimationFrame = animatedSprite->currentAnimation;
			//Set currentAnimation frame to zero
			animatedSprite->animationData[animatedSprite->currentAnimation].currentFrame = 0;
			//Reset frame timer
			animatedSprite->currentFrameTime = animatedSprite->frameTime;
			//Set finishedAnimation bool to false
			animatedSprite->finishedAnimation = false;
			updateSprite = true;
		}

		//If the frame has changed, update entity sprite
		if(updateSprite)
		{
			//Get sprite and spritesheet data
			auto sprite = m_RegistryPtr->GetComponent<Sprite>(entity);
			Vec2 spriteSheetSize = GetSpriteSheet(sprite->SpriteSheetID)->size;

			//Calculate UV
			Vec2 newUV = animatedSprite->animationData[animatedSprite->currentAnimation].startRect;

			for(int i = 0; i< animatedSprite->animationData[animatedSprite->currentAnimation].currentFrame;i++)
			{
				if (newUV.x + 1 < spriteSheetSize.x)
				{
					newUV.x += 1; 
				}
				else 
				{ 
					newUV.x = 0; 
					newUV.y += 1; 
				}
			}
			//Set sprite UV to current frame UV
			sprite->UV = Vec2Add(Vec2{ 0,0 },newUV);
		}
	}
}

float SystemManager::LinearInterp(float pos,Vec2 start, Vec2 end)
{
	float x1 = start.x;
	float y1 = start.y;
	float x2 = end.x;
	float y2 = end.y;

	return y1 + (((pos - x1) * (y2 - y1)) / (x2 - x1));
}

float SystemManager::RayCast(Vec2 start, Vec2 end,int steps)
{
	DrawCircle(start.x, start.y, 5, GREEN);
	DrawCircle(end.x, end.y, 5, BLUE);

	float stepDistanceX = (end.x - start.x) / steps;
	float stepDistanceY = (end.y - start.y) / steps;

	//If you failed this assertion you have too many steps in the raycast. You cannot have more steps than distance between the start and end points.
	assert(stepDistanceX >= 1 && stepDistanceY >= 1);

	for(int i = 1;i < steps; i++)
	{
		float newX = LinearInterp(stepDistanceX * i, start, end);
		float newY = LinearInterp(stepDistanceY * i, start, end);
		DrawCircle(start.x + newX, start.y + newY, 5, RED);
	}

	return 0.0f;
}

SpriteSheet* SystemManager::GetSpriteSheet(std::string ID)
{
	for(SpriteSheet spriteSheet : m_SpriteSheets)
	{
		if (spriteSheet.ID == ID)
		{ 
			return &spriteSheet; 
		}
	}
	//Should return spritesheet
	assert(false);
}

float SystemManager::GetDistance(Entity entity1, Entity entity2)
{
	//Get translations of both entities
	auto pos1 =  m_RegistryPtr->GetComponent<Transform2D>(entity1)->position;
	auto pos2 =  m_RegistryPtr->GetComponent<Transform2D>(entity2)->position;

	//Calculate x and y distance
	float xDist = pos1.x - pos2.x;
	float yDist = pos1.y - pos2.y;

	//Use pythagoras to calculate Distance
	float dist = sqrt((xDist*xDist) + (yDist*yDist));
	return dist;
}

void SystemManager::AddConstantForce(Entity entity, Vec2 force)
{
	Physics2D* physicsComp = m_RegistryPtr->GetComponent<Physics2D>(entity);

	physicsComp->force = Vec2Add(force,physicsComp->force);
}

Camera2D* SystemManager::GetCamera()
{
	return &camera;
}
