#include "raylib.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#include <array>
#include <algorithm>
#include <iostream>

#include "SystemManager.hpp"
#include "Components/ScriptComponent.hpp"
#include "Components/Vec2.hpp"

int main()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1280, 720, "Ecs_Clean");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	//Set up Registry and SystemManager
	Registry REGISTRY;
	REGISTRY.RegisterComponent<Transform2D>();
	REGISTRY.RegisterComponent<Tile>();
	REGISTRY.RegisterComponent<Sprite>();
	REGISTRY.RegisterComponent<AnimatedSprite>();
	REGISTRY.RegisterComponent<ScriptComponent>();
	REGISTRY.RegisterComponent<Physics2D>();
	REGISTRY.RegisterComponent<BoxCollider>();
	REGISTRY.RegisterComponent<CircleCollider>();

	SystemManager SYSTEM(&REGISTRY, 1280, 720);

	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		float dt = GetFrameTime();
		SYSTEM.Update(dt);
	}
	
	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}