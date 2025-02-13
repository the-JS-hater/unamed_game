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
	POSITION 	= 1 << 0,
	SPRITE 		= 1 << 1,
	VELOCITY 	= 1 << 2,
};


struct PositionComponent 
{
    Vector2 position;

    PositionComponent(Vector2 pos) : position {pos} {}; 
};


struct SpriteComponent 
{
	Texture2D texture; 
	Entity position_id;
	Color tint;

	SpriteComponent(Entity pos_id, Texture2D tex, Color tint) : 
		texture {tex}, 
		position_id {pos_id},
		tint {tint}
		{};
};


struct VelocityComponent
{
	Vector2 deltaV;

	VelocityComponent(Vector2 v) : deltaV {v} {};
};


struct ECS {
	long long entity_count;
	vector<Entity> entities; 
	vector<unsigned> flag_sets;
	vector<PositionComponent> positions; 
	vector<SpriteComponent> sprites;
	vector<VelocityComponent> velocities;


	ECS();

	Entity allocate_entity();

	void deallocate_entity(Entity id);

	void set_flag(Entity id, ComponentType flag);

	void set_position(Entity id, Vector2 pos);

	void set_sprite(Entity id, Texture2D tex, Color tint);
	
	void set_velocity(Entity id, Vector2 vec);
};


void render_sprites(ECS const&);

void update_positions(ECS& ecs);
