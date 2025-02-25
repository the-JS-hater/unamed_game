#pragma once


#include "raylib.h"
#include <cmath>

void elastic_collision(
	float, Vector2&, Rectangle&, float, Vector2&, Rectangle&
); 

Vector2 normalize(Vector2); 

float dot(Vector2, Vector2); 

Vector2 add(Vector2, Vector2); 

Vector2 sub(Vector2, Vector2); 

Vector2 scale(Vector2, float); 
