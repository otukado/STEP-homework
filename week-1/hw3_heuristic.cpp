#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using ll = long long;
using ull = unsigned long long;

// --- 高速な乱数生成器 (XorShift) ---
uint32_t xor_shift() {
    static uint32_t y = 2463534242;
    y = y ^ (y << 13);
    y = y ^ (y >> 17);
    return y = y ^ (y << 5);
}
inline double rnd() {
    return (double)xor_shift() / 4294967295.0;
}

// --- 実行時間管理クラス ---
class Timer {
    chrono::high_resolution_clock::time_point start_time;
public:
    Timer() { start_time = chrono::high_resolution_clock::now(); }
    double elapsed_ms() {
        auto now = chrono::high_resolution_clock::now();
        return chrono::duration_cast<chrono::duration<double, milli>>(now - start_time).count();
    }
};

array<int, 26> SCOREBOARD = {1, 3, 2, 2, 1, 3, 3, 1, 1, 4, 4, 2, 2, 1, 1, 3, 4, 1, 1, 1, 2, 3, 3, 4, 3, 4};
int calculate_word (array<int, 26> counts) {
    int score = 0;
    for(int i=0; i<26; ++i) {
        score += counts[i] * SCOREBOARD[i];
    }
    return score;
}

struct word {
    string original;
    array<int, 26> count;
    uint32_t mask = 0;       // 高速枝刈り用のビットマスク
    int score = 0;           // 事前計算されたスコア
    double base_eval = 0.0;  // 事前計算されたコスパ (スコア / 文字数)
};

auto count_alphabet(string & str) {
    auto alp = array<int, 26>{0};
    for(auto & c : str) {
        alp[c - 'a'] ++;
    }
    return alp;
}

string encode_state(array<int, 26> & arr) {
    string s(26, '\0');
    for(int i = 0; i < 26; ++i) {
        s[i] = (char)arr[i];
    }
    return s;
}

auto calc_subset(array<int, 26> sub, array<int,26> main) {
    array<int, 26> res = main;
    for(int i=0; i<26; ++i) {
        res[i] -= sub[i];
    }
    return res;
}

struct Node {
    Node* parent = nullptr;
    std::array<Node*, 26> children{};

    char alphabet = '$';
    int number = -1;

    Node() = default;
    Node(Node* par, char alp) : parent(par), alphabet(alp) {}

    ~Node() {
        for(auto& child : this->children) {
            delete child;
        }
    }
};

struct Trie {
    Node* root = new Node();

    ~Trie() {
        delete this->root;
    }

    void insert (array<int, 26> counts, int number) {
        auto curr = this->root;
        for(int i=0; i<26; ++i) {
            for(int j=0; j<counts[i]; ++j) {
                if(curr->children[i] == nullptr) {
                    curr->children[i] = new Node(curr, 'a' + i);
                }
                curr = curr->children[i];
            }

        }
        curr->number = number; // 辞書内での index
    }

    auto search(array<int, 26> counts) {
        auto curr = this->root;
        for(int i=0; i<26; ++i) {
            for(int j=0; j<counts[i]; ++j) {
                curr = curr->children[i];
            }
        }
        return curr;
    }

    vector<Node*> search_subset(array<int, 26> counts) {
        vector<Node*> subset;
        auto dfs = [&](auto & self, Node* node) -> void {
            if(node->number != -1) subset.push_back(node);
            for(auto child : node->children) {
                if(child == nullptr) continue;
                if(counts[child->alphabet - 'a'] > 0) {
                    --counts[child->alphabet - 'a'];
                    self(self, child);
                    ++counts[child->alphabet - 'a'];
                }
            }
        };
        dfs(dfs, this->root);
        return subset;
    };
};

bool compare_word(word a, word b) {
    if((int) a.original.size() == (int) b.original.size()) {
        return a.original < b.original;
    }
    return a.original.size() < b.original.size();
}

bool empty(array<int, 26> arr) {
    for(int i=0; i<26; ++i) {
        if(arr[i] != 0) return false;
    }
    return true;
}

