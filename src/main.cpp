#include <stdio.h>
#include <raylib.h>
#include <stdint.h>

#define WORLD_W 10
#define WORLD_H 10


enum GameFlags {
	PAUSED = 1 << 0,
	MAPEDITOR = 1 << 1,
};


enum TileType {
	GRASS,
	ROCK,
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

	Player() : hitbox{(Rectangle){0, 0, 20, 40}} {}
};


void move(Player& player)
{
	if (IsKeyDown(KEY_W))
	{
		player.hitbox.y -= 1;
	}
	if (IsKeyDown(KEY_A))
	{
		player.hitbox.x -= 1;
	}
	if (IsKeyDown(KEY_S))
	{
		player.hitbox.y += 1;
	}
	if (IsKeyDown(KEY_D))
	{
		player.hitbox.x += 1;
	}
}


int main(){
	// Make raylib not poop all over the terminal
	// TraceLogLevel enum in raylib.h
	SetTraceLogLevel(LOG_NONE);

	const int WINDOW_W = 1280;
	const int WINDOW_H = 720;
	
	InitWindow(WINDOW_W, WINDOW_H, "GAME");
	SetTargetFPS(60); //won't have to think about delta-time unless we do advances physics

	Player player;
	TileMap world;
	// used as a bitset alongside GameFlags enum
	// usage: if (flags & SOME_FLAG), note the bitwise and
	uint8_t flags = 0; 

	while (!WindowShouldClose())
	{
		// believe it or not... it works
		if (IsKeyPressed(KEY_P)) flags ^= PAUSED;
		if (flags & PAUSED) printf("Game is paused\n");	
		if (!(flags & PAUSED)) printf("Game is running\n");	

		// INPUT
		move(player);
		
		// UPDATE
		// todo...

		// RENDER
		BeginDrawing();
		
		ClearBackground(BLACK);
		DrawRectangleRec(player.hitbox, RED);

		EndDrawing();
	}

	CloseWindow();
}
