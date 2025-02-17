#include <vector>
#include <cstdlib>
#include <algorithm>


using std::vector;
using std::min;
using std::max;


template <typename T> 
using matrix = vector<vector<T>>;


enum Tiles
{
	EMPTY,
	WALL
};


struct TileMap
{
	size_t height;
	size_t width;	
	matrix<Tiles> map;

	TileMap(unsigned, unsigned);
};


struct BSPnode	
{
	int x;
	int y;
	int w;
	int h;
	BSPnode* left;
	BSPnode* right;

	BSPnode(int, int, int, int);
};

void generate_BSP(BSPnode*, int);

TileMap generate_dungeon(unsigned, unsigned, unsigned);

bool can_split(int, int, int);

void create_cooridor(TileMap&, int, int, int, int);

void create_room(TileMap&, unsigned, unsigned, int, int); 

