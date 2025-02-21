#pragma once

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <raylib.h>


using std::vector;
using std::min;
using std::max;


template <typename T> 
using matrix = vector<vector<T>>;


enum Tiles
{
	EMPTY,
	WALL
};


struct TileMap
{
	int height;
	int width;	
	matrix<Tiles> map;

	TileMap(int, int);
};


struct BSPnode	
{
	int x;
	int y;
	int w;
	int h;
	BSPnode* left;
	BSPnode* right;

	BSPnode(int, int, int, int);
};

void generate_BSP(BSPnode*, int);

TileMap generate_dungeon(int, int, int);

void generate_dungeon(BSPnode*, TileMap&);

bool can_split_vert(int, int, int); 

bool can_split_hor(int, int, int);

void create_cooridor(TileMap&, int, int, int, int); 

void create_room(TileMap&, BSPnode*);

void debug_draw_dungeon(TileMap const&, int const);

