#include "../inc/ecs.hpp"


#define MAX_ENTITIES 100000


/* ECS STRUCT STUFF */

ECS::ECS() : 
	entity_count {0}, entities(MAX_ENTITIES, -1), flag_sets(MAX_ENTITIES, 0)
{
	sprites.reserve(MAX_ENTITIES);
	velocities.reserve(MAX_ENTITIES);
	box_colliders.reserve(MAX_ENTITIES);
}

// Need to figure out a way to recycle deallocated entities
Entity ECS::allocate_entity() 
{
	if (entity_count >= MAX_ENTITIES) return -1;
	else entities[entity_count] = entity_count; 
	return entity_count++;
}

void ECS::deallocate_entity(Entity id) 
{
	entities[id] = -1;
}

void ECS::set_flag(Entity id, ComponentType flag) 
{
	flag_sets[id] |= flag;
}

void ECS::clear_flag(Entity id, ComponentType flag) 
{
	flag_sets[id] &= ~flag;
}

void ECS::set_sprite(Entity id, Texture2D tex, Color tint) 
{
	sprites[id] = SpriteComponent(tex, tint);
	set_flag(id, SPRITE);
}

void ECS::set_velocity(Entity id, Vector2 vec) 
{
	velocities[id] = VelocityComponent(vec);
	set_flag(id, VELOCITY);
}

void ECS::set_boxCollider(Entity id, Rectangle rec) 
{
	box_colliders[id] = BoxCollider(rec);
	set_flag(id, BOX_COLLIDER);
}

/* SYSTEMS */

void render_sprites(ECS const& ecs)
{
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if ((ecs.flag_sets[id] & (BOX_COLLIDER | SPRITE)) != (BOX_COLLIDER | SPRITE)) continue;
			
		DrawTextureV(
			ecs.sprites[id].texture,
			(Vector2){
				ecs.box_colliders[id].hitbox.x,
				ecs.box_colliders[id].hitbox.y
			},
			ecs.sprites[id].tint
		);
	}
}

void update_box_colliders(ECS& ecs)
{
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if ((ecs.flag_sets[id] & (BOX_COLLIDER | VELOCITY)) != (BOX_COLLIDER | VELOCITY)) continue;
		
		ecs.box_colliders[id].hitbox.x += ecs.velocities[id].deltaV.x;
		ecs.box_colliders[id].hitbox.y += ecs.velocities[id].deltaV.y;
	}
}


/* COMPONENTS */

SpriteComponent::SpriteComponent(Texture2D tex, Color tint) : 
		texture {tex}, tint {tint} {};

VelocityComponent::VelocityComponent(Vector2 v) : deltaV {v} {};

BoxCollider::BoxCollider(Rectangle rec) : hitbox {rec} {};


/* DEBUG FUNCTIONS */

void debug_draw_hitboxes(ECS const& ecs)
{
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if (ecs.flag_sets[id] & BOX_COLLIDER != BOX_COLLIDER) continue;
		
		DrawRectangleRec(ecs.box_colliders[id].hitbox, RED);
	}
}
