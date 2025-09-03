#pragma once

//Go through and switch back to raylib vec2 

struct Vec2 {
	float x;
	float y;
};

Vec2 Vec2Add(Vec2 v1, Vec2 v2);
Vec2 Vec2Minus(Vec2 v1, Vec2 v2);
Vec2 Vec2MultiplyFloat(Vec2 v1, float multi);
bool CompareVec2(Vec2 v1, Vec2 v2);