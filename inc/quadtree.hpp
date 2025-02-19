#pragma once


#include <vector>
#include <cstdlib>
#include <raylib.h>
#include "../inc/ecs.hpp"


using std::vector;

struct Quadtree {
    static const int MAX_ENTITIES = 20; 
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


void debug_render_quadtree(Quadtree*);
