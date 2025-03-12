#include "../inc/ecs.hpp"


#define MAX_ENTITIES 10000

/* ECS STRUCT STUFF */

ECS::ECS() : 
	entity_count {0}, 
	entities(MAX_ENTITIES, -1), 
	flag_sets(MAX_ENTITIES, 0),
	on_death_callbacks(MAX_ENTITIES, nullptr)
{
	sprites.resize(MAX_ENTITIES);
	velocities.resize(MAX_ENTITIES);
	accelerations.resize(MAX_ENTITIES);
	box_colliders.resize(MAX_ENTITIES);
	ai_comps.resize(MAX_ENTITIES);
	lifecycles.resize(MAX_ENTITIES);
	masses.resize(MAX_ENTITIES);
	health_components.resize(MAX_ENTITIES);
	damage_components.resize(MAX_ENTITIES);
	positions.resize(MAX_ENTITIES);
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
	flag_sets[id] = 0;
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

void ECS::set_velocity(Entity id, Vector2 vec, float max_speed, float retarding_factor) 
{
	velocities[id] = VelocityComponent(vec, max_speed, retarding_factor);
	set_flag(id, VELOCITY);
}

void ECS::set_acceleration(Entity id, Vector2 vec) 
{
	accelerations[id] = AccelerationComponent(vec);
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

void ECS::set_health(Entity id, float val)
{
	health_components[id] = HealthComponent(val);
	set_flag(id, HEALTH);
}

void ECS::set_damage(Entity id, float val)
{
	damage_components[id] = DamageComponent(val);
	set_flag(id, DAMAGE);
}

void ECS::set_position(Entity id , float x, float y)
{
	positions[id] = PositionComponent(x, y);
	set_flag(id, POSITION);
}

void ECS::register_on_death(Entity id, function<void(Entity, ECS&)> foo)
{
	on_death_callbacks[id] = foo;
	set_flag(id, ON_DEATH_CALLBACK);
}

/* SYSTEMS */

void render_sprites(ECS const& ecs)
{
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if ((ecs.flag_sets[id] & (POSITION|SPRITE)) != (POSITION|SPRITE)) continue;
			
		DrawTextureV(
			ecs.sprites[id].texture,
			(Vector2){
				ecs.positions[id].x,
				ecs.positions[id].y
			},
			ecs.sprites[id].tint
		);
	}
}

void update_positions(ECS& ecs)
{
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if ((ecs.flag_sets[id] & (POSITION|VELOCITY)) != (POSITION|VELOCITY)) continue;

		ecs.positions[id].x += ecs.velocities[id].deltaV.x;
		ecs.positions[id].y += ecs.velocities[id].deltaV.y;
	}
}

void update_box_colliders(ECS& ecs)
{
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if ((ecs.flag_sets[id] & (POSITION|BOX_COLLIDER)) != (POSITION|BOX_COLLIDER)) continue;

		ecs.box_colliders[id].hitbox.x = ecs.positions[id].x;
		ecs.box_colliders[id].hitbox.y = ecs.positions[id].y;
	}
}

void update_velocities(ECS& ecs)
{

	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if ((ecs.flag_sets[id] & (ACCELERATION|VELOCITY)) != (ACCELERATION|VELOCITY)) continue;
		
		Vector2& accV = ecs.accelerations[id].accV;
		Vector2& velV = ecs.velocities[id].deltaV; 
		float const max_speed = ecs.velocities[id].max_speed;
		float const retarding_factor = ecs.velocities[id].retarding_factor;
		
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
		if ((ecs.flag_sets[id] & (ACCELERATION|AI|BOX_COLLIDER)) != (ACCELERATION|AI|BOX_COLLIDER)) continue;
		
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
		if (ecs.lifecycles[id].countdown > 0) continue; 
		
		trigger_on_death(id, ecs);
		ecs.deallocate_entity(id);
	}
}


void update_health(ECS& ecs)
{
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if ((ecs.flag_sets[id] & (HEALTH)) != (HEALTH)) continue;
		if (ecs.health_components[id].health > 0.0f) continue;
		
		trigger_on_death(id, ecs);
		ecs.deallocate_entity(id);
	}
}

/* COMPONENTS */

SpriteComponent::SpriteComponent(Texture2D tex, Color tint) : 
		texture {tex}, tint {tint} {};

VelocityComponent::VelocityComponent(Vector2 v, float max, float r) 
	: deltaV {v}, max_speed{max}, retarding_factor{r} {};

AccelerationComponent::AccelerationComponent(Vector2 v) 
	: accV{v} {};

BoxCollider::BoxCollider(Rectangle rec): hitbox {rec} {};

LifecycleComponent::LifecycleComponent(int n) : countdown(n) {};

MassComponent::MassComponent(float w) : weight{w} {};

HealthComponent::HealthComponent(float h) : health{h} {};

DamageComponent::DamageComponent(float d) : damage{d} {};

PositionComponent::PositionComponent(float x, float y) 
	: x{x}, y{y} {};
		
/* DEFAULT CONSTRUCTORS BECAUSE C++ SUCKS */

SpriteComponent::SpriteComponent() : 
		texture {(Texture2D){0,0,0,0,0}}, tint {WHITE} {};

VelocityComponent::VelocityComponent() 
	: deltaV {(Vector2){0.0f,0.0f}}, max_speed{0.0}, retarding_factor{0.0f} {};

AccelerationComponent::AccelerationComponent() 
	: accV{(Vector2){0.0f, 0.0f}} {};

BoxCollider::BoxCollider() : hitbox {(Rectangle){0.0f,0.0f,0.0f,0.0f}} {};

LifecycleComponent::LifecycleComponent(){}

MassComponent::MassComponent(){}

HealthComponent::HealthComponent(){}

DamageComponent::DamageComponent(){}

PositionComponent::PositionComponent(){}

/* HELPER FUNCTIONS */

void trigger_on_death(Entity id, ECS& ecs)
{
	if ((ecs.flag_sets[id] & (ON_DEATH_CALLBACK)) == (ON_DEATH_CALLBACK))
	{
		ecs.on_death_callbacks[id](id, ecs);
	}
}

/* DEBUG FUNCTIONS */

void debug_draw_hitboxes(ECS const& ecs)
{
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if ((ecs.flag_sets[id] & BOX_COLLIDER) != (BOX_COLLIDER)) continue;
		
		DrawRectangleRec(ecs.box_colliders[id].hitbox, RED);
	}
}
