#pragma once


#include <vector>
#include <raylib.h>


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
	int const tile_size = 32;
	int height;
	int width;
	matrix<Tiles> map;

	TileMap(int, int);
};


void debug_draw_dungeon(TileMap const&, int const);
