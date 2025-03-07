#pragma once

#include <unordered_set> 
#include <utility> 
#include <vector> 
#include <cmath> 
#include <raylib.h> 
#include "tileMap.hpp"
#include "player.hpp"

using std::pair;
using std::vector;
using std::hash;
using std::unordered_set;
using Coord = pair<int, int>;


struct FlowField
{
	matrix<float> cost_field;
	matrix<Vector2> flow_field;
	TileMap& world;
	
	FlowField(TileMap&);

	void update_cost_field(int, int);
	
	void update_flow_field();
};


void debug_render_costfield(FlowField const&);

// std::pair hash function taken from:
// https://www.geeksforgeeks.org/how-to-create-an-unordered_map-of-pairs-in-c/
struct hash_pair {
  template <class T1, class T2>
  size_t operator()(const pair<T1, T2>& p) const
  {
    size_t hash1 = hash<T1>{}(p.first);
    size_t hash2 = hash<T2>{}(p.second);
    return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
  }
};


