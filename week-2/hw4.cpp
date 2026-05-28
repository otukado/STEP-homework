#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using ll = long long;
using ull = unsigned long long;

#include "hash_table.hpp"

namespace otukado {


template <typename V>
struct list {
    struct element {
        element* left = nullptr;
        element* right = nullptr;

        V value;
    };

    struct iterator {
        element* item;

        auto operator* () {
            return this->item->value;
        };
        void operator++ () {
            item = this->item->right;
        };
        void operator-- () {
            item = this->item->left;
        };
        bool operator== (iterator other) {
            return this->item == other.item;
        };
    };

    element* LEFT = new element();
    element* RIGHT = new element();

    list() {
        this->LEFT->right = this->RIGHT;
        this->RIGHT->left = this->LEFT;
    }

    element* back () {
        return this->RIGHT->left;
    }

    void push_back(V value) {
        element* elem = new element();

        auto back = this->back();
        this->RIGHT->left = elem; elem->right = this->RIGHT;
        back->right = elem; elem->left = back;
        elem->value = value;
    }

    void erase(element* it) {
        if(it != LEFT && it != RIGHT) {
            auto L = it->left;
            auto R = it->right;

            L->right = R; R->left = L;
            delete it;
        }
    }

    void pop_front() {
        auto it = LEFT->right;
        auto R = it->right;
        if(it == RIGHT) return;
        LEFT->right = R; R->left = LEFT;
        delete it;
    }

    iterator rbegin() {
        return iterator {RIGHT->left};
    }
    iterator rend() {
        return iterator {LEFT};
    }
    
}; 

template <typename K, typename V, int LIMIT>
struct cache {
    struct item {
        K key;
        V value;
    };

    list<item> items;
    hash_table<K, typename otukado::list<item>::element*> pointers;
    int hit_count = 0;
    int miss_count = 0;

    void access_page(K url, V content){
        auto old_it = pointers.get(url);
        if(old_it) items.erase(old_it.value());
        else if (pointers.size() >= LIMIT) items.pop_front();
        items.push_back({url, content});
        auto new_it = items.back();

        pointers.put(url, new_it);
    };

    auto get_pages(){
        std::vector<K> pages;
        for(auto it = this->items.rbegin(); it != this->items.rend(); --it) {
            pages.push_back((*it).key);
        }
        return pages;
    };
};


} // namespace otukado

void cache_test() {
    // Set the size of the cache to 4.
    otukado::cache<std::string, std::string, 4> cache;

    // Initially, no page is cached.
    assert(cache.get_pages().size() == 0);

    // Access "a.com".
    cache.access_page("a.com", "AAA");
    // "a.com" is cached.
    assert(cache.get_pages() == vector<string> {"a.com"});

    // Access "b.com".
    cache.access_page("b.com", "BBB");
    // The cache is updated to:
    //   (new)<-- "b.com", "a.com" -->(old)
    assert((cache.get_pages() == vector<string> {"b.com", "a.com"}));
    
    // Access "c.com".
    cache.access_page("c.com", "CCC");
    // The cache is updated to:
    //   (new)<-- "c.com", "b.com", "a.com" -->(old)
    assert((cache.get_pages() == vector<string> {"c.com", "b.com", "a.com"}));

    // Access "d.com".
    cache.access_page("d.com", "DDD");
    // The cache is updated to:
    //   (new)<-- "d.com", "c.com", "b.com", "a.com" -->(old)
    assert((cache.get_pages() == vector<string> {"d.com", "c.com", "b.com", "a.com"}));

    // Access "d.com" again.
    cache.access_page("d.com", "DDD");
    // The cache is updated to:
    //   (new)<-- "d.com", "c.com", "b.com", "a.com" -->(old)
    assert((cache.get_pages() == vector<string> {"d.com", "c.com", "b.com", "a.com"}));

    // Access "a.com" again.
    cache.access_page("a.com", "AAA");
    // The cache is updated to:
    //   (new)<-- "a.com", "d.com", "c.com", "b.com" -->(old)
    assert((cache.get_pages() == vector<string>{"a.com", "d.com", "c.com", "b.com"}));

    cache.access_page("c.com", "CCC");
    assert((cache.get_pages() == vector<string> {"c.com", "a.com", "d.com", "b.com"}));
    cache.access_page("a.com", "AAA");
    assert((cache.get_pages() == vector<string> {"a.com", "c.com", "d.com", "b.com"}));
    cache.access_page("a.com", "AAA");
    assert((cache.get_pages() == vector<string> {"a.com", "c.com", "d.com", "b.com"}));

    // Access "e.com".
    cache.access_page("e.com", "EEE");
    // The cache is full, so we remove the least recently accessed page "b.com".
    // The cache is updated to:
    //   (new)<-- "e.com", "a.com", "c.com", "d.com" -->(old)
    assert((cache.get_pages() == vector<string> {"e.com", "a.com", "c.com", "d.com"}));

    // Access "f.com".
    cache.access_page("f.com", "FFF");
    // The cache is full, so we remove the least recently accessed page "c.com".
    // The cache is updated to:
    //   (new)<-- "f.com", "e.com", "a.com", "c.com" -->(old)
    assert((cache.get_pages() == vector<string> {"f.com", "e.com", "a.com", "c.com"}));

    // Access "e.com".
    cache.access_page("e.com", "EEE");
    // The cache is updated to:
    //   (new)<-- "e.com", "f.com", "a.com", "c.com" -->(old)
    assert((cache.get_pages() == vector<string> {"e.com", "f.com", "a.com", "c.com"}));

    // Access "a.com".
    cache.access_page("a.com", "AAA");
    // The cache is updated to:
    //   (new)<-- "a.com", "e.com", "f.com", "c.com" -->(old)
    assert((cache.get_pages() == vector<string> {"a.com", "e.com", "f.com", "c.com"}));

    cout << "Tests passed!" << '\n';
};

void performance_test(){};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    std::list<int> list;

    otukado::hash_table<string, std::list<int>::iterator> hash_list;
    cache_test();

}