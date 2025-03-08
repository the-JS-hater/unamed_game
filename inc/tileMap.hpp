#pragma once


#include <vector>
#include <raylib.h>
#include <cstdlib>
#include <unordered_map>


using std::vector;
using std::unordered_map;


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
	unordered_map<Tiles, Texture2D> tex_map;

	TileMap(int, int);

	void init_map_textures();
};

Vector2 get_random_spawn_location(TileMap const&);

void debug_draw_dungeon(TileMap const&);
