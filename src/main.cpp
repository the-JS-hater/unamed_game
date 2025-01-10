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
};


struct TileMap {
	TileType map[WORLD_H][WORLD_W];

	TileMap() 
	{
		for (int y = 0; y < WORLD_H; y++) {
			for (int x = 0; x < WORLD_W; x++) {
				map[y][x] = GRASS;
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

	CloseWindow();
}
