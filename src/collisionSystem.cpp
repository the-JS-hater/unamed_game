#include "../inc/collisionSystem.hpp"

	
void handle_collisions(
	vector<pair<Entity, Entity>> const& collisions, ECS& ecs
)
{
	for (auto collision : collisions)
	{
		elastic_collision(
			ecs.masses[collision.first].weight,
			ecs.velocities[collision.first].deltaV,
			ecs.box_colliders[collision.first].hitbox,
			ecs.masses[collision.second].weight,
			ecs.velocities[collision.second].deltaV,
			ecs.box_colliders[collision.second].hitbox
		);
	}
}


void elastic_collision(
	float m1, Vector2& v1, Rectangle& r1, float m2, Vector2& v2, Rectangle& r2
) 
{
	Vector2 p1 = (Vector2){r1.x, r1.y};
	Vector2 p2 = (Vector2){r2.x, r2.y};
	Vector2 center1 = add(p1, (Vector2){r1.width, r1.height});
	Vector2 center2 = add(p2, (Vector2){r2.width, r2.height});


	// Normal vector between centers
  Vector2 n = sub(center2, center1);
  Vector2 n_hat = normalize(n); 
	
	// Entities are already moving away from each other
	if (dot(v1, n) < 0.0f) return; 

  // Relative velocity
  Vector2 v_rel = sub(v1, v2);

  // Velocity along the normal
  float v_rel_n = dot(v_rel, n_hat);

  // Calculate impulse (elastic collision with e=1)
  float J = (-2 * v_rel_n) / (1/m1 + 1/m2);

  // Apply impulse with correct direction (add to v1, subtract from v2)
  v1 = add(v1, scale(n_hat, J / m1));
  v2 = sub(v2, scale(n_hat, J / m2));
}


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
			velocity.deltaV.y = 0;
		}
  }
}


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
