#pragma once

#include <raylib.h>
#include <vector>

using std::vector;

//entities are merely ID:s used to access associated components(actual data,
//separate from any logic). It's then the components that are fed into
//"systems", which themselves are basically just functions that operate on
//specific components
using Entity = long long;

enum ComponentType 
{
	POSITION 			= 1 << 0,
	SPRITE 				= 1 << 1,
	VELOCITY 			= 1 << 2,
	BOX_COLLIDER 	= 1 << 3,
};

struct BoxCollider 
{
	Rectangle hitbox;
	//float mass; //NOTE: maybe make into own component later ?
	BoxCollider(Rectangle);
};

struct PositionComponent 
{
    Vector2 position;

    PositionComponent(Vector2); 
};

struct SpriteComponent 
{
	Texture2D texture; 
	Entity position_id;
	Color tint;

	SpriteComponent(Entity, Texture2D, Color); 
};

struct VelocityComponent
{
	Vector2 deltaV;

	VelocityComponent(Vector2);
};

struct ECS {
	long long entity_count;
	vector<Entity> entities; 
	vector<unsigned> flag_sets;
	vector<PositionComponent> positions; 
	vector<SpriteComponent> sprites;
	vector<VelocityComponent> velocities;
	vector<BoxCollider> box_colliders;


	ECS();

	Entity allocate_entity();

	void deallocate_entity(Entity);

	void set_flag(Entity, ComponentType);

	void set_position(Entity, Vector2);

	void set_sprite(Entity, Texture2D, Color);
	
	void set_velocity(Entity, Vector2);
	
	void set_boxCollider(Entity, Rectangle);
};


void render_sprites(ECS const&);

void update_positions(ECS&);

void update_box_colliders(ECS&);

void debug_draw_hitboxes(ECS const&);
