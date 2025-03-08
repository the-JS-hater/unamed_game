#include "../inc/tileMap.hpp"


TileMap::TileMap(int w, int h) 
	: height{h}, width{w}
{
	this -> map = matrix<Tiles>(h, vector<Tiles>(w, WALL));
}
	

void TileMap::init_map_textures()
{
	Texture2D floor_tex = LoadTexture("resources/sprites/Wood.png");
	Texture2D wall_tex = LoadTexture("resources/sprites/UglyBrick.png");

	tex_map.insert({EMPTY, floor_tex});
	tex_map.insert({WALL, wall_tex});
}

Vector2 get_random_spawn_location(TileMap const& tile_map)
{
	while (true)
	{
		int idx_x = rand()%tile_map.width;
		int idx_y = rand()%tile_map.height;

		if (tile_map.map[idx_y][idx_x] == WALL) continue;
		float size = tile_map.tile_size;

		return (Vector2){
			idx_x * size, //+ (size * 0.95f) / 2.0f, 
			idx_y * size //+ (size * 0.95f) / 2.0f 
		};
	}
}


void debug_draw_dungeon(TileMap const& tile_map)
{
	for (int y = 0; y < tile_map.height; y++)
	{
		for (int x = 0; x < tile_map.width; x++)
		{
			DrawTexture(
			 	tile_map.tex_map.find(tile_map.map[y][x])->second,
				x * tile_map.tile_size,
				y * tile_map.tile_size,
				WHITE	
			);
		}
	}
};
