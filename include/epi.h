#ifndef __EPI_H__
#define __EPI_H__

#include "common.h"
using namespace std;

void removeDuplicates(char str[], int length)
{
  if (length < 2) {
    return;
  }

  // Table of found characters, 256 is the number of possible
  // character codes in an 8-bit ANSI encoding system
  // char in C# is not ANSI, it is Unicode ans is 16-bit
  // that will require a much larger array with size of 63KB
  // bool found[65535] = { false };
  bool found[256] = { false };

  // Find duplicates and mark them in the table
  for (int i = 0; i < length; ++i) {
    size_t c = str[i];
    if (!found[c]) {
      found[c] = true;
    } else {
      str[i] = '\0';
    }
  }

  for (int i = 0; i < length; ++i) {
    if (str[i] == '\0') {
      int j = i + 1;
      while (j < length && str[i] == '\0') {
        ++j;
      }
    }
  }
}

void merge(vector<int>& arr, int l, int m, int h)
{
  int n1 = m - l + 1;
  int n2 = h - m;
  vector<int> left(n1 + 1, INT_MAX);
  vector<int> right(n2 + 1, INT_MAX);
  for (int i = 0; i < n1; ++i) {
    left[i] = arr[l + i];
  }
  for (int i = 0; i < n2; ++i) {
    right[i] = arr[m + i + 1];
  }

  int i = 0, j = 0;
  for (int k = l; k <= h; ++k) {
    if (left[i] <= right[j]) {
      arr[k] = left[i];
      ++i;
    } else {
      arr[k] = right[j];
      ++j;
    }
  }
}

void merge_sort(vector<int>& arr, int l, int h)
{
  if (l < h) {
    int m = (l + h) / 2;
    merge_sort(arr, l, m);
    merge_sort(arr, m + 1, h);
    merge(arr, l, m, h);
  }
}

int c4_1_right_prop(int x)
{
  return x | ((x & ~(x - 1)) - 1);
}

bool c4_1_is_pow2(int x)
{
  return (x > 0) && ((x & (x - 1)) == 0);
}

int c4_1_mod_pow2(int x, int m)
{
  return x & (m - 1);
}

void c4_1_mod_pow2_test()
{
  int num = 600;
  int mod = 64;
  cout << bitset<16>(num) << " mod " << bitset<16>(mod) <<
    " = " << bitset<16>(num % mod) << "|" << bitset<16>(c4_1_mod_pow2(num, mod)) << endl;
}

enum Color {
  RED, WHITE, BLUE
};

template<class T>
void print_list(T& arr)
{
  for (auto c : arr) {
    cout << c << ",";
  }

  cout << endl;
}

template<class T>
void print_matrix(vector<deque<T>> matrix)
{
  for (auto& v : matrix) {
    for (auto& n : v) {
      cout << n << ' ';
    }
    cout << endl;
  }
}

void c5_1_dutch_flag(int pv_index, vector<Color>& arr)
{
  int smaller = 0, equal = 0, larger = arr.size();
  Color pv = arr[pv_index];

  //
  // [0, smaller - 1]
  // [smaller, equal - 1]
  // [equal, larger - 1]
  // [larger, arr.size() - 1]
  //

  while (equal < larger) {
    if (arr[equal] < pv) {
      swap(arr[equal++], arr[smaller++]);
    } else if (arr[equal] == pv) {
      ++equal;
    } else {
      swap(arr[equal], arr[--larger]);
    }
  }
}

void c5_1_dutch_flag_test()
{
  vector<Color> arr1 = { BLUE, RED, WHITE, WHITE, RED, BLUE, BLUE, RED, WHITE, WHITE, RED };
  vector<Color> arr2 = arr1;
  c5_1_dutch_flag(3, arr2);

  print_list(arr1);
  print_list(arr2);
}

int c5_6_longest_equal_subarray(vector<int> arr)
{
  int ans = 0;
  int begin = -1, end = 0;
  int n = INT_MIN;

  while (end < (int)arr.size()) {
    if (arr[end] != n) {
      ans = max(ans, end - begin);
      n = arr[end];
      begin = end;
    }
    end++;
  }

  return ans;
}

struct TreeNode {
  int val;
  TreeNode* left;
  TreeNode* right;
  TreeNode* parent;
};

void c9_1_size_larget_subree_test()
{

}

