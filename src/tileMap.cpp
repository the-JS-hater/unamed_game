#include "../inc/tileMap.hpp"


TileMap::TileMap(int w, int h) 
	: height{h}, width{w}
{
	this -> map = matrix<Tiles>(h, vector<Tiles>(w, WALL));
};


void debug_draw_dungeon(TileMap const& tile_map, int const tile_size)
{
	for (int y = 0; y < tile_map.height; y++)
	{
		for (int x = 0; x < tile_map.width; x++)
		{
			if (tile_map.map[y][x] == WALL) DrawRectangle(
				x * tile_size,
				y * tile_size,
				tile_size,
				tile_size,
				BLACK
			);
		}
	}
};
