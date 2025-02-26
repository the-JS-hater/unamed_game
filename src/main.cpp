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


using std::vector;
using std::rand;


#define WINDOW_W 1280
#define WINDOW_H 720
#define WORLD_W 10000
#define WORLD_H 10000
#define NR_OF_TEST_ENTITIES 2000
#define PLAYER_ACC 1.0f
#define PLAYER_SPEED 6.0f
#define BULLET_SPEED 12.0f
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


// This will hold a sort of refrence to the entity ID of the player, but also
// hold auxilliary data that doesn't fit in, or belong to, the ECS
struct Player
{
	Entity id;
	float max_speed;
	Vector2 acc_vec;

	Player(Entity id, float x)
		: id{id}, max_speed{x}, acc_vec{(Vector2){0.0f, 0.0f}} {};
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

	flags |= FPS_VISIBLE;
	//flags |= DEBUG_CAMERA;
	//flags |= FULLSCREEN;
}
	
	
Player init_player(ECS& ecs, TileMap const& tile_map)
{
	Entity player_id = ecs.allocate_entity();
	Texture2D player_tex = LoadTexture("resources/sprites/DuckHead.png");

	ecs.set_sprite(player_id, player_tex, WHITE);
	Vector2 pos = get_random_spawn_location(tile_map);
	ecs.set_boxCollider(player_id, (Rectangle){pos.x * TILE_SIZE, pos.y * TILE_SIZE, 32.0f, 32.0f});
	ecs.set_velocity(player_id, (Vector2){0.0f, 0.0f});

	return Player(player_id, PLAYER_SPEED);
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


void move_player(ECS& ecs, Player& player)
{
	Vector2& velV = ecs.velocities[player.id].deltaV; 
	Vector2& accV = player.acc_vec;
	
	accV = {0.0f, 0.0f};
	
	if (IsKeyDown(KEY_W)) accV.y = -PLAYER_ACC;
	if (IsKeyDown(KEY_A)) accV.x = -PLAYER_ACC;
	if (IsKeyDown(KEY_S)) accV.y = +PLAYER_ACC;
	if (IsKeyDown(KEY_D)) accV.x = +PLAYER_ACC;
	
	velV.x += accV.x;
	velV.y += accV.y;
	
	if (accV.x == 0.0f and velV.x > 0.0f) velV.x -= PLAYER_ACC;
	if (accV.x == 0.0f and velV.x < 0.0f) velV.x += PLAYER_ACC;
	if (accV.y == 0.0f and velV.y > 0.0f) velV.y -= PLAYER_ACC;
	if (accV.y == 0.0f and velV.y < 0.0f) velV.y += PLAYER_ACC;
	
	// Stop if your slow as fuck
	if (0.0f <= velV.x and velV.x <= 0.9f) velV.x = 0.0f;
	if (0.0f <= velV.y and velV.y <= 0.9f) velV.y = 0.0f;
	if (0.0f >= velV.x and velV.x >= -0.9f) velV.x = 0.0f;
	if (0.0f >= velV.y and velV.y >= -0.9f) velV.y = 0.0f;
		
	// woah, slow down there buckaroo
	if (velV.x > player.max_speed) velV.x = player.max_speed; 
	if (velV.y > player.max_speed) velV.y = player.max_speed; 
	if (velV.x < -player.max_speed) velV.x = -player.max_speed; 
	if (velV.y < -player.max_speed) velV.y = -player.max_speed;
}


void update_camera(Camera2D& cam, ECS const& ecs)
{
	cam.target = (Vector2){
		ecs.box_colliders[0].hitbox.x,
		ecs.box_colliders[0].hitbox.y
	};
}


void render_cursor(Texture2D& tex)
{
	Vector2 pos = GetMousePosition();
	DrawTextureV(tex, pos, WHITE);
}
	

void fire_gun(ECS& ecs, Texture2D& tex, Player& player)
{	
	if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
	
	Vector2 target_pos = GetMousePosition();
  float pos_x = static_cast<float>(GetScreenWidth() / 2);
  float pos_y = static_cast<float>(GetScreenHeight() / 2);
	Vector2 pos = {pos_x, pos_y};
	Vector2 target_dir = scale(normalize(sub(target_pos, pos)), BULLET_SPEED);

	// Player position is in world coordinates, pos is in screen coordinates
	Vector2 spawn_pos = {
		ecs.box_colliders[player.id].hitbox.x,
		ecs.box_colliders[player.id].hitbox.y
	};
	
	Entity id = ecs.allocate_entity();
	ecs.set_sprite(id, tex, WHITE);
	ecs.set_velocity(id, target_dir);
	ecs.set_boxCollider(id, (Rectangle){
		spawn_pos.x + target_dir.x * 3.0f, 
		spawn_pos.y + target_dir.y * 3.0f, 
		16.0f, 
		16.0f
	});
}


int main()
{
	// usage: if (flags & SOME_FLAG), note the bitwise and
	int flags;

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
		(Vector2){WORLD_W / 2, WORLD_H / 2}, 
		0.0f, 
		0.1f
	};

	TileMap test_map = generate_dungeon(WORLD_W / TILE_SIZE, WORLD_H/ TILE_SIZE, 10);
	ECS ecs; //calls default constructor
	Quadtree quadtree = Quadtree(0, (Rectangle){0, 0, WORLD_W, WORLD_H});
	vector<pair<Entity, Entity>> collisions;

	init(flags);
	
	Player player = init_player(ecs, test_map);
	Texture2D cursor_tex = LoadTexture("resources/sprites/Crosshair.png");
	Texture2D bullet_tex = LoadTexture("resources/sprites/Bullet.png");

	//gen_test_entities(ecs, quadtree, test_map);
	
	if (flags & FULLSCREEN) ToggleFullscreen();
	HideCursor();
	
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
		move_player(ecs, player);
		fire_gun(ecs, bullet_tex, player);
		
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
		
		if (!(flags & DEBUG_CAMERA)) update_camera(camera, ecs);

		// RENDER
		BeginDrawing();
		(flags & DEBUG_CAMERA) ? BeginMode2D(debug_camera) : BeginMode2D(camera);
		ClearBackground(WHITE);

		debug_draw_dungeon(test_map, TILE_SIZE);
		//debug_render_quadtree(&quadtree);
		//debug_draw_hitboxes(ecs);
		render_sprites(ecs);
		//debug_render_collisions(collisions, ecs);

		EndMode2D();
		// Draw things that are relative to screen coordinates, and not world
		// coordinates i.e GUI stuff
		render_cursor(cursor_tex);
		if (flags & FPS_VISIBLE) DrawFPS(10, 10);

		EndDrawing();
	}

	CloseWindow();
}
