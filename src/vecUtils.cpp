#include "../inc/vecUtils.hpp"


//NOTE: prolly move this somewhere else, or rename this module
void elastic_collision(
	float m1, Vector2& v1, Rectangle& r1, float m2, Vector2& v2, Rectangle& r2
) 
{
	Vector2 p1 = (Vector2){r1.x, r1.y};
	Vector2 p2 = (Vector2){r2.x, r2.y};
	Vector2 center1 = add(p1, (Vector2){r1.width, r1.height});
	Vector2 center2 = add(p2, (Vector2){r2.width, r2.height});


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
	
	Vector2 norm_v1 = normalize(v1);
	Vector2 norm_v2 = normalize(v1);

	p1 = add(p1, scale(norm_v1, length(n) / 2.0f));
	p2 = sub(p2, scale(norm_v2, length(n) / 2.0f));

	r1.x = p1.x;
	r1.y = p1.y;
	r2.x = p2.x;
	r2.y = p2.y;
}


Vector2 normalize(Vector2 v) 
{
	float len = length(v);

	if (len == 0) return {0, 0}; 
	
	return (Vector2){v.x / len, v.y / len};
}

float length(Vector2 v)
{
	return sqrt(v.x * v.x + v.y * v.y);
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
