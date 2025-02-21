#include "../inc/vecUtils.hpp"


//NOTE: prolly move this somewhere else, or rename this module
void elastic_collision(
	float m1, Vector2& v1, Vector2 p1, float m2, Vector2& v2, Vector2 p2
) 
{
	//TODO: refactor to avoid this retarded shift to the center
	Vector2 center1 = add(p1, (Vector2){16.0f, 16.0f});
	Vector2 center2 = add(p2, (Vector2){16.0f, 16.0f});


	// Normal vector between centers
  Vector2 n = sub(center2, center1);
  Vector2 n_hat = normalize(n); 

  // Relative velocity
  Vector2 v_rel = sub(v1, v2);

  // Velocity along the normal
  float v_rel_n = dot(v_rel, n_hat);

  // Calculate impulse (elastic collision with e=1)
  float J = (-2 * v_rel_n) / (1/m1 + 1/m2);

  // Apply impulse with correct direction (add to v1, subtract from v2)
  v1 = add(v1, scale(n_hat, J / m1));
  v2 = sub(v2, scale(n_hat, J / m2));
}


Vector2 normalize(Vector2 v) 
{
	float length = sqrt(v.x * v.x + v.y * v.y);

	if (length == 0) return {0, 0}; 
	
	return (Vector2){v.x / length, v.y / length};
}


float dot(Vector2 a, Vector2 b) 
{
	return a.x * b.x + a.y * b.y;
}


Vector2 add(Vector2 a, Vector2 b)
{
	return (Vector2){a.x + b.x, a.y + b.y};
}


Vector2 sub(Vector2 a, Vector2 b) 
{
	return (Vector2){a.x - b.x, a.y - b.y};
}


Vector2 scale(Vector2 v, float scalar) 
{
	return (Vector2){v.x * scalar, v.y * scalar};
}