void c18_2_paint_a_boolean_matrix(int row, int col, vector<deque<int>>& matrix)
{
  int target_color = 1 - matrix[row][col];
  stack<pair<int, int>> s;
  pair<int, int> offset[] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

  s.push({ row, col });

  while (!s.empty()) {
    auto curr = s.top();
    s.pop();
    bool is_feasible = (curr.first >= 0 && curr.first < (int)matrix.size() &&
                        curr.second >= 0 && curr.second < (int)matrix[0].size());
    if (!is_feasible ||
        matrix[curr.first][curr.second] == target_color)
      continue;

    matrix[curr.first][curr.second] = target_color;

    for (auto& dir : offset) {
      s.push({ curr.first + dir.first, curr.second + dir.second });
    }
  }
}

void c18_2_paint_a_boolean_matrix_test()
{
  vector<deque<int>> matrix{
    { 1, 0, 0, 1 },
    { 0, 0, 1, 0 },
    { 1, 1, 1, 0 },
    { 1, 0, 0, 1 }
  };

  cout << "Input:" << endl;
  print_matrix(matrix);
  c18_2_paint_a_boolean_matrix(0, 1, matrix);

  cout << "Output:" << endl;
  print_matrix(matrix);
}

void c5_6_longest_equal_subarray_test()
{
  cout << "i=" << c5_6_longest_equal_subarray({}) << endl;
  cout << "i=" << c5_6_longest_equal_subarray({ 1 }) << endl;
  cout << "i=" << c5_6_longest_equal_subarray({ 0, 1, 2, 3, 4 }) << endl;
  cout << "i=" << c5_6_longest_equal_subarray({ 0, 0, 0, 0, 2, 3, 4 }) << endl;
  cout << "i=" << c5_6_longest_equal_subarray({ 0, 1, 1, 2, 3, 4, 4, 5, 4, 4, 4, 2 ,3 }) << endl;
  cout << "i=" << c5_6_longest_equal_subarray({ 0, 1, 1, 2, 3, 4, 4, 5, 5, 5, 5, 2 ,3 }) << endl;
}

void reverseWords(string &s) {
  if (s.empty()) return;

  int lo = 0, hi = s.size() - 1;
  while (lo < hi) swap(s[lo++], s[hi--]);

  size_t i = 0;
  while (i < s.size()) {
    while (i < s.size() && s[i] == ' ') ++i;
    lo = i;
    while (i < s.size() && s[i] != ' ') ++i;
    hi = i - 1;
    while (lo < hi) swap(s[lo++], s[hi--]);
  }

  size_t k = 0;
  size_t last_word_end = INT_MIN;
  for (i = 0; i < s.size(); ++i) {
    if (s[i] != ' ') {
      s[k++] = s[i];
      last_word_end = i;
    } else if (last_word_end == i - 1) {
      s[k++] = ' ';
    }
  }

  if (s[k - 1] == ' ') {
    k--;
  }

  s.resize(k);
}

void reverse_string_test()
{
  string str = " ab cd  ef   gh    ";
  reverseWords(str);
  cout << "[" << str << "]" << endl;
}

struct UndirectedGraphNode {
  int label;
  vector<UndirectedGraphNode *> neighbors;
  UndirectedGraphNode(int x) : label(x) {};
};

UndirectedGraphNode* c18_5_clone_graph(UndirectedGraphNode *node) {
  if (node == nullptr) return nullptr;
  unordered_map<UndirectedGraphNode*, UndirectedGraphNode*> m;
  stack<UndirectedGraphNode*> s;
  s.push(node);

  while (!s.empty()) {
    auto curr = s.top();
    s.pop();
    if (m.count(curr) == 0) {
      m[curr] = new UndirectedGraphNode(curr->label);
      for (auto adj : curr->neighbors) {
        s.push(adj);
      }
    }
  }

  for (auto& n : m) {
    auto cloned_node = n.second;
    for (auto adj : n.first->neighbors) {
      cloned_node->neighbors.push_back(m[adj]);
    }
  }

  return m[node];
}

void c18_5_clone_graph_test()
{
  c18_5_clone_graph(nullptr);
}

int real_cmp(double lhs, double rhs) {
  double relative_error = (lhs - rhs) / rhs;
  if (relative_error < -numeric_limits<double>::epsilon()) return -1;
  else if (relative_error > numeric_limits<double>::epsilon()) return 1;
  else return 0;
}

double c11_5_real_sqrt(double n)
{
  double lo, hi;
  if (n < 1.0) {
    lo = n;
    hi = 1.0;
  } else {
    lo = 1.0;
    hi = n;
  }

  while (real_cmp(lo, hi) != 0) {
    double mid = lo + 0.5 * (hi - lo);
    double mid_sqr = mid * mid;
    if (real_cmp(mid_sqr, n) == 1) { // greater
      hi = mid;
    } else { // less than or equal
      lo = mid;
    }
  }

  return lo;
}

