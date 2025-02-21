#include "../inc/quadtree.hpp"


Quadtree::Quadtree(int level, Rectangle const& bounds) : 
	level {level}, bounds {bounds} 
{
  for (int i {0}; i < 4; i++) 
	{
    nodes[i] = nullptr;
  }
}


void Quadtree::clear() 
{
  entities.clear();
  for (int i {0}; i < 4; i++) 
	{
    if (nodes[i] != nullptr) 
		{
			nodes[i]->clear();
			delete nodes[i];
			nodes[i] = nullptr;
    }
  }
}


void Quadtree::split() 
{
  float x = bounds.x;
  float y = bounds.y;
  float offset_x = bounds.width / 2;
  float offset_y = bounds.height / 2;
		
	//this->nodes
  nodes[0] = new Quadtree(level + 1, (Rectangle){x, y, offset_x, offset_y}); 
  nodes[1] = new Quadtree(level + 1, (Rectangle){x + offset_x, y, offset_x, offset_y}); 
  nodes[2] = new Quadtree(level + 1, (Rectangle){x, y + offset_y, offset_x, offset_y}); 
  nodes[3] = new Quadtree(level + 1, (Rectangle){x + offset_x, y + offset_y, offset_x, offset_y});
}


int Quadtree::getIndex(ECS const& ecs, Entity const& id) const
{
  int index = -1;
	Rectangle entity_bounds = ecs.box_colliders[id].hitbox;

  for (int i {0}; i < 4; i++) 
	{
    if (nodes[i] && CheckCollisionRecs(nodes[i]->bounds, entity_bounds)) 
		{
      index = i;
      break;
    }
  }
  return index;
}


void Quadtree::insert(ECS const& ecs, Entity const& id) 
{
	//NOTE: rewrite as to make sure to insert id into ALL nodes that accept it?
  if (nodes[0]) 
	{
    int index = getIndex(ecs, id);
    if (index != -1) 
		{
      nodes[index]->insert(ecs, id);
      return;
    }
  }

  entities.push_back(id);

  if (entities.size() > MAX_ENTITIES && level < MAX_LEVELS) 
	{
    if (nodes[0] == nullptr) 
		{
      split();
    }
		//NOTE: double check this logic
    for (int i {0}; i < entities.size(); ) 
		{
      int index = getIndex(ecs, entities[i]);
      if (index != -1) 
			{
        nodes[index]->insert(ecs, entities[i]);
        entities.erase(entities.begin() + i);
      } else i++; 
    }
  }
}


vector<Entity> Quadtree::retrieve(Rectangle const& range) const 
{
  vector<Entity> result;

  if (!CheckCollisionRecs(bounds, range))
	{
    return result; 
  }

  for (const auto& entity : entities) 
	{
    result.push_back(entity);
  }
	
	// Any given node either has 4 child nodes, or 0 child nodes
  if (nodes[0]) 
	{
    for (int i {0}; i < 4; i++) 
		{
      vector<Entity> child_results = nodes[i]->retrieve(range);
      result.insert(result.end(), child_results.begin(), child_results.end());
    }
  }
  return result; 
}


void find_all_intersections(
    Quadtree const* tree, 
    vector<pair<Entity, Entity>>& vec,
    ECS const& ecs
)
{
  if (!tree) return;

  for (int i = 0; i < tree->entities.size(); ++i) 
	{
    for (int j = i + 1; j < tree->entities.size(); ++j) 
		{
      Entity entity_a = tree->entities[i];
      Entity entity_b = tree->entities[j];

      Rectangle bounds_a = ecs.box_colliders[entity_a].hitbox;
      Rectangle bounds_b = ecs.box_colliders[entity_b].hitbox;

      if (CheckCollisionRecs(bounds_a, bounds_b)) 
			{
        vec.emplace_back(entity_a, entity_b); 
      }
    }
  }

  for (int i = 0; i < 4; ++i) 
	{
    if (tree->nodes[i]) 
		{
      find_all_intersections(tree->nodes[i], vec, ecs);
    }
  }
}


/* DEBUG FUNCTIONS */

void debug_render_quadtree(Quadtree const* tree)
{
	if (!tree) return;
	
	DrawRectangleLinesEx(tree->bounds, 1, RED);
	
	for (auto child : tree->nodes)
	{
		debug_render_quadtree(child);
	}
}


void debug_render_collisions(
	vector<pair<Entity, Entity>> const& collisions, ECS const& ecs
)
{
	for (auto collision : collisions)
	{
		DrawRectangleRec(ecs.box_colliders[collision.first].hitbox, PINK);
		DrawRectangleRec(ecs.box_colliders[collision.second].hitbox, PINK);
	}
}

