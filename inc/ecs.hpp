#pragma once

#include <raylib.h>
#include <vector>
//Hopefully map realted structs will be refactored to a more sensible module in
//the future
#include "../inc/dungeonGen.hpp" 


using std::vector;

//entities are merely ID:s used to access associated components(actual data,
//separate from any logic). It's then the components that are fed into
//"systems", which themselves are basically just functions that operate on
//specific components
using Entity = long long;

enum ComponentType 
{
	SPRITE 				= 1 << 0,
	VELOCITY 			= 1 << 1,
	BOX_COLLIDER 	= 1 << 2,
};


struct BoxCollider 
{
	Rectangle hitbox;
	
	BoxCollider(Rectangle);
};

struct SpriteComponent 
{
	Texture2D texture; 
	Color tint;

	SpriteComponent(Texture2D, Color); 
};

struct VelocityComponent
{
	Vector2 deltaV;
	
	VelocityComponent(Vector2);
};

struct ECS {
	long long entity_count;
	vector<Entity> entities;
	vector<Entity> deallocated;
	vector<unsigned> flag_sets;
	vector<SpriteComponent> sprites;
	vector<VelocityComponent> velocities;
	vector<BoxCollider> box_colliders;

	ECS();

	Entity allocate_entity();

	void deallocate_entity(Entity);

	void set_flag(Entity, ComponentType);

	void clear_flag(Entity, ComponentType); 
										 	
	void set_sprite(Entity, Texture2D, Color);
	
	void set_velocity(Entity, Vector2);
	
	void set_boxCollider(Entity, Rectangle);
};


void render_sprites(ECS const&);

void update_box_colliders(ECS&);

void debug_draw_hitboxes(ECS const&);

