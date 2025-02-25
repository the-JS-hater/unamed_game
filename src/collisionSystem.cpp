#include "../inc/collisionSystem.hpp"


//NOTE: This system should be called BEFORE update_positions
void handle_wall_collisions(ECS& ecs, TileMap const& world_map)
{
  for (Entity id : ecs.entities)
  {
    if (ecs.entities[id] == -1) continue;
		if ((ecs.flag_sets[id] & (VELOCITY | BOX_COLLIDER)) != (VELOCITY | BOX_COLLIDER)) continue;

    VelocityComponent& velocity = ecs.velocities[id];
    BoxCollider& collider = ecs.box_colliders[id];
    
    Rectangle moved_x = {
      collider.hitbox.x + velocity.deltaV.x,
      collider.hitbox.y,
      collider.hitbox.width,
      collider.hitbox.height
    };
    
    if (has_wall_collision(moved_x, world_map)) 
		{
			collider.hitbox.x -= velocity.deltaV.x;
			velocity.deltaV.x = 0;
		}

    Rectangle moved_y = {
      collider.hitbox.x + velocity.deltaV.x,
      collider.hitbox.y + velocity.deltaV.y,
      collider.hitbox.width,
      collider.hitbox.height
    };
    
    if (has_wall_collision(moved_y, world_map)) 
		{
			collider.hitbox.y -= velocity.deltaV.y;
			velocity.deltaV.y = 0;
		}
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
