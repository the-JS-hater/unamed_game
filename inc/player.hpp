#pragma once 

#include "ecs.hpp"
#include "tileMap.hpp"
#include "vecUtils.hpp"
#include "raylib.h"


// This will hold a sort of refrence to the entity ID of the player, but also
// hold auxilliary data that doesn't fit in, or belong to, the ECS
struct Player
{
	Entity id;

	Player(Entity);
};

Player init_player(ECS&, TileMap const&);

void move_player(ECS&, Player&);

void fire_gun(ECS&, Texture2D&, Player&);

void update_player_camera(Camera2D&, ECS const&, Player const&);
