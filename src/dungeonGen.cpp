#include "../inc/dungeonGen.hpp"


TileMap::TileMap(unsigned w, unsigned h) : 
height{h}, width{w}
{
	this -> map = matrix<Tiles>(h, vector<Tiles>(w, WALL));
};


BSPnode::BSPnode(int x, int y, int w, int h) :
x{x}, y{y}, w{w}, h{h}, left{nullptr}, right{nullptr} {};


void generate_BSP(BSPnode* root, int min_size) 
{
  if (!can_split(root->w, root->h, min_size)) {
    return; 
  }

  int cut;
  if (rand() % 2 == 0) {
		// Cut along Y-axis
    cut = min_size + rand() % (root->w - min_size * 2);
    root->left = new BSPnode(root->x, root->y, cut, root->h);
    root->right = new BSPnode(root->x + cut, root->y, root->w - cut, root->h);
  } else {	
		// Cut along X-axis
		cut = min_size + rand() % (root->h - min_size * 2);
		root->left = new BSPnode(root->x, root->y, root->w, cut);
		root->right = new BSPnode(root->x, root->y + cut, root->w, root->h - cut);
  }

  generate_BSP(root->left, min_size);
  generate_BSP(root->right, min_size);
}


bool can_split(int w, int h, int min_size) 
{
  return 
		(w > min_size * 2 && h > min_size) || 
		(h > min_size * 2 && w > min_size);
};


TileMap generate_dungeon(unsigned w, unsigned h, unsigned min_size)
{
	BSPnode* root = new BSPnode(0, 0, w, h);
	generate_BSP(root, min_size);
	
	TileMap dungeon = TileMap(w, h);
	// TODO: USE THE BSP TREE to fill in the TileMap here
	

	return TileMap(w, h); 
};


void create_room(TileMap& map, int x, int y, int w, int h) 
{
  for (int i {y}; i < y + h; i++) 
	{
    for (int j {x}; j < x + w; j++) 
		{
      map.map[i][j] = EMPTY; 
    }
  }
}


void create_cooridor(TileMap& map, int x1, int y1, int x2, int y2) {
  if (x1 == x2) 
	{ 
    for (int y = min(y1, y2); y <= max(y1, y2); ++y) 
		{
      map.map[y][x1] = EMPTY; 
    }
  } else if (y1 == y2) { 
    for (int x = min(x1, x2); x <= max(x1, x2); ++x) 
		{
      map.map[y1][x] = EMPTY; 
    }
  } else { 
    create_cooridor(map, x1, y1, x1, y2); 
    create_cooridor(map, x1, y2, x2, y2); 
  }
}