void c11_5_real_sqrt_test()
{
  for (double i = 0.0; i <= 100.0; i += 0.001) {
    double expected = sqrtf(i);
    double actual = c11_5_real_sqrt(i);

    if (real_cmp(expected, actual) == 0) {
      printf("FAILED: sqrtf(%f) expected:%f actual:%f\n", i, expected, actual);
    }
  }
}

vector<int> c9_11_inorder_traversal(TreeNode* root) {
  TreeNode* prev = nullptr;
  TreeNode* curr = root;
  vector<int> res;

  while (curr != nullptr) {
    TreeNode* next;
    if (prev == curr->parent) {
      // Came down from a parent to either left or right subtree
      if (curr->left != nullptr) {
        next = curr->left;
      } else {
        res.push_back(curr->val);
        if (curr->right != nullptr) {
          next = curr->right;
        } else {
          next = curr->parent;
        }
      }
    } else if (prev == curr->left) {
      // Return back to a parent from left subtree
      res.push_back(curr->val);
      if (curr->right != nullptr) {
        next = curr->right;
      } else {
        next = curr->parent;
      }
    } else {
      // Return back to a parent from right subtree
      next = curr->parent;
    }
    prev = curr;
    curr = next;
  }

  return res;
}

struct DijkstraData {
  int dist;
  UndirectedGraphNode* prev;
};

typedef int NodeId;

struct UndirectedGraph {
  UndirectedGraph(int size)
    : nodes(size, vector<pair<NodeId, int>>())
  {
  }

  // first: v node label
  // second: weight from u to v
  vector<vector<pair<NodeId, int>>> nodes;

  vector<pair<NodeId, int>>& adj(NodeId u) {
    return nodes[u];
  }

  void add_edge(int u, int v, int w) {
    nodes[u].push_back({ v, w });
    nodes[v].push_back({ u, w });
  }

  void add_edge2(char u, char v, int w) {
    nodes[u - 'A'].push_back({ v - 'A', w });
    nodes[v - 'A'].push_back({ u - 'A', w });
  }
};

vector<NodeId> c18_9_dijkstra_priority_queue(UndirectedGraph* graph, NodeId src, NodeId dst) {
  vector<NodeId> prev(graph->nodes.size(), -1);
  vector<int> dist(graph->nodes.size(), INT_MAX);
  priority_queue<pair<int, NodeId>, vector<pair<int, NodeId>>, greater<pair<int, NodeId>>> min_q;
  dist[src] = 0;
  min_q.push({ 0, src });

  while (!min_q.empty()) {
    auto curr = min_q.top();
    int u = curr.second;
    min_q.pop();
    for (auto neighbor : graph->adj(u)) {
      NodeId v = neighbor.first;
      int w = neighbor.second;
      if (dist[u] + w < dist[v]) {
        dist[v] = dist[u] + w;
        prev[v] = u;
        min_q.push({ dist[v], v });
      }
    }
  }

  return prev;
}

vector<NodeId> c18_9_dijkstra_set(UndirectedGraph* graph, NodeId src, NodeId dst) {
  vector<NodeId> prev(graph->nodes.size(), -1);
  vector<int> dist(graph->nodes.size(), INT_MAX);
  set<pair<int, NodeId>> min_q;
  dist[src] = 0;
  min_q.insert({ 0, src });

  while (!min_q.empty()) {
    auto curr = *min_q.begin();
    int u = curr.second;
    min_q.erase(min_q.begin());
    for (auto neighbor : graph->adj(u)) {
      NodeId v = neighbor.first;
      int w = neighbor.second;
      if (dist[u] + w < dist[v]) {
        if (dist[v] != INT_MAX) {
          min_q.erase({ dist[v], v });
        }

        dist[v] = dist[u] + w;
        prev[v] = u;
        min_q.insert({ dist[v], v });
      }
    }
  }

  return prev;
}

