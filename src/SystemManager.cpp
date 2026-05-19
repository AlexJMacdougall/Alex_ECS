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
	Entity Player3 = m_RegistryPtr->CreateEntity("Player2");

	m_RegistryPtr->AddComponent<Transform2D>(Player, Transform2D{ Vec2{0,0},Vec2{1,1} });
	m_RegistryPtr->AddComponent<Transform2D>(Player2, Transform2D{ Vec2{0,100},Vec2{1,1} });
	m_RegistryPtr->AddComponent<Transform2D>(Player3, Transform2D{ Vec2{0,-100},Vec2{1,1} });

	Sprite playerSprite = Sprite{ {0,0},"TestTexture",0 };

	m_RegistryPtr->AddComponent<Sprite>(Player, playerSprite);
	m_RegistryPtr->AddComponent<Sprite>(Player2, playerSprite);
	m_RegistryPtr->AddComponent<Sprite>(Player3, playerSprite);

	PlayerScript *playerScript = new PlayerScript(Player,m_RegistryPtr,&camera);

	ScriptComponent playerScriptComponent;
	playerScriptComponent.attachScript<PlayerScript>(*playerScript);
	m_RegistryPtr->AddComponent<ScriptComponent>(Player, playerScriptComponent);

	Physics2D playerPhysics = Physics2D{ 200.0f, false };

	m_RegistryPtr->AddComponent<Physics2D>(Player, playerPhysics);

	m_RegistryPtr->AddComponent<BoxCollider>(Player, BoxCollider{32,32});
	m_RegistryPtr->AddComponent<BoxCollider>(Player2, BoxCollider{32,32});
	m_RegistryPtr->AddComponent<BoxCollider>(Player3, BoxCollider{32,32});

	TileMap test = TileMap(m_RegistryPtr, Vec2{ 0 }, SPRITE_SIZE);
}

void SystemManager::Update(float dt)
{
	RunScripts(dt);

	PhysicsUpdate(dt);

	Animate(dt);

	Draw();
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

		//Calculate Weight
		//float weight = PhysObject->mass * GRAVITY;

		//Apply weight 
		//if (PhysObject->enableGravity) { AddForce(entity, Vec2{ 0.0f,weight }); }

		//Calculate drag
		float drag_X = PhysObject->drag * -1 * (PhysObject->velocity.x) / 2;
		float drag_Y = PhysObject->drag * -1 * (PhysObject->velocity.y) / 2;

		//Apply Drag
		PhysObject->velocity.x += drag_X;
		PhysObject->velocity.y += drag_Y;
			
		//Vector containing all colliding entities
		std::vector<Entity> collisions;

		//this is bad make it better
		if(m_RegistryPtr->CheckEntityHasComponent<BoxCollider>(entity))
		{
			for (auto collision : AABB_Collision(entity)) { collisions.push_back(collision); }
			//add box to circle collision here
		}
		else if(m_RegistryPtr->CheckEntityHasComponent<CircleCollider>(entity))
		{
			for (auto collision : Circle_To_AABB_Collision(entity)) { collisions.push_back(collision); }
			for (auto collision : CircleCollision(entity)) { collisions.push_back(collision); }
		} 

		//If the entity has collided and has trigger enabled, call it's "onCollision" function
		if(collisions.size() != 0 && PhysObject->enableTrigger)
		{
			if (m_RegistryPtr->CheckEntityHasComponent<ScriptComponent>(entity)){m_RegistryPtr->GetComponent<ScriptComponent>(entity)->onCollision(collisions);}
		}

		//Calculate Velocity from acceleration
		PhysObject->velocity.x += PhysObject->acceleration.x * dt;
		PhysObject->velocity.y += PhysObject->acceleration.y * dt;

		//Clamp Velocity between -max and max
		PhysObject->velocity.x = Clamp(PhysObject->velocity.x, -PhysObject->maxVelocity,PhysObject->maxVelocity);
		PhysObject->velocity.y = Clamp(PhysObject->velocity.y, -PhysObject->maxVelocity,PhysObject->maxVelocity);

		//Update position
		transform->position.x += PhysObject->velocity.x * dt;
		transform->position.y += PhysObject->velocity.y * dt;
		
		/* DEBUG
		std::cout << "Vel: (" << PhysObject->velocity.x << ", " << PhysObject->velocity.y << ")"
			<< "Acc: (" << PhysObject->acceleration.x << ", " << PhysObject->acceleration.y << ")"
		*/

		PhysObject->acceleration = Vec2{ 0 }; //Reset acceleration, so objects dont accelerate with no input
	}
}


std::vector<Entity> SystemManager::AABB_Collision(Entity entity) 	//Returns a set of entities with AABB colliders that collide with the passed entity
{
	std::set<Entity> collidableEntities = m_RegistryPtr->GetEntitiesWithComponent<BoxCollider>();
	collidableEntities.erase(entity); //Remove passed entity so it wont collide with itself

	std::vector<Entity> collidingEntities = {};

	//Check passed entity has a box collider
	assert(m_RegistryPtr->CheckEntityHasComponent<BoxCollider>(entity));

	auto pos1 = m_RegistryPtr->GetComponent<Transform2D>(entity)->position;
	auto collider1 = m_RegistryPtr->GetComponent<BoxCollider>(entity);

	for (Entity colliderEntity : collidableEntities)
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

			Vec2 direction;

			float rightOverlap = abs(left1 - right2);   //Overlap when collider1 is approaching from the right of collider 2
			float leftOverlap = abs(right1 - left2);    //Overlap when collider1 is approaching from the left of collider 2
			float topOverlap = abs(lower1 - upper2);    //Overlap when collider1 is approaching from above collider 2
			float bottomOverlap = abs(upper1 - lower2); //Overlap when collider1 is approaching from below collider 2

			float minOverlapValue = std::min(std::min(rightOverlap, leftOverlap), std::min(topOverlap, bottomOverlap)); //Smallest overlap value is most recent, therefore is axis of collision

		collidingEntities.push_back(colliderEntity);
		}
	}
	return collidingEntities;
}

