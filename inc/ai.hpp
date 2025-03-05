#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <utility>
#include "tileMap.hpp"

using std::abs;
using std::reverse;
using std::pair;
using std::hash;
using std::vector;
using std::priority_queue;
using std::unordered_map;

using Coord = std::pair<int, int>;

float manhattan_distance(Coord const&, Coord const&); 

vector<Coord> reconstruct_path(unordered_map<Coord,Coord> const&, Coord);

vector<Coord> get_neighbors(Coord const&, TileMap const&);

vector<Coord> a_star(Coord, Coord, TileMap const&); 
