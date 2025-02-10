#pragma once

#include <raylib.h>
#include <vector>

using std::vector;

//entities are merely ID:s used to access associated components(actual data,
//separate from any logic). It's then the components that are fed into
//"systems", which themselves are basically just functions that operate on
//specific components
using Entity = long long;


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


struct ECS {
	long long entity_count;
	vector<Entity> entities; 
	vector<PositionComponent> positions; 
	vector<SpriteComponent> sprites;

	ECS();

	Entity allocate_entity();

	void deallocate_entity(Entity id);

	void set_position(Entity id, Vector2 pos);

	void set_sprite(Entity id, Texture2D tex, Color tint);
};
