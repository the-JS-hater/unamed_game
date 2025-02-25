#include "../inc/vecUtils.hpp"


Vector2 normalize(Vector2 v) 
{
	float len = length(v);

	if (len == 0) return {0, 0}; 
	
	return (Vector2){v.x / len, v.y / len};
}

float length(Vector2 v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

float dot(Vector2 a, Vector2 b) 
{
	return a.x * b.x + a.y * b.y;
}


Vector2 add(Vector2 a, Vector2 b)
{
	return (Vector2){a.x + b.x, a.y + b.y};
}


Vector2 sub(Vector2 a, Vector2 b) 
{
	return (Vector2){a.x - b.x, a.y - b.y};
}


Vector2 scale(Vector2 v, float scalar) 
{
	return (Vector2){v.x * scalar, v.y * scalar};
}
