#include <stdio.h>
#include <raylib.h>
#include "../inc/test.h"

int main(){
	// Make raylib not poop all over the terminal
	// TraceLogLevel enum in raylib.h
	SetTraceLogLevel(LOG_NONE);

	const int WINDOW_W = 1280;
	const int WINDOW_H = 720;

	InitWindow(WINDOW_W, WINDOW_H, "GAME");
	SetTargetFPS(60); //won't have to think about delta-time unless we do advances physics

	while (!WindowShouldClose())
	{
		test();
		// INPUT
		// todo...
		
		// UPDATE
		// todo...

		// RENDER
		BeginDrawing();
		// todo...
		EndDrawing();
	}

	CloseWindow();
}
