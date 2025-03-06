#pragma once

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <utility>
#include <cstdio>

#include <stdio.h>

#include "tileMap.hpp"

using std::vector;
using std::min;
using std::max;
using std::pair;


struct BSPnode	
{
	int x;
	int y;
	int w;
	int h;
	BSPnode* left;
	BSPnode* right;
	Rectangle room;

	BSPnode(int, int, int, int);
};

void generate_BSP(BSPnode*, int const); 

void generate_dungeon(int, int, int, TileMap&); 

void generate_dungeon(BSPnode*, TileMap&); 

void create_room(TileMap&, BSPnode*); 

void connect_nodes(TileMap&, BSPnode*, BSPnode*); 

pair<int, int> get_room_center(BSPnode*); 

void create_corridor( TileMap&, int, int, int, int, int); 
