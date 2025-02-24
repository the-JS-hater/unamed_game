#pragma once
	

#include "dungeonGen.hpp"
#include "ecs.hpp"


void handle_wall_collisions(ECS&, TileMap const&);

bool has_wall_collision(Rectangle const&, TileMap const&);
