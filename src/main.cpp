
#include "concurrency_tests.h"
#include "memory_mgmt.h"
#include "epi.h"

using namespace std;

void rev(string& s, int i, int j) {
  for (; i < j; ++i, --j) swap(s[i], s[j]);
}

//void reverseWords(string &s) {
//  rev(s, 0, s.size() - 1);
//  int begin = 0;
//  int end = 0;
//  while (end < s.size()) {
//    if (s[end] == ' ') {
//      rev(s, begin, end - 1);
//      begin = end + 1;
//    }
//    end++;
//  }
//
//  rev(s, begin, end - 1);
//}

struct TrieNode;

struct TrieNode {
  unordered_map<char, shared_ptr<TrieNode>> Children;
  bool IsEow;
  int Val;
  TrieNode() { Val = 0; IsEow = false; }
};

class MapSum {
public:

  /** Initialize your data structure here. */
  MapSum() : root(new TrieNode) {

  }

  void insert(string key, int val) {
    shared_ptr<TrieNode> curr = root;
    for (size_t i = 0; i < key.length(); ++i) {
      auto res = curr->Children.emplace(key[i], make_shared<TrieNode>());
      curr = res.first->second;
    }

    curr->IsEow = true;
    curr->Val = val;
  }

  int getsum(const TrieNode* root) {
    int sum = 0;
    if (root->IsEow) sum += root->Val;
    for (auto c : root->Children) {
      sum += getsum(c.second.get());
    }
    return sum;
  }

  int sum(string prefix) {
    shared_ptr<TrieNode> curr = root;
    for (size_t i = 0; i < prefix.length(); ++i) {
      auto itr = curr->Children.find(prefix[i]);
      if (itr == curr->Children.end()) return false;
      curr = itr->second;
    }
    return getsum(curr.get());
  }

  shared_ptr<TrieNode> root;
};

int main()
{
  vector<uint8_t> b = { 0x12, 0x34, 0x56, 0x78 };

  for (size_t i = 0; i < b.size(); ++i) {
    cout << bitset<8>(b[i]) << " ";
  }
  cout << endl;
  for (size_t i = 0; i < b.size() * 8; ++i) {
    if (i > 0 && i % 8 == 0) cout << " ";
    if (get_bit(i, b)) cout << "1";
    else cout << "0";
  }
  cout << endl;

  auto p = { true, false, false };
  auto occ = count_occ(b, p);
  cout << "occ=" << occ << endl;

  /*MapSum m;

  m.insert("bazza", 2);
  m.insert("bazeza", 2);
  m.insert("dazeza", 2);
  m.insert("apple", 3);
  cout << m.sum("apple") << endl;
  m.insert("app", 2);
  cout << m.sum("ap") << endl;
  m.insert("azza", 2);
  cout << m.sum("a") << endl;
  cout << m.sum("az") << endl;
  m.insert("azeza", 2);
  cout << m.sum("a") << endl;
  cout << m.sum("az") << endl;
  cout << m.sum("az") << endl;

  cout << m.sum("ba") << endl;
  cout << m.sum("bazz") << endl;*/

  return 0;
}
