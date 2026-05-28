#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using ll = long long;
using ull = unsigned long long;

#include "./hash_table.hpp"

void functional_test() {
    otukado::hash_table<string, int> hash_table;

    assert(hash_table.put("aaa", 1));
    assert(hash_table.get("aaa") == 1);
    assert(hash_table.size() == 1);

    assert(hash_table.put("bbb", 2));
    assert(hash_table.put("ccc", 3));
    assert(hash_table.put("ddd", 4));
    assert(hash_table.get("aaa") == 1);
    assert(hash_table.get("bbb") == 2);
    assert(hash_table.get("ccc") == 3);
    assert(hash_table.get("ddd") == 4);
    assert(!hash_table.get("a"));
    assert(!hash_table.get("aa"));
    assert(!hash_table.get("aaaa"));
    assert(hash_table.size() == 4);

    assert(!hash_table.put("aaa", 11));
    assert(hash_table.get("aaa") == 11);
    assert(hash_table.size() == 4);

    assert(hash_table.erase("aaa"));
    assert(!hash_table.get("aaa"));
    assert(hash_table.size() == 3);

    assert(!hash_table.erase("a"));
    assert(!hash_table.erase("aa"));
    assert(!hash_table.erase("aaa"));
    assert(!hash_table.erase("aaaa"));

    assert(hash_table.erase("ddd"));
    assert(hash_table.erase("ccc"));
    assert(hash_table.erase("bbb"));
    assert(!hash_table.get("aaa"));
    assert(!hash_table.get("bbb"));
    assert(!hash_table.get("ccc"));
    assert(!hash_table.get("ddd"));
    assert(hash_table.size() == 0);

    assert(hash_table.put("abc", 1));
    assert(hash_table.put("acb", 2));
    assert(hash_table.put("bac", 3));
    assert(hash_table.put("bca", 4));
    assert(hash_table.put("cab", 5));
    assert(hash_table.put("cba", 6));
    assert(hash_table.get("abc") == 1);
    assert(hash_table.get("acb") == 2);
    assert(hash_table.get("bac") == 3);
    assert(hash_table.get("bca") == 4);
    assert(hash_table.get("cab") == 5);
    assert(hash_table.get("cba") == 6);
    assert(hash_table.size() == 6);

    assert(hash_table.erase("abc"));
    assert(hash_table.erase("cba"));
    assert(hash_table.erase("bac"));
    assert(hash_table.erase("bca"));
    assert(hash_table.erase("acb"));
    assert(hash_table.erase("cab"));
    assert(hash_table.size() == 0);

    for(int i=0; i<100; ++i) {
        hash_table.put(to_string(i), i);
    }
    for(int i=0; i<100; ++i) {
        assert(hash_table.get(to_string(i)) == i);
    }
    for(int i=0; i<100; ++i) {
        assert(hash_table.erase(to_string(i)));
    }
    hash_table.put("abc", 1);
    hash_table.put("acb", 2);
    assert(hash_table.get("abc") == 1);
    assert(hash_table.get("acb") == 2);
    cout << "Functional tests passed!" << '\n';
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    functional_test();
    
}