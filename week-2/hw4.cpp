#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using ll = long long;
using ull = unsigned long long;

#include "hash_table.hpp"
#include "random.hpp"

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

    V back () {
        return this->RIGHT->left->value;
    }

    V front() {
        return this->LEFT->right->value;
    }

    void push_back(V value) {
        element* elem = new element();

        auto back = this->rbegin().item;
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
    iterator begin() {
        return iterator {this->LEFT->right};
    }

    iterator end() {
        return iterator {this->RIGHT};
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
        if(old_it) {
            ++this->hit_count;
            items.erase(old_it.value());
        }
        else {
            ++this->miss_count;
            if (pointers.size() >= LIMIT) {
                pointers.erase(items.front().key);
                items.pop_front();  
            }
        }
        items.push_back({url, content});
        pointers.put(url, items.rbegin().item);
    };

    auto get_pages(){
        std::vector<K> pages;
        for(auto it = this->items.rbegin(); it != this->items.rend(); --it) {
            pages.push_back((*it).key);
        }
        return pages;
    };

    float get_hitrate() {
        float total = this->hit_count + this->miss_count;
        return max<float>(0, (float)this->hit_count / total);
    }
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

void performance_test() {
    //  Set the size of the cache to 100.
    otukado::cache<std::string, std::string, 100> cache;

    //  Generate queries based on the Zipf law.
    const float ALPHA = 1.5;
    const int NUM_QUERIES = 1000000;
    const int NUM_PAGES = 1000;
    
    vector<int> ranks;
    vector<float> weights;
    for(int rank=1; rank<NUM_PAGES+1; ++rank) {
        ranks.push_back(rank);
        weights.push_back(1.0 / pow(rank, ALPHA));
    }

    discrete_distribution<> dist(weights.begin(), weights.end());
    
    otukado::xor_shift random;

    for (int i = 0; i < NUM_QUERIES; ++i) {
        auto query = ranks[dist(random)];
        cache.access_page(std::to_string(query), "");
    }

    //  If your cache implementation is correct, the hit rate will be 91%.
    cout << "Cache hit rate = " << cache.get_hitrate() * 100 << '\n';
    cout << "Performance tests passed!" << '\n';
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cache_test();
    performance_test();
}