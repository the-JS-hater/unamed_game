#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <raylib.h>
#include <stdint.h>
#include <vector>

#define WORLD_W 256
#define WORLD_H 256

enum GameFlags {
	PAUSED = 1 << 0,
	MAPEDITOR = 1 << 1, //TODO
	//current bitset is 8bit, so there are more possible flags
	UNUSEDFLAG = 1 << 2,	
	UNUSEDFLAG1 = 1 << 3,
	UNUSEDFLAG2 = 1 << 4,
	UNUSEDFLAG3 = 1 << 5,
	UNUSEDFLAG4 = 1 << 6,
	UNUSEDFLAG5 = 1 << 7,
};


enum TileType {
	GRASS,
  DIRT,
};

struct TileMap {
	TileType map[WORLD_H][WORLD_W];

	TileMap() 
	{
		for (int y = 0; y < WORLD_H; y++) {
			for (int x = 0; x < WORLD_W; x++) {
        if (x % 2 == 0 && y % 2 == 1) {
          map[y][x] = DIRT;
        } else if (x % 2 == 1 && y % 2 == 0) {
          map[y][x] = DIRT;
        } else {
				  map[y][x] = GRASS;
        }
			}
		}
	}
};


struct Player {
	Rectangle hitbox;
	float speed;

	Player() : hitbox{(Rectangle){0, 0, 20, 40}}, speed{1.0f} {}
};


enum EntityType {
	TREE,
	ROCK,
};


struct Entity {
	//entity ID?
	Rectangle hitbox;
	Vector2 moveVec;
	int entityFlags;
	EntityType entityType;
};


struct TreeNode {
  int x;
  int y;
  int width;
  int height;

  Color color;

  TreeNode* left;
  TreeNode* right;
};


void generate_tree(TreeNode *root) {
  TreeNode *left, *right;
  int cut;
  // Vertical or horizontal cut.
  if (rand() % 2 == 0) {
    // Random cut along the x axis.
    cut = rand() % root->width;
    left = new TreeNode {root->x, root->y, cut, root->height, RED};  
    right = new TreeNode {cut, root->y, root->width, root->height, GREEN};  
  } else {
    // Random cut along the y axis.
    cut = rand() % root->height;
    left = new TreeNode {root->x, root->y, root->width, cut, BLUE};  
    right = new TreeNode {root->x, cut, root->width, root->height, GRAY};
  }

  if (rand() % 100 < 40) {
    generate_tree(left);
  } else {
    left->left = nullptr;
    left->right = nullptr;
  }


  if (rand() % 100 < 40) {
    generate_tree(right);
  } else {
    right->left = nullptr;
    right->right = nullptr;
  }

  root->left = left;
  root->right = right;
}


void draw_tree(TreeNode *root) {
  // Draw bg color.
  DrawRectangle(root->x, root->y, root->width, root->height, root->color);
  // Draw border.
  DrawRectangleLines(root->x, root->y, root->width, root->height, BLACK);

  if (root->left != nullptr) draw_tree(root->left);
  if (root->right != nullptr) draw_tree(root->right);
}


void delete_tree(TreeNode *root) {
  if (root->left != nullptr) delete_tree(root->left);
  if (root->right != nullptr) delete_tree(root->right);

  delete root;
};


void move(Player& player)
{
	//normalize movement speed at some point in the future?
	
	if (IsKeyDown(KEY_W))
	{
		player.hitbox.y -= player.speed;
	}
	if (IsKeyDown(KEY_A))
	{
		player.hitbox.x -= player.speed;
	}
	if (IsKeyDown(KEY_S))
	{
		player.hitbox.y += player.speed;
	}
	if (IsKeyDown(KEY_D))
	{
		player.hitbox.x += player.speed;
	}
}


void draw_map(TileMap& tileMap) {
  int x, y;

  for (y = 0; y < WORLD_H; y++) {
    for (x = 0; x < WORLD_W; x++) {
      switch (tileMap.map[y][x]) {
        case GRASS: 
          DrawRectangle(x * 16, y * 16, 16, 16, GREEN);
          break;
        case DIRT:
          DrawRectangle(x * 16, y * 16, 16, 16, BROWN);
          break;
      }
    }
  }
}


int main(){
	const int WINDOW_W = 1280;
	const int WINDOW_H = 720;
	
	// Make raylib not poop all over the terminal
	// TraceLogLevel enum in raylib.h
	SetTraceLogLevel(LOG_NONE);
	InitWindow(WINDOW_W, WINDOW_H, "GAME");
	SetTargetFPS(60); //won't have to think about delta-time unless we do advances physics

	Player player;
	TileMap world;
	std::vector<Entity> entities;

  srand(time(0));
  // What are memory leaks?
  TreeNode *root = new TreeNode{0, 0, WINDOW_W, WINDOW_H};
  generate_tree(root);

	entities.push_back(
		Entity {
			(Rectangle) {50, 40, 10, 80},
			(Vector2) {0, 0},
			0,
			TREE
		}
	);
	entities.push_back(
		Entity {
			(Rectangle) {90, 110, 10, 10},
			(Vector2) {0, 0},
			0,
			ROCK	
		}
	);

	// used as a bitset alongside GameFlags enum
	// usage: if (flags & SOME_FLAG), note the bitwise and
	// perhaps it's possible to bundle this bitset with the enum in a struct?
	uint8_t flags = 0; 
	
	const char* PAUSED_TEXT = "GAME IS PAUSED";
	const int FONT_SIZE = 30;
	const int TEXT_OFFSET = MeasureText(PAUSED_TEXT, FONT_SIZE);

	while (!WindowShouldClose())
	{
		// believe it or not... it works
		if (IsKeyPressed(KEY_P)) flags ^= PAUSED;
		
		while (flags & PAUSED)
		{
			BeginDrawing();
			ClearBackground(BLACK);
			
			DrawText(
				PAUSED_TEXT, 
				WINDOW_W / 2 - TEXT_OFFSET / 2, 
				WINDOW_H / 2 - FONT_SIZE / 2, 
				FONT_SIZE, 
				RED
			);

			EndDrawing();

			if (IsKeyPressed(KEY_P)) flags ^= PAUSED;
		}

		// INPUT
		move(player);
		
		// UPDATE
		// todo...

		// RENDER
		BeginDrawing();
		ClearBackground(BLACK);

    draw_tree(root);
		
		for (Entity entity : entities)
		{	
			switch (entity.entityType)
			{
				case TREE: {
					DrawRectangleRec(entity.hitbox, GREEN); 
					break;
				}
				case ROCK: {
					DrawRectangleRec(entity.hitbox, GRAY); 
					break;
				} 
			}
		}

		DrawRectangleRec(player.hitbox, RED);

		EndDrawing();
	}

  delete_tree(root);
	CloseWindow();
}
