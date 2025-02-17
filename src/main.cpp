#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <raylib.h>
#include <stdint.h>
#include <vector>
#include <cstdlib>
#include "../inc/ecs.hpp"
#include "../inc/dungeonGen.hpp"


using std::vector;
using std::rand;


#define WINDOW_W 1280
#define WINDOW_H 720


enum GameFlags 
{
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


void render_pause_screen()
{
	const char* PAUSED_TEXT = "GAME IS PAUSED";
	const int FONT_SIZE = 30;
	const int TEXT_OFFSET = MeasureText(PAUSED_TEXT, FONT_SIZE);

	DrawText(
		PAUSED_TEXT, 
		WINDOW_W / 2 - TEXT_OFFSET / 2, 
		WINDOW_H / 2 - FONT_SIZE / 2, 
		FONT_SIZE, 
		RED
	);
}


void init(uint8_t& flags, ECS& ecs)
{
	// Make raylib not poop all over the terminal
	// TraceLogLevel enum in raylib.h
	SetTraceLogLevel(LOG_NONE);
	InitWindow(WINDOW_W, WINDOW_H, "GAME");
	//won't have to think about delta-time unless we do advances physics
	SetTargetFPS(60); 

	flags = 0;
	ecs = *new ECS();
}


int main()
{
  srand(time(0));
  // What are memory leaks?
  TreeNode *root = new TreeNode{0, 0, WINDOW_W, WINDOW_H};
  generate_tree(root);
	
	// used as a bitset alongside GameFlags enum
	// usage: if (flags & SOME_FLAG), note the bitwise and
	// perhaps it's possible to bundle this bitset with the enum in a struct?
	uint8_t flags;
	ECS ecs;
	init(flags, ecs);
	
	//THIS IS JUST FOR TESTING
	Texture2D test_tex = LoadTexture("resources/sprites/Spam.png");
	for (int i {0}; i < 1000; i++) {
		Entity id = ecs.allocate_entity();
		float rand_x = rand()%WINDOW_W;
		float rand_y = rand()%WINDOW_H;

		ecs.set_position(id, (Vector2){rand_x, rand_y});
		ecs.set_sprite(id, test_tex, WHITE);
		
		// 50% chance
		if ((rand() % 2) + 1 <= 2)
		{
			float rand_vx = rand()%20 - 10;
			float rand_vy = rand()%20 - 10;
			ecs.set_velocity(id, (Vector2){rand_vx, rand_vy});
		}
	}

	while (!WindowShouldClose())
	{
		// believe it or not... it works
		// XOR compound assignment
		if (IsKeyPressed(KEY_P)) flags ^= PAUSED;
		
		while (flags & PAUSED)
		{
			BeginDrawing();
			ClearBackground(BLACK);

			render_pause_screen();
			
			EndDrawing();

			if (IsKeyPressed(KEY_P)) flags ^= PAUSED;
		}

		// INPUT
		// todo...
		
		// UPDATE
		update_positions(ecs);

		// RENDER
		BeginDrawing();
		ClearBackground(BLACK);

    draw_tree(root);
		
		render_sprites(ecs);
		
		DrawFPS(10, 10);
		EndDrawing();
	}

  delete_tree(root);
	CloseWindow();
}
