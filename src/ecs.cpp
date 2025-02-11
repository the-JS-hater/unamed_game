#include "../inc/ecs.hpp"


#define MAX_ENTITIES 100000


/* ECS STRUCT STUFF */

ECS::ECS() : entity_count {0}, entities(MAX_ENTITIES, -1)
{
	positions.reserve(MAX_ENTITIES);
	sprites.reserve(MAX_ENTITIES);
	velocities.reserve(MAX_ENTITIES);
}

Entity ECS::allocate_entity() {
	// Currently ID == idx of entities vector. Kinda low key dumb, but idk
	// honestly. Should also figure out how to recycle deallocated ID:s
	if (entity_count >= MAX_ENTITIES) return -1;
	else entities[entity_count] = entity_count;
	return entity_count++;
}


void ECS::deallocate_entity(Entity id) {
	entities[id] = -1;
}


void ECS::set_position(Entity id, Vector2 pos) {
	positions[id] = PositionComponent(pos);
}


void ECS::set_sprite(Entity id, Texture2D tex, Color tint) {
	sprites[id] = SpriteComponent(id, tex, tint);
}


void ECS::set_velocity(Entity id, Vector2 vec) {
	velocities[id] = VelocityComponent(vec);
}


/* SYSTEMS */

//TODO: prolly should validate entity... you know... actually has a a sprite
//component, or whatever other stuff
void render_sprites(ECS const& ecs)
{
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		
		DrawTextureV(
			ecs.sprites[id].texture,
			ecs.positions[id].position,
			ecs.sprites[id].tint
		);
	}
}


void update_positions(ECS& ecs)
{
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		
		
		ecs.positions[id].position.x += ecs.velocities[id].deltaV.x;
		ecs.positions[id].position.y += ecs.velocities[id].deltaV.y;
	}
}


