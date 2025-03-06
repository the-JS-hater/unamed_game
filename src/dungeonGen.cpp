#include "../inc/dungeonGen.hpp"

#define MIN_ROOM_SIZE 8 


BSPnode::BSPnode(int x, int y, int w, int h) :
x{x}, y{y}, w{w}, h{h}, left{nullptr}, right{nullptr}, room{0,0,0,0} {};


void generate_BSP(BSPnode* root, int const min_size) 
{
  if (root == nullptr) return;
 
	const bool can_split_along_x = (root->w >= (2 * min_size));
	const bool can_split_along_y = (root->h >= (2 * min_size));
  if (!can_split_along_x and !can_split_along_y) return;

  float ratio = static_cast<float>(root->h) / root->w;
  bool split_along_x;
  
  if (ratio > 1.75f) split_along_x = false;
  else if (ratio < 0.25f) split_along_x = true;
  else split_along_x = (rand() % 2 == 0);

  if (can_split_along_x and !can_split_along_y) split_along_x = true;
  if (!can_split_along_x and can_split_along_y) split_along_x = false;

  int split_pos;
  if (split_along_x) 
	{
    int buffer = root->w - 2 * min_size;
    split_pos = min_size + (buffer > 0 ? rand() % buffer : 0);
    root->left = new BSPnode(root->x, root->y, split_pos, root->h);
    root->right = new BSPnode(root->x + split_pos, root->y, root->w - split_pos, root->h);
  } 
	else 
	{
    int buffer = root->h - 2 * min_size;
    split_pos = min_size + (buffer > 0 ? rand() % buffer : 0);
    root->left = new BSPnode(root->x, root->y, root->w, split_pos);
    root->right = new BSPnode(root->x, root->y + split_pos, root->w, root->h - split_pos);
  }

  generate_BSP(root->left, min_size);
  generate_BSP(root->right, min_size);
}


void generate_dungeon(int w, int h, int min_size, TileMap& tile_map) 
{
  BSPnode* root = new BSPnode(0, 0, w, h);
  generate_BSP(root, min_size);
  generate_dungeon(root, tile_map);
}


void generate_dungeon(BSPnode* node, TileMap& tile_map) 
{
  if (node == nullptr) return;

  if (!node->left and !node->right) 
	{
    create_room(tile_map, node);
    return;
  }

  generate_dungeon(node->left, tile_map);
  generate_dungeon(node->right, tile_map);

  connect_nodes(tile_map, node->left, node->right);
}


void create_room(TileMap& tile_map, BSPnode* leaf) 
{
  const int min_room_size = MIN_ROOM_SIZE;

  int padding_x = rand() % (leaf->w - min_room_size);
  int padding_y = rand() % (leaf->h - min_room_size);

  padding_x = max(1, padding_x);
  padding_y = max(1, padding_y);

  int room_w = max(min_room_size, leaf->w - 2 * padding_x);
  int room_h = max(min_room_size, leaf->h - 2 * padding_y);

  int room_x = leaf->x + padding_x;
  int room_y = leaf->y + padding_y;

  leaf->room = {room_x, room_y, room_w, room_h};

  for (int y = room_y; y < room_y + room_h; ++y) 
	{
    for (int x = room_x; x < room_x + room_w; ++x) 
		{
      tile_map.map[y][x] = EMPTY;
    }
  }
}


void connect_nodes(TileMap& tile_map, BSPnode* left, BSPnode* right) 
{
  auto [x1, y1] = get_room_center(left);
  auto [x2, y2] = get_room_center(right);
	
  int width = 3 + rand() % 3;
  create_corridor(tile_map, x1, y1, x2, y2, width);
}


pair<int, int> get_room_center(BSPnode* node) 
{
  while (node->left) node = node->left; 
  
	return {
    node->room.x + node->room.width / 2,
    node->room.y + node->room.height / 2
  };
}


void create_corridor(
	TileMap& tile_map, 
	int x1, int y1, 
	int x2, int y2, 
	int width
) 
{
	if (x1 != x2) 
	{
    for (int x = min(x1, x2); x <= max(x1, x2); ++x)
		{
      for (int w = 0; w < width; ++w)
			{
				tile_map.map[y1+w][x] = EMPTY;
			}
		}
  }
  
	if (y1 != y2) 
	{
    for (int y = min(y1, y2); y <= max(y1, y2); ++y)
		{
      for (int w = 0; w < width; ++w)
			{
				tile_map.map[y][x2+w] = EMPTY;
			}
		}
  }
}

