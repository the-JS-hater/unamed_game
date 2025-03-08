#include <cstdlib>
#include <ctime>
#include <cmath>
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
#include "../inc/vecUtils.hpp"
#include "../inc/pathfinding.hpp"
#include "../inc/player.hpp"


using std::vector;
using std::rand;
using std::make_pair;
using std::srand;
using std::time;


#define WINDOW_W 1280
#define WINDOW_H 720
#define WORLD_W 100
#define WORLD_H 100
#define DEBUG_CAM_ZOOM 0.1f //Lower -> zoom out
//WARN: prolly wanna tweak the macro in src/dungeonGen.cpp if your gonna touch
//this one
#define MIN_BSPNODE_SIZE 15 

#define NR_OF_TEST_ENTITIES 0
#define TILE_SIZE 32


// Will we ever had more here? WHO KNOWS!?
// prolly would be neat with some debug features idk
enum GameFlags 
{
	PAUSED 				= 1 << 0,
	FPS_VISIBLE		= 1 << 1,
	DEBUG_CAMERA 	= 1 << 2,
	FULLSCREEN 		= 1 << 3,
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


void init(int& flags)
{
	// Make raylib not poop all over the terminal
	// TraceLogLevel enum in raylib.h
	SetTraceLogLevel(LOG_NONE);
	InitWindow(WINDOW_W, WINDOW_H, "GAME");
	//won't have to think about delta-time unless we do advances physics
	SetTargetFPS(60); 
	
	srand(time(nullptr));

	flags |= FPS_VISIBLE;
	//flags |= DEBUG_CAMERA;
	//flags |= FULLSCREEN;
}
	
	
void gen_test_entities(ECS& ecs, Quadtree& quadtree, TileMap const& tile_map)
{	
	Texture2D test_tex = LoadTexture("resources/sprites/Spam.png");
	for (int i {0}; i < NR_OF_TEST_ENTITIES; i++) {
		Entity id = ecs.allocate_entity();
			
		Vector2 pos = get_random_spawn_location(tile_map);

		ecs.set_sprite(id, test_tex, WHITE);
		ecs.set_boxCollider(id, (Rectangle){pos.x, pos.y, 32.0f, 32.0f});
		
		float rand_vx = rand()%20 - 10;
		float rand_vy = rand()%20 - 10;
		ecs.set_velocity(id, (Vector2){rand_vx, rand_vy}, 5.0f);
		ecs.set_acceleration(id, (Vector2){0.0f, 0.0f}, 0.95f);
		ecs.set_aiComponent(id);
	}
}


int main()
{
	// usage: if (flags & SOME_FLAG), note the bitwise and
	int flags;
	init(flags);

	// Regular player following camera
	Camera2D camera = {
		(Vector2){WINDOW_W / 2, WINDOW_H / 2}, 
		(Vector2){0.0f, 0.0f}, 
		0.0f, 
		1.0f
	};

	// Wont follow the player, centered at the middle of the world. Very zoomed
	// out
	Camera2D debug_camera = {
		(Vector2){WINDOW_W / 2, WINDOW_H / 2}, 
		(Vector2){WORLD_W / 2 * TILE_SIZE, WORLD_H / 2 * TILE_SIZE}, 
		0.0f, 
		DEBUG_CAM_ZOOM
	};
	
	TileMap world_map = TileMap(WORLD_W, WORLD_H);
	world_map.init_map_textures();
	generate_dungeon(WORLD_W, WORLD_H, MIN_BSPNODE_SIZE, world_map);

	ECS ecs; //calls default constructor >:{
	Quadtree quadtree = Quadtree(0, (Rectangle){0, 0, WORLD_W, WORLD_H});
	vector<pair<Entity, Entity>> collisions;
	
	Player player = init_player(ecs, world_map);
	Texture2D cursor_tex = LoadTexture("resources/sprites/Crosshair.png");
	Texture2D bullet_tex = LoadTexture("resources/sprites/Bullet.png");

	FlowField flow_field = FlowField(world_map);
	gen_test_entities(ecs, quadtree, world_map);
	
	HideCursor();
	
	/* GAME LOOP */
	while (not WindowShouldClose())
	{
		// believe it or not... it works
		// XOR compound assignment
		if (IsKeyPressed(KEY_P)) flags ^= PAUSED;
		if (IsKeyPressed(KEY_F)) flags ^= FULLSCREEN;
			
		if ((flags & FULLSCREEN) and !IsWindowFullscreen()) ToggleFullscreen();
		if (!(flags & FULLSCREEN) and IsWindowFullscreen()) ToggleFullscreen();
		
		while (flags & PAUSED)
		{
			BeginDrawing();
			ClearBackground(BLACK);

			render_pause_screen();
			
			EndDrawing();

			if (IsKeyPressed(KEY_P)) flags ^= PAUSED;
		}

		// INPUT
		move_player(ecs, player);
		fire_gun(ecs, bullet_tex, player);
		
		// UPDATE
		
		update_lifecycles(ecs);

		int player_x, player_y; //must be grid idx:es
		player_x = static_cast<int>(ecs.box_colliders[player.id].hitbox.x / TILE_SIZE);
		player_y = static_cast<int>(ecs.box_colliders[player.id].hitbox.y / TILE_SIZE);
		flow_field.update_cost_field(player_x, player_y);
		flow_field.update_flow_field();
		update_ai_entities(ecs, world_map, flow_field);

		
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

		update_velocities(ecs);
		
		handle_collisions(collisions, ecs);
		handle_wall_collisions(ecs, world_map);
		
		update_box_colliders(ecs);
		
		if (!(flags & DEBUG_CAMERA)) update_player_camera(camera, ecs, player);

		// RENDER
		// i need to refactor this as soon as possible, fuck this shit
		BeginDrawing();
		(flags & DEBUG_CAMERA) ? BeginMode2D(debug_camera) : BeginMode2D(camera);
		ClearBackground(BLACK);
		
		//debug_render_costfield(flow_field);
		//debug_render_flowfield(flow_field);
		//debug_render_quadtree(&quadtree);
		//debug_draw_hitboxes(ecs);
		debug_draw_dungeon(world_map);
		render_sprites(ecs);
		//debug_render_collisions(collisions, ecs);

		EndMode2D();
		// Draw things that are relative to screen coordinates, and not world
		// coordinates i.e GUI stuff
		render_corshair(cursor_tex);
		
		if (flags & FPS_VISIBLE) DrawFPS(10, 10);

		EndDrawing();
	}

	CloseWindow();
}
