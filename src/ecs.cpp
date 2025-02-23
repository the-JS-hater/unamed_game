#include "../inc/ecs.hpp"


#define MAX_ENTITIES 100000


/* ECS STRUCT STUFF */

ECS::ECS() : 
	entity_count {0}, entities(MAX_ENTITIES, -1), flag_sets(MAX_ENTITIES, 0)
{
	positions.reserve(MAX_ENTITIES);
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

void ECS::set_position(Entity id, Vector2 pos) 
{
	positions[id] = PositionComponent(pos);
	set_flag(id, POSITION);
}

void ECS::set_sprite(Entity id, Texture2D tex, Color tint) 
{
	sprites[id] = SpriteComponent(id, tex, tint);
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
		if ((ecs.flag_sets[id] & (POSITION | SPRITE)) != (POSITION | SPRITE)) continue;
			
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
		if ((ecs.flag_sets[id] & (POSITION | VELOCITY)) != (POSITION | VELOCITY)) continue;
		
		ecs.positions[id].position.x += ecs.velocities[id].deltaV.x;
		ecs.positions[id].position.y += ecs.velocities[id].deltaV.y;
	
		if (ecs.positions[id].position.x < 0) ecs.velocities[id].deltaV.x = 1;
		if (ecs.positions[id].position.y < 0) ecs.velocities[id].deltaV.y = 1;
		
		//NOTE: Wolrd size hardcoded for testing. Should be changed later on
		if (ecs.positions[id].position.x > 200 * 32) ecs.velocities[id].deltaV.x = -1;
		if (ecs.positions[id].position.y > 200 * 32) ecs.velocities[id].deltaV.y = -1;
	}
}


void update_box_colliders(ECS& ecs)
{
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if ((ecs.flag_sets[id] & (POSITION | BOX_COLLIDER)) != (POSITION | BOX_COLLIDER)) continue;

		ecs.box_colliders[id].hitbox.x = ecs.positions[id].position.x;
		ecs.box_colliders[id].hitbox.y = ecs.positions[id].position.y;
	}
}

//NOTE: This system should be called BEFORE update_positions
void handle_wall_collisions(ECS& ecs, TileMap const& world_map)
{
  for (Entity id : ecs.entities)
  {
    if (ecs.entities[id] == -1) continue;
		if ((ecs.flag_sets[id] & (POSITION | VELOCITY | BOX_COLLIDER)) != (POSITION | VELOCITY | BOX_COLLIDER)) continue;

    VelocityComponent& velocity = ecs.velocities[id];
    BoxCollider const& collider = ecs.box_colliders[id];
    
    Rectangle moved_x = {
      collider.hitbox.x + velocity.deltaV.x,
      collider.hitbox.y,
      collider.hitbox.width,
      collider.hitbox.height
    };
    
    if (has_wall_collision(moved_x, world_map)) velocity.deltaV.x = 0;

    Rectangle moved_y = {
      collider.hitbox.x + velocity.deltaV.x,
      collider.hitbox.y + velocity.deltaV.y,
      collider.hitbox.width,
      collider.hitbox.height
    };
    
    if (has_wall_collision(moved_y, world_map)) velocity.deltaV.y = 0;
  }
}


/* HELPER function, maybe shouldn't even be here...*/
bool has_wall_collision(Rectangle const& rect, TileMap const& world_map)
{
	int const tile_size = world_map.tile_size;

  int const left_tile = (int)(rect.x / tile_size);
  int const right_tile = (int)((rect.x + rect.width) / tile_size);
  int const top_tile = (int)(rect.y / tile_size);
  int const bottom_tile = (int)((rect.y + rect.height) / tile_size);

  for (int y = top_tile; y <= bottom_tile; ++y)
  {
    if (y < 0 || y >= world_map.height) continue;
    
    const auto& row = world_map.map[y];

    for (
			int x = std::max(left_tile, 0); 
			x <= std::min(right_tile, world_map.width - 1); 
			x++
		)
    {
      if (row[x] == WALL)
      {
        Rectangle const tile_rect = {
          (float)x * tile_size,
          (float)y * tile_size,
          tile_size,
          tile_size
        };

        if (CheckCollisionRecs(rect, tile_rect)) return true;
      }
    }
  }
  return false;
}


/* COMPONENTS */

PositionComponent::PositionComponent(Vector2 pos) : position {pos} {};

SpriteComponent::SpriteComponent(Entity pos_id, Texture2D tex, Color tint) : 
		texture {tex}, position_id {pos_id}, tint {tint} {};

VelocityComponent::VelocityComponent(Vector2 v) : deltaV {v} {};

BoxCollider::BoxCollider(Rectangle rec) : hitbox {rec}/*, mass{10.0f}*/ {};


/* DEBUG FUNCTIONS */

void debug_draw_hitboxes(ECS const& ecs)
{
	for (Entity id: ecs.entities)
	{
		if (ecs.entities[id] == -1) continue;
		if (ecs.flag_sets[id] == (POSITION | BOX_COLLIDER)) continue;
		
		DrawRectangleLinesEx(ecs.box_colliders[id].hitbox, 5, RED);
	}
}
