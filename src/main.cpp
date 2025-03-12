#include <cstdlib>
#include <ctime>
#include <cmath>
#include <stdio.h>
#include <raylib.h>
#include <stdint.h>
#include <vector>
#include <utility>
#include <functional>
#include "../inc/ecs.hpp"
#include "../inc/dungeonGen.hpp"
#include "../inc/quadtree.hpp"
#include "../inc/collisionSystem.hpp"
#include "../inc/tileMap.hpp"
#include "../inc/vecUtils.hpp"
#include "../inc/pathfinding.hpp"
#include "../inc/player.hpp"

#include <stdio.h>

using std::vector;
using std::rand;
using std::make_pair;
using std::srand;
using std::time;
using std::function;

#define WINDOW_W 1280
#define WINDOW_H 720
#define WORLD_W 100
#define WORLD_H 100 
#define DEBUG_CAM_ZOOM 0.1f //Lower -> zoom out
#define NR_OF_TEST_ENTITIES 100 
#define TILE_SIZE 32

//WARN: prolly wanna tweak the macro in src/dungeonGen.cpp if your gonna touch
//this one
#define MIN_BSPNODE_SIZE 15 

Texture2D cursor_tex;  
Texture2D bullet_tex;
Texture2D spam_tex;
Texture2D duckhead_tex;
Texture2D duckbody_tex;
Texture2D explosion_tex;

	
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
	SetTargetFPS(60); 
	
	cursor_tex 		= LoadTexture("resources/sprites/Crosshair.png");
	bullet_tex 		= LoadTexture("resources/sprites/Bullet.png");
	spam_tex 			= LoadTexture("resources/sprites/Spam.png");
 	duckhead_tex	= LoadTexture("resources/sprites/DuckHead.png");
 	duckbody_tex	= LoadTexture("resources/sprites/DuckBody.png");
 	explosion_tex	= LoadTexture("resources/sprites/Explosion.png");
	
	srand(time(nullptr));

	flags |= FPS_VISIBLE;
	//flags |= DEBUG_CAMERA;
	//flags |= FULLSCREEN;
}
	

	
void gen_test_entities(ECS& ecs, TileMap const& tile_map)
{	
	function<void(Entity, ECS&)> awesome_callback(
		[&explosion_tex, &duckhead_tex, &duckbody_tex](Entity id, ECS& ecs)
		{
			Entity explosion_id = ecs.allocate_entity();
			Entity head_id = ecs.allocate_entity();
			Entity body_id = ecs.allocate_entity();

			ecs.set_lifecycle(explosion_id, 30);
			ecs.set_lifecycle(head_id, 120);
			ecs.set_lifecycle(body_id, 120);
			
			float x = ecs.positions[id].x;
			float y = ecs.positions[id].y;
			ecs.set_position(explosion_id, x, y);
			ecs.set_position(head_id, x, y);
			ecs.set_position(body_id, x, y);
	
			float dx1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float dx2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float dy1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float dy2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

			ecs.set_velocity(
				head_id,
				(Vector2){dx1, dy1},
				9.0f,
				0.1f
			);
			ecs.set_velocity(
				body_id,
				(Vector2){dx2, dy2},
				9.0f,
				0.1f
			);
			ecs.set_sprite(explosion_id, explosion_tex, WHITE);
			ecs.set_sprite(head_id, duckhead_tex, WHITE);
			ecs.set_sprite(body_id, duckbody_tex, WHITE);
		}
	); 

	function<void(Entity, ECS&)> test_callback([](Entity, ECS&){printf("Goodbye cruel world!\n");});

	for (int i {0}; i < NR_OF_TEST_ENTITIES; i++) 
	{
		Entity id = ecs.allocate_entity();
			
		Vector2 pos = get_random_spawn_location(tile_map);
		ecs.set_sprite(id, spam_tex, WHITE);
		ecs.set_position(id, pos.x, pos.y);
		ecs.set_boxCollider(id, (Rectangle){pos.x, pos.y, 32.0f, 32.0f});
		
		float rand_vx = rand()%20 - 10;
		float rand_vy = rand()%20 - 10;
		ecs.set_velocity(id, (Vector2){rand_vx, rand_vy}, 5.0f, 0.95f);
		ecs.set_acceleration(id, (Vector2){0.0f, 0.0f});
		ecs.set_aiComponent(id);
		ecs.set_mass(id, 1000.0f);
		ecs.set_health(id, 1.0f);
		ecs.register_on_death(id, awesome_callback);
		//ecs.register_on_death(id, test_callback);
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

	FlowField flow_field = FlowField(world_map);
	gen_test_entities(ecs, world_map);
	
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
		fire_gun(ecs, bullet_tex, player, world_map);
		
		// UPDATE
		
		update_health(ecs);
		update_lifecycles(ecs);

		int player_x, player_y; //must be grid idx:es
		player_x = static_cast<int>(ecs.positions[player.id].x / TILE_SIZE);
		player_y = static_cast<int>(ecs.positions[player.id].y / TILE_SIZE);
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
		
		update_positions(ecs);
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
