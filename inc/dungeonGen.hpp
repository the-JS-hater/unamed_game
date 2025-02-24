#pragma once

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include "tileMap.hpp"

using std::vector;
using std::min;
using std::max;


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


