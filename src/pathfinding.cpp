#include "inc/pathfinding.hpp"


FlowField::FlowField(Vector2& pos, TileMap& world)
	: player_pos{pos}, world{world} 
{
	cost_field = vector(world.height, vector(world.width, INFINITY));
	flow_field = vector(world.height, vector(world.width, {0.0f, 0.0f}));
};


FlowField::update_cost_field()
{	
	vector<Coord> const dirs = {{1,0},{-1,0},{0,1},{0,-1}};
	unordered_set visited<Coord, hash_pair>;
	vector<Coord> to_visit = {{player_pos.x, player_pos.y}};// allow diagonal movement?
	cost_field[player_pos.y][player_pos.x] = 0.0f;

	while (!to_visit.empty())
	{
		Coord current = to_visit.top(); to_visit.pop_back();
		visited.insert(current);
		
		// get neighbors
		for (Coord dir : dirs) 
		{	
			auto [x,y] = current;
			auto [dx,dy] = dir;
			Coord new_pos = {x + dx, y + dy};
			auto [nx,ny] = new_pos;

			if (
			 nx > 0 and
			 nx < world.width and
			 ny > 0 and
			 ny < world.height and
			 !visited.contains(new_pos)
			) 
			{
			 	to_visit.push_back({new_pos})
				cost_field[ny][nx] = cost_field[y][x] + 1.0f;
			}
		}
	}
}


FlowField::update_flow_field()
{

}
