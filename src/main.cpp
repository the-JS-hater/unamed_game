#include <stdio.h>
#include <raylib.h>
#include <stdint.h>

#define WORLD_W 10
#define WORLD_H 10


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
	//normalize movement speed at some point in the future
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
		DrawRectangleRec(player.hitbox, RED);

		EndDrawing();
	}

	CloseWindow();
}
