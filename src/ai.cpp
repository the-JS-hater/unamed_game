#include "../inc/ai.hpp"


// std::pair hash function taken from:
// https://www.geeksforgeeks.org/how-to-create-an-unordered_map-of-pairs-in-c/
struct hash_pair {
  template <class T1, class T2>
  size_t operator()(const pair<T1, T2>& p) const
  {
    size_t hash1 = hash<T1>{}(p.first);
    size_t hash2 = hash<T2>{}(p.second);
    return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
  }
};


float manhattan_distance(Coord const& a, Coord const& b) 
{
  return abs(a.first - b.first) + abs(a.second - b.second);
}

vector<Coord> reconstruct_path(
  unordered_map<Coord, Coord, hash_pair> const& came_from,  
  Coord current
) 
{
  vector<Coord> path;
  path.push_back(current);
  while (came_from.find(current) != came_from.end()) 
  {
    current = came_from.at(current);
    path.push_back(current);
  }
  reverse(path.begin(), path.end());
  return path;
}


vector<Coord> get_neighbors(Coord const& pos, TileMap const& tileMap) 
{
  vector<Coord> neighbors;
  vector<Coord> const directions = {{0,1}, {0,-1}, {1,0}, {-1,0}};
  
  for (const auto& dir : directions) 
	{
    int x = pos.first + dir.first;
    int y = pos.second + dir.second;
    
    if (
			x >= 0 && x < tileMap.width && 
      y >= 0 && y < tileMap.height &&
      tileMap.map[y][x] != WALL
    ) 
    {
      neighbors.emplace_back(x, y);
    }
  }
  return neighbors;
}


vector<Coord> a_star(Coord start, Coord target, TileMap const& tileMap) 
{
  if (start == target) return {start};
  
  auto h = [&target](Coord const& pos) 
  {
    return manhattan_distance(pos, target);
  };

  using QueueElement = pair<float, Coord>;
  auto cmp = [](QueueElement const& a, QueueElement const& b) 
  {
    return a.first > b.first; 
  };

  priority_queue<QueueElement, vector<QueueElement>, decltype(cmp)> open_set(cmp);

  unordered_map<Coord, Coord, hash_pair> came_from;
  unordered_map<Coord, float, hash_pair> g_score;
  unordered_map<Coord, float, hash_pair> f_score;

  g_score[start] = 0;
  f_score[start] = h(start);
  open_set.emplace(f_score[start], start);

  while (!open_set.empty()) 
  {
    auto [current_f, current] = open_set.top();
    open_set.pop();

    if (current == target) 
    {
      return reconstruct_path(came_from, current);
    }

    if (f_score[current] < current_f) continue;

    for (auto const& neighbor : get_neighbors(current, tileMap)) 
    {
      float tentative_gScore = g_score[current] + 1.0f;
      float neighbor_g = INFINITY;
      
      if (auto it = g_score.find(neighbor); it != g_score.end()) 
      {
        neighbor_g = it->second;
      }

      if (tentative_gScore < neighbor_g) 
      {
        came_from[neighbor] = current;
        g_score[neighbor] = tentative_gScore;
        float new_f = tentative_gScore + h(neighbor);
        f_score[neighbor] = new_f;
        open_set.emplace(new_f, neighbor);
      }
    }
  }

  return {};
}


