#include "../inc/ecs.hpp"


#define MAX_ENTITIES 1000000


/* ECS STRUCT STUFF */

ECS::ECS() : 
	entity_count {0}, 
	entities(MAX_ENTITIES, -1),
	flag_sets(MAX_ENTITIES, 0)
{
	positions.reserve(MAX_ENTITIES);
	sprites.reserve(MAX_ENTITIES);
	velocities.reserve(MAX_ENTITIES);
}

// Need to figure out a way to recycle deallocated entities
Entity ECS::allocate_entity() {
	if (entity_count >= MAX_ENTITIES) return -1;
	else entities[entity_count] = entity_count;
	return entity_count++;
}

void ECS::deallocate_entity(Entity id) {
	entities[id] = -1;
}

void ECS::set_flag(Entity id, ComponentType flag) {
	flag_sets[id] &= flag;
}

void ECS::set_position(Entity id, Vector2 pos) {
	positions[id] = PositionComponent(pos);
	set_flag(id, POSITION);
}

void ECS::set_sprite(Entity id, Texture2D tex, Color tint) {
	sprites[id] = SpriteComponent(id, tex, tint);
	set_flag(id, SPRITE);
}

void ECS::set_velocity(Entity id, Vector2 vec) {
	velocities[id] = VelocityComponent(vec);
	set_flag(id, VELOCITY);
}


/* SYSTEMS */

void render_sprites(ECS const& ecs)
{
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if (ecs.flag_sets[id] == (POSITION | SPRITE)) continue;
			
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
		if (ecs.flag_sets[id] == (POSITION | VELOCITY)) continue;
		
		ecs.positions[id].position.x += ecs.velocities[id].deltaV.x;
		ecs.positions[id].position.y += ecs.velocities[id].deltaV.y;
	}
}
