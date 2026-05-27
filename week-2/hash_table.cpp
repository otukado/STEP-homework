#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using ll = long long;
using ull = unsigned long long;

ull calculate_hash(string);


// T と BUCKET_SIZE を指定する
template<typename K, typename V>
struct HashTable {
    // size によって動的に HashTable 内でサイズを変更したい
    vector<list<pair<K, V>>> table = vector<list<pair<K, V>>>(3);
    int _size;
    inline static vector<ull> primes = {2}; // 全ての HashTable で共有したい
    inline static vector<bool> is_prime;


    bool put(K key, V value) {
        for(auto & element : this->get_bucket(key)) {
            if(element.first == key) {
                element.second = value;
                return false;
            }
        }
        this->get_bucket(key).emplace_back(key, value);
        ++this->_size;
        resize();
        return true;
    };

    // key がなければ nullopt を返す(error 投げない)
    optional<V> get(K key) {
        for(const auto & element : this->get_bucket(key)) {
            if(element.first == key) return element.second;
        }
        return nullopt;
    };

    bool erase(K key) {
        auto & _table = get_bucket(key);
        // iterator で回す
        for(auto it = _table.begin(); it != _table.end(); ++it) {
            if(key == it->first) {
                _table.erase(it); // key と一致したら削除
                --this->_size;
                resize();
                return true;
            }
        }
        return false;
    };

    int size() {
        return this->_size;
    }
    int bucket_size() {
        return this->table.size();
    }


    void resize() {
        if(!(3 * this->size() > 2 * this->bucket_size() || this->size() * 5 < this->bucket_size())) return; // あとでここの条件いじる
        // primes から bucket_size を決定する
        if((int) primes.back() < (int) this->size() * 5)expand_prime((int) this->size() * 10); // あとで考える
         
        const int x = *(lower_bound(this->primes.begin(), this->primes.end(), this->size() * 3));
        auto copied = this->table;
        this->table.clear();
        this->table.resize(x);
        cout << x << '\n';
        // copied から resize した table に詰め直す
        for(const auto & bucket : copied) {
            for(const auto & element : bucket) {
                get_bucket(element.first).push_back(element);
            }
        }
    };

    // 素数表を大きくする
    void expand_prime(int next_size) {
        int sz = is_prime.size();
        is_prime.resize(next_size, false);
        for(const auto & prime : primes) {
            for(int i = sz / prime; i*prime < next_size; ++i) {
                is_prime[i * prime] = true;
            }
        }
        for(int i=sz; i<next_size; ++i) {
            if(!is_prime[i] && i > 2) primes.push_back(i);
        }
    }

    ull calculate_hash(K key) {
        ull hash = ::calculate_hash(key) % this->bucket_size();
        return hash;
    }
    
    // key に対応する bucket を返す関数
    auto & get_bucket(K key) {
        // hash を計算し、mod を取る
        ull hash = calculate_hash(key);
        return this->table[hash];
    }
};

// hash 計算の本体。外にあるべき。string 用
ull calculate_hash(string key) {
    ull hash = 0;
    for(const auto & c : key) {
        hash += c - 'a'; // あとで直す
    }
    return hash;
};

void functional_test() {
    HashTable<string, int> hash_table;

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