std::vector<Entity> SystemManager::Circle_To_AABB_Collision(Entity entity) 
{
	std::set<Entity> collidableEntities = m_RegistryPtr->GetEntitiesWithComponent<BoxCollider>();
	collidableEntities.erase(entity);//Remove passed entity so it wont collide with itself

	std::vector<Entity> collidingEntities = {};

	//Check passed entity has a circle collider
	assert(m_RegistryPtr->CheckEntityHasComponent<CircleCollider>(entity));

	//Get circle centre and radius
	Vec2 circleCentre = m_RegistryPtr->GetComponent<Transform2D>(entity)->position;
	float circleRadius = m_RegistryPtr->GetComponent<CircleCollider>(entity)->radius;

	for (Entity colliderEntity : collidableEntities)
	{
		//Get box extents and centre
		Vec2 boxCentre = m_RegistryPtr->GetComponent<Transform2D>(colliderEntity)->position;
		BoxCollider* collider = m_RegistryPtr->GetComponent<BoxCollider>(colliderEntity);
		float halfBoxWidth = collider->width/2;
		float halfBoxHeight = collider->height/2;

		//Get difference vector between circle and box centres
		Vec2 differenceVector = Vec2Minus(circleCentre, boxCentre);

		//Get point of collision by clamping difference vector to box extents then adding to box position
		Vec2 collisionPoint = Vec2Add(Vec2{ Clamp(differenceVector.x,-halfBoxWidth,halfBoxWidth),Clamp(differenceVector.y,-halfBoxHeight,halfBoxHeight) },boxCentre);

		//DrawCircle(collisionPoint.x, collisionPoint.y, 1, BLUE); //DEBUG
		//DrawLine(circleCentre.x, circleCentre.y, collisionPoint.x, collisionPoint.y, GREEN); //DEBUG

		//Get distance between point of collision and circle centre
		float distanceToEdgeSquard = pow((collisionPoint.x - circleCentre.x), 2) + pow((collisionPoint.y - circleCentre.y), 2);

		//Check if distance to edge is less than radius
		if(distanceToEdgeSquard <= pow(circleRadius,2))
		{
			collidingEntities.push_back(colliderEntity);
		}
	}

	return collidingEntities;
}

std::vector<Entity> SystemManager::CircleCollision(Entity entity) //Returns a set of entities with Circle colliders that collide with the passed entity
{
	//NEED TO ADD DETECTING DIRECTION OF COLLISION 
	std::set<Entity> entities = m_RegistryPtr->GetEntitiesWithComponent<CircleCollider>();
	entities.erase(entity); //Remove passed entity so it wont collide with itself

	std::vector<Entity> collidingEntities = {};

	CircleCollider* collider1 = m_RegistryPtr->GetComponent<CircleCollider>(entity);
	Vec2 pos1 = m_RegistryPtr->GetComponent<Transform2D>(entity)->position;

	for (Entity colliderEntity : entities)
	{
		CircleCollider* collider2 = m_RegistryPtr->GetComponent<CircleCollider>(colliderEntity);
		Vec2 pos2 = m_RegistryPtr->GetComponent<Transform2D>(colliderEntity)->position;

		if(GetDistanceSquared(pos1,pos2) < pow(collider1->radius + collider2->radius,2))
		{
			collidingEntities.push_back(colliderEntity);
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

float SystemManager::RayCast(Vec2 start, Vec2 end,int steps,float radius)
{
	float stepDistanceX = (end.x - start.x) / steps;
	float stepDistanceY = (end.y - start.y) / steps;

	//If you failed this assertion you have too many steps in the raycast. You cannot have more steps than distance between the start and end points.
	assert(stepDistanceX >= 1 && stepDistanceY >= 1);

	for(int i = 0;i <= steps; i++)
	{
		float newX = LinearInterp(stepDistanceX * i, start, end);
		float newY = LinearInterp(stepDistanceY * i, start, end);
		DrawCircle(start.x + newX, start.y + newY, radius, RED);
	}

	DrawCircle(start.x, start.y, radius, GREEN);
	DrawCircle(end.x, end.y, radius, BLUE);

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

float SystemManager::GetDistanceSquared(Vec2 pos1, Vec2 pos2)
{
	//Calculate x and y distance
	float xDist = pos2.x - pos1.x;
	float yDist = pos2.y - pos1.y;

	float dist = pow(xDist,2) + pow(yDist,2);
	return dist;
}

float SystemManager::GetDistance(Vec2 pos1,Vec2 pos2)
{
	return sqrt(GetDistanceSquared(pos1, pos2));
}

float SystemManager::Clamp(float value, float min, float max)
{
	return std::max(min, std::min(max,value));
}

float SystemManager::GetVectorMagnitude(Vec2 vector)
{
	return sqrt(GetVectorMagnitudeSquared(vector));
}

float SystemManager::GetVectorMagnitudeSquared(Vec2 vector)
{
	return(pow(vector.x, 2) * pow(vector.y, 2));
}

Camera2D* SystemManager::GetCamera()
{
	return &camera;
}
