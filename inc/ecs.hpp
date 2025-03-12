#pragma once

#include <raylib.h>
#include <vector>
#include <functional>
#include <pathfinding.hpp>

using std::vector;
using std::function;

//entities are merely ID:s used to access associated components(actual data,
//separate from any logic). It's then the components that are fed into
//"systems", which themselves are basically just functions that operate on
//specific components
using Entity = long long;

enum ComponentType 
{
	SPRITE 						= 1 << 0,
	VELOCITY 					= 1 << 1,
	BOX_COLLIDER 			= 1 << 2,
	ACCELERATION 			= 1 << 3,
	AI 								= 1 << 4,
	LIFECYCLE 				= 1 << 5,
	MASS 							= 1 << 6,
	HEALTH 						= 1 << 7,
	DAMAGE 						= 1 << 8,
	ON_DEATH_CALLBACK	= 1 << 9,
};

enum AiState
{
	IDLE 			= 1 << 0,
	TRACKING  = 1 << 1,
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
	float max_speed;
	
	VelocityComponent(Vector2, float);
};

struct AccelerationComponent
{
	Vector2 accV;
	float retarding_factor; //trust me, retarding is the real term

	AccelerationComponent(Vector2, float);
};

struct AiComponent
{
	AiState state = IDLE;
};

struct LifecycleComponent
{
	int countdown;

	LifecycleComponent(int);
};

struct MassComponent
{
	float weight;

	MassComponent(float);
};

struct HealthComponent
{
	float health;

	HealthComponent(float);
};

struct DamageComponent 
{
	float damage;

	DamageComponent(float);
};

struct ECS {
	long long entity_count;
	vector<Entity> entities;
	vector<Entity> deallocated;
	vector<unsigned> flag_sets;
	vector<SpriteComponent> sprites;
	vector<VelocityComponent> velocities;
	vector<AccelerationComponent> accelerations;
	vector<AiComponent> ai_comps;
	vector<BoxCollider> box_colliders;
	vector<LifecycleComponent> lifecycles;
	vector<MassComponent> masses;
	vector<HealthComponent> health_components;
	vector<DamageComponent> damage_components;
	vector<function<void(Entity, ECS&)>> on_death_callbacks;

	ECS();

	Entity allocate_entity();

	void deallocate_entity(Entity);

	void set_flag(Entity, ComponentType);

	void clear_flag(Entity, ComponentType); 
										 	
	void set_sprite(Entity, Texture2D, Color);
	
	void set_velocity(Entity, Vector2, float);

	void set_acceleration(Entity, Vector2, float);
	
	void set_boxCollider(Entity, Rectangle);
	
	void set_aiComponent(Entity);
	
	void set_lifecycle(Entity, int);

	void set_mass(Entity, float);
	
	void set_health(Entity, float);

	void set_damage(Entity, float);

	void register_on_death(Entity, function<void(Entity, ECS&)>);
};

void render_sprites(ECS const&);

void update_box_colliders(ECS&);

void update_velocities(ECS&);

void update_ai_entities(ECS&, TileMap const&, FlowField const&);

void update_lifecycles(ECS&);

void update_health(ECS&); 

void trigger_on_death(Entity, ECS&);

void debug_draw_hitboxes(ECS const&);

