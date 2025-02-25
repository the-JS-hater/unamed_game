#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <raylib.h>
#include <stdint.h>
#include <vector>
#include <utility>
#include "../inc/ecs.hpp"
#include "../inc/dungeonGen.hpp"
#include "../inc/quadtree.hpp"
#include "../inc/collisionSystem.hpp"
#include "../inc/tileMap.hpp"


using std::vector;
using std::rand;


#define WINDOW_W 1280
#define WINDOW_H 720
#define WORLD_W 2000
#define WORLD_H 2000
#define NR_OF_TEST_ENTITIES 1000
#define PLAYER_SPEED 5.0f
#define TILE_SIZE 32


// Will we ever had more here? WHO KNOWS!?
// prolly would be neat with some debug features idk
enum GameFlags 
{
	PAUSED 			= 1 << 0,
	FPS_VISIBLE	= 1 << 1,
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
	flags |= FPS_VISIBLE;
	ecs = *new ECS();
}
	
	
void init_player(ECS& ecs)
{
	Entity player_id;
	if (ecs.entity_count == 0) player_id = ecs.allocate_entity();
	else player_id = 0;

	Texture2D player_tex = LoadTexture("resources/sprites/DuckHead.png");

	ecs.set_sprite(player_id, player_tex, WHITE);
	ecs.set_boxCollider(player_id, (Rectangle){WORLD_W / 2, WORLD_H / 2, 32.0f, 32.0f});
	ecs.set_velocity(player_id, (Vector2){0.0f, 0.0f});

	//NOTE: touching the flag sets directly like this is probably a pretty bad
	//idea...
	ecs.flag_sets[0] = (SPRITE | BOX_COLLIDER | VELOCITY);
}
	
	
void gen_test_entities(ECS& ecs, Quadtree& quadtree, TileMap const& tile_map)
{	
	Texture2D test_tex = LoadTexture("resources/sprites/Spam.png");
	for (int i {0}; i < NR_OF_TEST_ENTITIES; i++) {
		Entity id = ecs.allocate_entity();
			
		Vector2 pos = get_random_spawn_location(tile_map);

		ecs.set_sprite(id, test_tex, WHITE);
		ecs.set_boxCollider(id, (Rectangle){pos.x * TILE_SIZE - 15.9f, pos.y * TILE_SIZE - 15.9f, 32.0f, 32});
		
		float rand_vx = rand()%20 - 10;
		float rand_vy = rand()%20 - 10;
		ecs.set_velocity(id, (Vector2){rand_vx, rand_vy});
	}
}


void move_player(ECS& ecs)
{
	//NOTE: id 0 is assumed to be the player
	Vector2* vec = &ecs.velocities[0].deltaV;

	//ecs.velocities[0].deltaV.x = 0.0f;
	//ecs.velocities[0].deltaV.y = 0.0f;

	if (IsKeyDown(KEY_W)) vec->y = -PLAYER_SPEED;
	if (IsKeyDown(KEY_A)) vec->x = -PLAYER_SPEED;
	if (IsKeyDown(KEY_S)) vec->y = +PLAYER_SPEED;
	if (IsKeyDown(KEY_D)) vec->x = +PLAYER_SPEED;
}


void update_camera(Camera2D& cam, ECS const& ecs)
{
	cam.target = (Vector2){
		ecs.box_colliders[0].hitbox.x,
		ecs.box_colliders[0].hitbox.y
	};
}


int main()
{
	// used as a bitset alongside GameFlags enum
	// usage: if (flags & SOME_FLAG), note the bitwise and
	// perhaps it's possible to bundle this bitset with the enum in a struct?
	uint8_t flags;
	Camera2D camera = {(Vector2){WINDOW_W / 2,WINDOW_H / 2}, (Vector2){0.0f, 0.0f}, 0.0f, 2.0f};
	TileMap test_map = generate_dungeon(WORLD_W / TILE_SIZE, WORLD_H/ TILE_SIZE, 10);
	ECS ecs;
	Quadtree quadtree = Quadtree(0, (Rectangle){0, 0, WORLD_W, WORLD_H});
	vector<pair<Entity, Entity>> collisions;
	
	init(flags, ecs);
	init_player(ecs);
	
	gen_test_entities(ecs, quadtree, test_map);

	while (not WindowShouldClose())
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
		move_player(ecs);
		
		// UPDATE
		//NOTE: could be optimized in such a way as to not having to regenerate the
		//whole quadtree every update
		quadtree.clear();
		for (auto id: ecs.entities) 
		{	
			if (id == -1) continue;
			quadtree.insert(ecs, id);
		}
		//NOTE: O(n) complexity, idk if there's a sensible way to avoid doing it
		collisions.clear();
		find_all_intersections(&quadtree, collisions, ecs);
		
		handle_collisions(collisions, ecs);
		handle_wall_collisions(ecs, test_map);
		
		update_box_colliders(ecs);
		update_camera(camera, ecs);

		// RENDER
		BeginDrawing();
		BeginMode2D(camera);
		ClearBackground(WHITE);
	
		debug_draw_dungeon(test_map, TILE_SIZE);
		//debug_render_quadtree(&quadtree);
		debug_draw_hitboxes(ecs);
		//render_sprites(ecs);
		debug_render_collisions(collisions, ecs);

		EndMode2D();
		// Draw things that are relative to screen coordinates, and not world
		// coordinates i.e GUI stuff
		if (flags & FPS_VISIBLE) DrawFPS(10, 10);

		EndDrawing();
	}

	CloseWindow();
}
