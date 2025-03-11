#include "../inc/player.hpp"

#define PLAYER_ACC 1.0f
#define PLAYER_RET 0.8f
#define PLAYER_SPEED 6.0f
#define BULLET_SPEED 20.0f
#define BULLET_WEIGHT 1200.0f
#define CORSHAIR_SCALE 150.0f

Player::Player(Entity id): id{id} {};


Player init_player(ECS& ecs, TileMap const& tile_map)
{
	Entity player_id = ecs.allocate_entity();
	Texture2D player_tex = LoadTexture("resources/sprites/DuckHead.png");

	ecs.set_sprite(player_id, player_tex, WHITE);
	Vector2 pos = get_random_spawn_location(tile_map);
	ecs.set_boxCollider(player_id, (Rectangle){pos.x, pos.y, 32.0f, 32.0f});
	ecs.set_velocity(player_id, (Vector2){0.0f, 0.0f}, PLAYER_SPEED);
	ecs.set_acceleration(player_id, (Vector2){0.0f, 0.0f}, PLAYER_RET);
	ecs.set_mass(player_id, 1000.0f);

	return Player(player_id);
}


void move_player(ECS& ecs, Player& player)
{
	Vector2& velV = ecs.velocities[player.id].deltaV; 
	Vector2& accV = ecs.accelerations[player.id].accV;
	
	accV = {0.0f, 0.0f};
	
	if (IsKeyDown(KEY_W)) accV.y = -PLAYER_ACC;
	if (IsKeyDown(KEY_A)) accV.x = -PLAYER_ACC;
	if (IsKeyDown(KEY_S)) accV.y = +PLAYER_ACC;
	if (IsKeyDown(KEY_D)) accV.x = +PLAYER_ACC;
}
	

void fire_gun(ECS& ecs, Texture2D& tex, Player& player, TileMap const& world)
{	
	if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
	
	Vector2 target_pos = GetMousePosition();
  float pos_x = static_cast<float>(GetScreenWidth() / 2);
  float pos_y = static_cast<float>(GetScreenHeight() / 2);
	Vector2 pos = {pos_x, pos_y};
	Vector2 target_dir = scale(normalize(get_cursor_dir()), BULLET_SPEED);

	// Player position is in world coordinates, pos is in screen coordinates
	Vector2 spawn_pos = {
		ecs.box_colliders[player.id].hitbox.x,
		ecs.box_colliders[player.id].hitbox.y
	};

	Rectangle aabb = (Rectangle){
		spawn_pos.x + target_dir.x * 2.2f, 
		spawn_pos.y + target_dir.y * 2.2f, 
		16.0f, 
		16.0f
	};

	if (has_wall_collision(aabb, world)) return;

	Entity id = ecs.allocate_entity();
	ecs.set_sprite(id, tex, WHITE);
	ecs.set_velocity(id, target_dir, 3.0f);
	ecs.set_boxCollider(id, aabb);
	ecs.set_lifecycle(id, 600);
	ecs.set_mass(id, BULLET_WEIGHT);
	ecs.set_damage(id, 0.5f);
}


void update_player_camera(Camera2D& cam, ECS const& ecs, Player const& player)
{
	cam.target = (Vector2){
		ecs.box_colliders[player.id].hitbox.x,
		ecs.box_colliders[player.id].hitbox.y
	};
	Vector2 mid_point = (Vector2){GetScreenWidth()/2, GetScreenHeight()/2};
	Vector2 cursor_vec = get_cursor_dir();
	float vec_len = min(length(cursor_vec), CORSHAIR_SCALE);
	
	cam.offset = sub(mid_point, scale(normalize(cursor_vec), 0.5f * vec_len));
}


Vector2 get_cursor_dir()
{
	Vector2 target_pos = GetMousePosition();
  float pos_x = static_cast<float>(GetScreenWidth() / 2);
  float pos_y = static_cast<float>(GetScreenHeight() / 2);
	Vector2 pos = {pos_x, pos_y};
	Vector2 target_dir = sub(target_pos, pos);
}


void render_corshair(Texture2D& tex)
{
	auto [x, y] = GetMousePosition();
	Vector2 pos = (Vector2){
		static_cast<int>(x - 16.0f),
		static_cast<int>(y - 16.0f)
	};
	DrawTextureV(
		tex,
		pos,
		WHITE
	);
}

