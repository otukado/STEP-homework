#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using ll = long long;
using ull = unsigned long long;

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
    ifstream words("../scripts/anagram/words.txt");
    ofstream words_output("./output/words_output.txt");
    ifstream input("../scripts/anagram/small.txt");
    ofstream output("./output/small_answer_hw3.txt");
    string line;
    string query;

    vector<word> dictionary;
    while(getline(words, line)) {
        dictionary.push_back({line, count_alphabet(line)});
    }
    sort(dictionary.begin(), dictionary.end(), compare_word);

    Trie trie;
    map<array<int, 26>, bool> memo;

    // 今までの単語で新しい単語を作ることができるか
    auto canMake = [&](auto & w) -> bool {
        auto dfs = [&](auto& self, array<int, 26> counts) -> bool {
            if(empty(counts)) return true;
            if(memo.contains(counts)) return memo[counts];

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
            min_dictionary.push_back(dictionary[i]);
            words_output << dictionary[i].original << '\n';
        }
        memo[dictionary[i].count] = true;
    }

    cout << (int) min_dictionary.size() << '\n';

    map<array<int, 26>, bool> ans_memo;
    int ans_score = 0;
    int nx = 0;

    while(getline(input, query)) {
        nx ++;

        auto alp = count_alphabet(query);
        unordered_map<string, int> query_memo;
        unordered_map<string, int> best_choice;

        auto initial_subset = trie.search_subset(alp);
        vector<int> valid_dict_indices;
        for(auto node : initial_subset) {
            valid_dict_indices.push_back(node->number);
        }

        vector<string> ans;

        auto subset = trie.search_subset(alp);
        auto dfs = [&] (auto & self, array<int, 26> counts) -> int {
            string state = encode_state(counts);
            if(query_memo.contains(state)) return query_memo[state];

            int best_id = -1;
            int mx = 0;

            for(int dict_idx : valid_dict_indices) {
                auto& d_word = dictionary[dict_idx];

                // 現在の counts でこの単語が作れるかチェック
                bool can_make = true;
                for(int i = 0; i < 26; ++i) {
                    if(counts[i] < d_word.count[i]) {
                        can_make = false;
                        break;
                    }
                }
                if(!can_make) continue;

                int word_score = calculate_word(d_word.count);
                for(int i = 0; i < 26; ++i) counts[i] -= d_word.count[i];
                int total = word_score + self(self, counts);
                for(int i = 0; i < 26; ++i) counts[i] += d_word.count[i];

                if (total > mx) {
                    mx = total;
                    best_id = dict_idx;
                }
            }
            best_choice[state] = best_id;
            return query_memo[state] = mx;
        };

        int final_max_score = dfs(dfs, alp);
        ans_score += final_max_score;

        vector<string> ans_words;
        array<int, 26> temp_counts = alp;
        string current_state = encode_state(temp_counts);

        // 経路復元
        while (!current_state.empty() && best_choice.contains(current_state) && best_choice[current_state] != -1) {
            int chosen_idx = best_choice[current_state];
            ans_words.push_back(dictionary[chosen_idx].original);

            for (int i=0; i<26; ++i) {
                temp_counts[i] -= dictionary[chosen_idx].count[i];
            }
            current_state = encode_state(temp_counts);
        }

        array<string, 2> space = {" ", ""};
        for(int i=0; i<(int) ans_words.size(); ++i) {
            output << ans_words[i] << space[i == (int) ans_words.size() - 1];
        }

        output << '\n';
        cout << nx << '\n';

    }
    cout << min_dictionary.size() << '\n';
}