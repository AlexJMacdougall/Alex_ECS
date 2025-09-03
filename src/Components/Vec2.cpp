#include "Components/Vec2.hpp"

Vec2 Vec2Add(Vec2 v1, Vec2 v2)
{
    return Vec2{ v1.x + v2.x, v1.y + v2.y };
}

Vec2 Vec2Minus(Vec2 v1, Vec2 v2)
{
    return Vec2{ v1.x - v2.x, v1.y - v2.y };
}

Vec2 Vec2MultiplyFloat(Vec2 vec, float multi)
{
    return Vec2{ vec.x * multi,vec.y * multi };
}

bool CompareVec2(Vec2 v1, Vec2 v2)
{
    return(v1.x == v2.x && v1.y == v2.y);
}
