#include "../inc/dungeonGen.hpp"


TileMap::TileMap(int w, int h) : 
height{h}, width{w}
{
	this -> map = matrix<Tiles>(h, vector<Tiles>(w, WALL));
};


BSPnode::BSPnode(int x, int y, int w, int h) :
x{x}, y{y}, w{w}, h{h}, left{nullptr}, right{nullptr} {};


void generate_BSP(BSPnode* root, int min_size) 
{
	if (root == nullptr) return;

  int cut;
  if ((rand()%100 > 50) && can_split_vert(root->w, root->h, min_size))
	{
		// Cut along Y-axis
    cut = min_size + rand() % (root->w - min_size * 2);
    root->left = new BSPnode(root->x, root->y, cut, root->h);
    root->right = new BSPnode(root->x + cut, root->y, root->w - cut, root->h);
  } 
	else if (can_split_hor(root->w, root->h, min_size)) 
	{	
		// Cut along X-axis
		cut = min_size + rand() % (root->h - min_size * 2);
		root->left = new BSPnode(root->x, root->y, root->w, cut);
		root->right = new BSPnode(root->x, root->y + cut, root->w, root->h - cut);
  }	
	else 
	{
		// Can't make any cut
		return;
	}

  generate_BSP(root->left, min_size);
  generate_BSP(root->right, min_size);
}


bool can_split_vert(int w, int h, int min_size) 
{
  return (w > min_size * 2 && h > min_size);
};


bool can_split_hor(int w, int h, int min_size)
{
	return (h > min_size * 2 && w > min_size);
};


TileMap generate_dungeon(int w, int h, int min_size)
{
	BSPnode* root = new BSPnode(0, 0, w, h);
	generate_BSP(root, min_size);
	TileMap dungeon = TileMap(w, h);
	generate_dungeon(root, dungeon);
	
	return dungeon;
};


void generate_dungeon(BSPnode* bsp_tree, TileMap& tile_map)
{	
	//Non-leaf:s should always have both left and right child nodes
	if (bsp_tree->left == nullptr)
	{	
		create_room(tile_map, bsp_tree);
		return;
	}
		
	generate_dungeon(bsp_tree->left, tile_map);
	generate_dungeon(bsp_tree->right, tile_map);
	
	BSPnode* left = bsp_tree->left;
	BSPnode* right = bsp_tree->right;

	int x1 = left->x + left->w / 2; 
	int y1 = left->y + left->h / 2;
	int x2 = right->x + right->w / 2;
	int y2 = right->y + right->h / 2;

	create_cooridor(tile_map, x1, y1, x2, y2);
};


void create_room(TileMap& tile_map, BSPnode* leaf) 
{	
	int random_offset_x = (rand()%(leaf->w)) / 5 + 1;
	int random_offset_y = (rand()%(leaf->h)) / 5 + 1;
	int start_x = max(leaf->x, 0);
	int start_y = max(leaf->y, 0);
	int end_x = min(leaf->x + leaf->w, tile_map.width);
	int end_y = min(leaf->y + leaf->h, tile_map.height);

  for (int i {start_y + random_offset_y}; i < end_y - random_offset_y; i++) 
	{
    for (int j {start_x + random_offset_x}; j < end_x - random_offset_y; j++) 
		{
      tile_map.map[i][j] = EMPTY; 
    }
  }
};


void create_cooridor(TileMap& tile_map, int x1, int y1, int x2, int y2) 
{
  if (x1 == x2) 
	{ 
    for (int y = min(y1, y2); y <= max(y1, y2); y++) 
		{
      tile_map.map[y][x1] = EMPTY; 
    }
  } 
	else if (y1 == y2) 
	{ 
    for (int x = min(x1, x2); x <= max(x1, x2); ++x) 
		{
      tile_map.map[y1][x] = EMPTY; 
    }
  } 
	else 
	{ 
    create_cooridor(tile_map, x1, y1, x1, y2); 
    create_cooridor(tile_map, x1, y2, x2, y2); 
  }
};


void debug_draw_dungeon(TileMap const& tile_map)
{
	for (int y = 0; y < tile_map.height; y++)
	{
		for (int x = 0; x < tile_map.width; x++)
		{
			if (tile_map.map[y][x] == WALL) DrawRectangle(x*10,y*10,10,10,BLACK);
		}
	}
};


