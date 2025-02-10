#include "../inc/ecs.hpp"


#define MAX_ENTITIES 100000


ECS::ECS() : entity_count {0}, entities(MAX_ENTITIES, -1)
{
	positions.reserve(MAX_ENTITIES);
	sprites.reserve(MAX_ENTITIES);
}

Entity ECS::allocate_entity() {
	// Currently ID == idx of entities vector. Kinda low key dumb, but idk
	// honestly
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
