#include "../inc/pathfinding.hpp"


FlowField::FlowField(TileMap& world)
	: world{world} 
{
	cost_field = vector(world.height, vector(world.width, INFINITY));
	flow_field = vector(world.height, vector(world.width, (Vector2){0.0f, 0.0f}));
};


/* BFS from player position, incrementing cost along the way*/
// NOTE: possible optimizations:
// - Limit "simulation distance"
// - Only do partial updates
void FlowField::update_cost_field(int player_x, int player_y)
{	
	vector<Coord> const dirs = {{1,0},{-1,0},{0,1},{0,-1}}; // allow diagonal movement?
	Coord player_pos = make_pair(player_x, player_y);
	
	unordered_set<Coord, hash_pair> visited;
	queue<Coord> to_visit;
	visited.insert(player_pos);
	to_visit.push(player_pos);
	cost_field[player_y][player_x] = 0.0f;
	
	while (!to_visit.empty())
	{
		Coord current = to_visit.front(); to_visit.pop();
		
		auto [x,y] = current;
		// get neighbors
		for (Coord dir : dirs) 
		{	
			auto [dx,dy] = dir;
			Coord new_pos = {x + dx, y + dy};
			auto [nx,ny] = new_pos;

			if (
				nx >= 0 and
			 	nx < world.width and
			 	ny >= 0 and
			 	ny < world.height and
			 	world.map[ny][nx] != WALL and 
			 	visited.find(new_pos) == visited.end()
			) 
			{
				visited.insert(new_pos);
			 	to_visit.push(new_pos);
				cost_field[ny][nx] = cost_field[y][x] + 1.0f;
			}
		}
	}
}


void FlowField::update_flow_field()
{
	
}


void debug_render_costfield(FlowField const& flow_field)
{
	for (int y = 0; y < flow_field.world.height; ++y)
	{
		for (int x = 0; x < flow_field.world.width; ++x)
		{
			int red_val = static_cast<int>(255.0f * min(1.0f, flow_field.cost_field[y][x]/255.0f));
			Color color = (Color){red_val,0,0, 150};
			DrawRectangle(
				x * flow_field.world.tile_size,
				y * flow_field.world.tile_size,
				flow_field.world.tile_size,
				flow_field.world.tile_size,
				color	
			);
		}
	}
}

