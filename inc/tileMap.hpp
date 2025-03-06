#pragma once


#include <vector>
#include <raylib.h>
#include <cstdlib>


using std::vector;


template <typename T> 
using matrix = vector<vector<T>>;

enum Tiles
{
	EMPTY,
	WALL
};

struct TileMap
{
	float const tile_size = 32;
	int height;
	int width;
	matrix<Tiles> map;

	TileMap(int, int);
};

Vector2 get_random_spawn_location(TileMap const&);

void debug_draw_dungeon(TileMap const&);
