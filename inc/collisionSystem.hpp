#pragma once
	

#include "raylib.h"
#include "tileMap.hpp"
#include "ecs.hpp"
#include "vecUtils.hpp"


void elastic_collision(
	float, Vector2&, Rectangle&, float, Vector2&, Rectangle&
); 

void handle_wall_collisions(ECS&, TileMap const&);

bool has_wall_collision(Rectangle const&, TileMap const&);
