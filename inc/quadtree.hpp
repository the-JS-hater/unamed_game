#pragma once

#include <vector>
#include <cstdlib>
#include <utility>
#include <raylib.h>
#include "../inc/ecs.hpp"


using std::vector;
using std::pair;

struct Quadtree {
    static const int MAX_ENTITIES = 2; 
    static const int MAX_LEVELS = 8; 

    int level;
    Rectangle bounds; 
    vector<Entity> entities; 
    Quadtree* nodes[4]; 

    Quadtree(int, Rectangle const&);

    void clear();

    void split();

    int getIndex(ECS const&, const Entity&) const;

    void insert(ECS const&, Entity const&);

    vector<Entity> retrieve(Rectangle const&) const;
};


void find_all_intersections(
	Quadtree const*, vector<pair<Entity, Entity>>&, ECS const&
);


void debug_render_quadtree(Quadtree const*);


void debug_render_collisions(vector<pair<Entity, Entity>> const&, ECS const&);
