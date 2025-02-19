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
  if (nodes[0] != nullptr) 
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

    for (int i {0}; i < entities.size(); ) 
		{
      int index = getIndex(ecs, entities[i]);
      if (index != -1) 
			{
        nodes[index]->insert(ecs, entities[i]);
        entities.erase(entities.begin() + i);
      } else ++i; 
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
  if (nodes[0] != nullptr) 
	{
    for (int i {0}; i < 4; i++) 
		{
      vector<Entity> child_results = nodes[i]->retrieve(range);
      result.insert(result.end(), child_results.begin(), child_results.end());
    }
  }

  return result; 
}
