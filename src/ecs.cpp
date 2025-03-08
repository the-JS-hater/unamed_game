#include "../inc/ecs.hpp"


#define MAX_ENTITIES 10000 


/* ECS STRUCT STUFF */

ECS::ECS() : 
	entity_count {0}, 
	entities(MAX_ENTITIES, -1), 
	flag_sets(MAX_ENTITIES, 0)
{
	sprites.reserve(MAX_ENTITIES);
	velocities.reserve(MAX_ENTITIES);
	accelerations.reserve(MAX_ENTITIES);
	box_colliders.reserve(MAX_ENTITIES);
	ai_comps.reserve(MAX_ENTITIES);
	lifecycles.reserve(MAX_ENTITIES);
	masses.reserve(MAX_ENTITIES);
	deallocated.reserve(MAX_ENTITIES);
}

Entity ECS::allocate_entity() 
{
	if (deallocated.size() > 0)
	{	
		Entity id = deallocated.back(); 
		deallocated.pop_back();
		entities[id] = id;
		return id;
	} 
	else if (entity_count < MAX_ENTITIES)
	{
		entities[entity_count] = entity_count;
		return entity_count++;
	} 
	else 
	{
		return -1; 
	} 
}

void ECS::deallocate_entity(Entity id) 
{	
	entities[id] = -1;
	deallocated.push_back(id);
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

void ECS::set_velocity(Entity id, Vector2 vec, float max_speed) 
{
	velocities[id] = VelocityComponent(vec, max_speed);
	set_flag(id, VELOCITY);
}

void ECS::set_acceleration(Entity id, Vector2 vec, float retarding_factor) 
{
	accelerations[id] = AccelerationComponent(vec, retarding_factor);
	set_flag(id, ACCELERATION);
}

void ECS::set_boxCollider(Entity id, Rectangle rec) 
{
	box_colliders[id] = BoxCollider(rec);
	set_flag(id, BOX_COLLIDER);
}

void ECS::set_aiComponent(Entity id)
{
	ai_comps[id] = AiComponent();
	set_flag(id, AI);
}


void ECS::set_lifecycle(Entity id, int timer)
{
	lifecycles[id] = LifecycleComponent(timer); 
	set_flag(id, LIFECYCLE);
}


void ECS::set_mass(Entity id, float weight)
{
	masses[id] = MassComponent(weight);
	set_flag(id, MASS);
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

void update_velocities(ECS& ecs)
{

	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if ((ecs.flag_sets[id] & (ACCELERATION | VELOCITY)) != (ACCELERATION | VELOCITY)) continue;
		
		Vector2& velV = ecs.velocities[id].deltaV; 
		float const max_speed = ecs.velocities[id].max_speed;
		Vector2& accV = ecs.accelerations[id].accV;
		float const retarding_factor = ecs.accelerations[id].retarding_factor;
		
		velV.x += accV.x;
		velV.y += accV.y;

		if (accV.x == 0.0f and velV.x > 0.0f) velV.x *= retarding_factor; 
		if (accV.x == 0.0f and velV.x < 0.0f) velV.x *= retarding_factor; 
		if (accV.y == 0.0f and velV.y > 0.0f) velV.y *= retarding_factor; 
		if (accV.y == 0.0f and velV.y < 0.0f) velV.y *= retarding_factor; 
	
		// Stop if your slow as fuck
		if (0.0f <= velV.x and velV.x <= 0.9f) velV.x = 0.0f;
		if (0.0f <= velV.y and velV.y <= 0.9f) velV.y = 0.0f;
		if (0.0f >= velV.x and velV.x >= -0.9f) velV.x = 0.0f;
		if (0.0f >= velV.y and velV.y >= -0.9f) velV.y = 0.0f;
			
		// woah, slow down there buckaroo
		if (velV.x > max_speed) velV.x = max_speed; 
		if (velV.y > max_speed) velV.y = max_speed; 
		if (velV.x < -max_speed) velV.x = -max_speed; 
		if (velV.y < -max_speed) velV.y = -max_speed;
	}
}

void update_ai_entities(ECS& ecs, TileMap const& world, FlowField const& flow_field)
{
	//NOTE: for now i'm not considering AI state
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if ((ecs.flag_sets[id] & (ACCELERATION | AI)) != (ACCELERATION | AI)) continue;
		
		auto [x,y,h,w] = ecs.box_colliders[id].hitbox;

		int idx_x = static_cast<int>((x + w / 2.0f) / world.tile_size);
		int idx_y = static_cast<int>((y + h / 2.0f) / world.tile_size);
	
		auto [dx, dy] = flow_field.flow_field[idx_y][idx_x];
		ecs.accelerations[id].accV = (Vector2){dx, dy};
	}
}


void update_lifecycles(ECS& ecs)
{
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if ((ecs.flag_sets[id] & (LIFECYCLE)) != (LIFECYCLE)) continue;
		
		ecs.lifecycles[id].countdown -= 1;
		if (ecs.lifecycles[id].countdown <= 0) ecs.deallocate_entity(id);
	}
}


/* COMPONENTS */

SpriteComponent::SpriteComponent(Texture2D tex, Color tint) : 
		texture {tex}, tint {tint} {};

VelocityComponent::VelocityComponent(Vector2 v, float max) 
	: deltaV {v}, max_speed{max} {};

AccelerationComponent::AccelerationComponent(Vector2 v, float r) 
	: accV{v}, retarding_factor{r} {};

BoxCollider::BoxCollider(Rectangle rec): hitbox {rec} {};

LifecycleComponent::LifecycleComponent(int n) : countdown(n) {};

MassComponent::MassComponent(float w) : weight{weight} {};

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