int main() {
    // 全体の制限時間を管理するタイマー（安全のため58秒に設定）
    Timer global_timer;
    const double TOTAL_TIME_LIMIT_MS = 180000.0;

    ifstream words("../scripts/anagram/words.txt");
    ofstream words_output("./output/words_output.txt");
    ifstream input("../scripts/anagram/large.txt");
    ofstream output("./output/large_ans_hw3.txt");
    string line;

    vector<word> dictionary;
    while(getline(words, line)) {
        dictionary.push_back({line, count_alphabet(line), 0});
    }
    sort(dictionary.begin(), dictionary.end(), compare_word);

    Trie trie;
    map<array<int, 26>, bool> memo;

    // 今までの単語で新しい単語を作ることができるか
    auto canMake = [&](auto & w) -> bool {
        auto dfs = [&](auto& self, array<int, 26> counts) -> bool {
            if(empty(counts)) return true;
            if(memo.count(counts)) return memo[counts];

            auto subset = trie.search_subset(counts);
            for(int id = 0; id < (int) subset.size(); ++id) {
                auto arr = calc_subset(dictionary[subset[id]->number].count, counts);
                if(self(self, arr)) {
                    return memo[counts] = true;
                };
            }
            return memo[counts] = false;
        };
        return dfs(dfs, w.count);
    };

    vector<word> min_dictionary;

    for(int i=0; i<(int) dictionary.size(); ++i) {
        if(!canMake(dictionary[i])) {
            trie.insert(dictionary[i].count, i);
            
            // 枝刈り用のビットマスクを構築
            uint32_t m = 0;
            for(int j = 0; j < 26; ++j) {
                if(dictionary[i].count[j] > 0) m |= (1 << j);
            }
            dictionary[i].mask = m;

            dictionary[i].score = calculate_word(dictionary[i].count);
            dictionary[i].base_eval = (double)dictionary[i].score / dictionary[i].original.size();

            min_dictionary.push_back(dictionary[i]);
            words_output << dictionary[i].original << '\n';
        }
        memo[dictionary[i].count] = true;
    }

    vector<string> queries;
    string q_line;
    while(getline(input, q_line)) {
        queries.push_back(q_line);
    }
    int Q = queries.size();

    for (int q_idx = 0; q_idx < Q; ++q_idx) {
        const string& query = queries[q_idx];

        // 制限時間計算
        double remaining_time = TOTAL_TIME_LIMIT_MS - global_timer.elapsed_ms();
        double time_limit_ms = max(0.1, remaining_time / (Q - q_idx));

        // クエリの文字カウントとビットマスクを生成
        array<int, 26> q_counts = {0};
        uint32_t q_mask = 0;
        for (char c : query) {
            int idx = c - 'a';
            q_counts[idx]++;
            q_mask |= (1 << idx);
        }

        // このクエリで作れるか
        vector<const word*> candidates;
        for (const auto& w : min_dictionary) {
            if ((w.mask & ~q_mask) != 0) continue;

            // 各アルファベットの個数がクエリを超えていないか
            bool can_fit = true;
            for (int i = 0; i < 26; ++i) {
                if (w.count[i] > q_counts[i]) {
                    can_fit = false;
                    break;
                }
            }
            if (can_fit) candidates.push_back(&w);
        }

        int best_score = -1;
        vector<string> best_combination;

        if (!candidates.empty()) {
            struct CandidateInfo {
                const word* w_ptr;
                int score;
                double base_eval;
            };
            vector<CandidateInfo> cand_infos;
            cand_infos.reserve(candidates.size());
            for (const word* w : candidates) {
                cand_infos.push_back({w, w->score, w->base_eval});
            }

            double start_time = global_timer.elapsed_ms();
            int iterations = 0;

            // 乱択貪欲回す
            while (true) {
                if ((++iterations & 31) == 0) {
                    if (global_timer.elapsed_ms() - start_time > time_limit_ms) {
                        break;
                    }
                }

                array<int, 26> current_counts = q_counts;
                vector<string> current_combination;
                int current_score = 0;

                // 評価値にランダムなノイズ（0.8 〜 1.2倍）を付与
                vector<pair<double, int>> eval_list; // {スコア、インデックス}
                eval_list.reserve(cand_infos.size());
                for (int i = 0; i < (int)cand_infos.size(); ++i) {
                    double noisy_eval = cand_infos[i].base_eval * (0.8 + 0.4 * rnd());
                    eval_list.push_back({noisy_eval, i});
                }
                sort(eval_list.rbegin(), eval_list.rend());

                // 貪欲
                for (const auto& item : eval_list) {
                    const auto& info = cand_infos[item.second];
                    const word* w = info.w_ptr;

                    while (true) {
                        bool can_add = true;
                        for (int i = 0; i < 26; ++i) {
                            if (current_counts[i] < w->count[i]) {
                                can_add = false;
                                break;
                            }
                        }
                        if (!can_add) break;
                        // 確定後
                        for (int i = 0; i < 26; ++i) {
                            current_counts[i] -= w->count[i];
                        }
                        current_combination.push_back(w->original);
                        current_score += info.score;
                    }
                }

                // 最高スコアの更新
                if (current_score > best_score) {
                    best_score = current_score;
                    best_combination = current_combination;
                }
            }
        }

        for (int i = 0; i < (int) best_combination.size(); ++i) {
            output << best_combination[i] << (i == best_combination.size() - 1 ? "" : " ");
        }
        output << '\n';
    }
}