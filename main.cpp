#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <Windows.h>
#include <thread>
#include <iostream>
#include <list>
#include <unordered_map>
#include <bitset>
#include <deque>
#include <stack>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <set>
#include <random>
#include <array>
#include <atomic>
#include <map>
using namespace std;

#include "epi.h"
#include "parallel_computing.h"
#include "memory_mgmt.h"

UINT64 UnsafeMulDiv64x32x64(UINT64 mul64, UINT32 mul32, UINT64 div64)
{
  UINT64 q = mul64 / div64;
  UINT64 r = mul64 % div64;
  return q * mul32 + (r * mul32) / div64;
}

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
    for (int i = 0; i < key.length(); ++i) {
      auto res = curr->Children.emplace(key[i], new TrieNode);
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
    for (int i = 0; i < prefix.length(); ++i) {
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
  uint64_t m1[256];
  unordered_map<char, uint64_t> m2;
  size_t s = 0;
  for (int c = 'a'; c <= 'z'; ++c) {
    m2.insert({ c, 0xdeadbeef });
  }
  for (size_t i = 0; i < m2.bucket_count(); ++i) {
    s += m2.bucket_size(i);
  }
  cout << sizeof(m1) << endl << sizeof(m2) + s;

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