void c18_9_dijkstra_test() {
  UndirectedGraph g(7);
  g.add_edge2('A', 'B', 4);
  g.add_edge2('A', 'C', 3);
  g.add_edge2('A', 'E', 7);
  g.add_edge2('B', 'D', 5);
  g.add_edge2('B', 'C', 6);
  g.add_edge2('C', 'D', 11);
  g.add_edge2('C', 'E', 8);
  g.add_edge2('D', 'F', 2);
  g.add_edge2('D', 'G', 10);
  g.add_edge2('D', 'E', 2);
  g.add_edge2('E', 'G', 5);
  g.add_edge2('F', 'G', 3);
  auto prev = c18_9_dijkstra_priority_queue(&g, 'A' - 'A', 'F' - 'A');
  NodeId curr = 'F' - 'A';
  while (curr != -1) {
    cout << (char)(curr + 'A') << "->";
    curr = prev[curr];
  }
  cout << endl;
}

enum NodeColor {
  White,
  Gray,
  Black
};
struct DirectedGraph {
  DirectedGraph(int size)
    : nodes(size, vector<pair<NodeId, int>>())
  {
  }

  // first: v node label
  // second: weight from u to v
  vector<vector<pair<NodeId, int>>> nodes;

  vector<pair<NodeId, int>>& adj(NodeId u) {
    return nodes[u];
  }

  void add_edge(int u, int v) {
    add_edge(u, v, 1);
  }

  void add_edge(int u, int v, int w) {
    nodes[u].push_back({ v, w });
  }

  void add_edge2(char u, char v, int w) {
    nodes[u - 'A'].push_back({ v - 'A', w });
  }
};

bool has_cycle_dfs(DirectedGraph* graph, NodeId u, vector<NodeColor>& color) {
  if (color[u] == Gray) return true;

  color[u] = Gray;
  for (auto curr : graph->adj(u)) {
    int v = curr.first;
    if (color[v] != Black && has_cycle_dfs(graph, v, color))
      return true;
  }
  color[u] = Black;
  return false;
}

bool c18_4_detect_graph_cycle(DirectedGraph* graph)
{
  vector<NodeColor> color(graph->nodes.size(), White);
  for (size_t i = 0; i < graph->nodes.size(); ++i) {
    if (color[i] == White && has_cycle_dfs(graph, i, color))
      return true;
  }

  return false;
}

void c18_4_detect_graph_cycle_test()
{
  DirectedGraph g(7);
  g.add_edge(1, 2);
  g.add_edge(2, 3);
  g.add_edge(1, 3);
  g.add_edge(4, 1);
  g.add_edge(4, 5);
  g.add_edge(5, 6);
  g.add_edge(6, 4);

  if (c18_4_detect_graph_cycle(&g)) {
    cout << "has cycle" << endl;
  } else {
    cout << "no cycle detected" << endl;
  }
}

void topo_dfs(DirectedGraph* g, NodeId u, deque<bool>& visited, stack<NodeId>& ordering) {
  visited[u] = true;
  for (auto curr : g->adj(u)) {
    NodeId v = curr.first;
    if (visited[v]) continue;
    topo_dfs(g, v, visited, ordering);
  }
  ordering.push(u);
}

stack<NodeId> topo_ordering(DirectedGraph* g) {
  deque<bool> visited(g->nodes.size(), false);
  stack<NodeId> ordering;
  for (size_t i = 0; i < g->nodes.size(); ++i) {
    if (!visited[i]) {
      topo_dfs(g, i, visited, ordering);
    }
  }
  return ordering;
}

stack<int> c18_8_photo_day(DirectedGraph* g) {
  return topo_ordering(g);
}

void c18_8_photo_day_test() {
  DirectedGraph g(8);
  g.add_edge(1, 3);
  g.add_edge(2, 3);
  g.add_edge(2, 5);
  g.add_edge(3, 4);
  g.add_edge(4, 6);
  g.add_edge(5, 6);
  g.add_edge(6, 7);

  auto s = c18_8_photo_day(&g);
  while (!s.empty()) {
    cout << s.top() << "->";
    s.pop();
  }
  cout << endl;
}

// 0x12
// 0001 0010

bool get_bit(size_t bi, const vector<uint8_t>& stream) {
  size_t by = bi / 8;
  size_t shft = bi % 8;
  return stream[by] & (1 << (7 - shft));
}

uint32_t count_occ(const std::vector<uint8_t>& stream, const std::deque<bool> pattern) {
  uint32_t occ = 0;
  size_t bit_count = stream.size() * 8;
  for (size_t i = 0; i < bit_count;) {
    for (size_t k = i, j = 0; k < bit_count && j < pattern.size();) {
      if (!(get_bit(k, stream) ^ pattern[j])) {
        k++;
        j++;
      } else {
        i++;
        break;
      }

      if (j == pattern.size()) {
        occ++;
        i += pattern.size();
      }
    }
  }

  return occ;
}
#endif // __EPI_H__
