#include "../inc/vecUtils.hpp"


//NOTE: prolly move this somewhere else, or rename this module
void elastic_collision(
	float m1, Vector2& v1, Vector2 p1, float m2, Vector2& v2, Vector2 p2
) 
{
	// normal vector
  Vector2 n = sub(p2, p1);
  Vector2 n_hat = normalize(n); 

  // relative velocity
  Vector2 v_rel = sub(v1, v2);

  // velocity along the normal
  float v_rel_n = dot(v_rel, n_hat);

  // calculate impulse (for perfectly elastic collision, e = 1)
  float J = -2 * v_rel_n / (1/m1 + 1/m2);

  // update velocities
  v1 = sub(v1, scale(n_hat, J / m1));
  v2 = add(v2, scale(n_hat, J / m2));
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
