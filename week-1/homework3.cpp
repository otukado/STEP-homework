#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using ll = long long;
using ull = unsigned long long;

struct dict {
    int length;
    string sorted;
    string word;
    vector<int> vec;
};

auto to_vec(const string & word) {
    auto alp = vector(26, 0);
    for(int i=0; i<(int) word.size(); ++i) {
        ++alp[word[i] - 'a'];
    }
    return alp;
}

bool compare_word(const dict & a, const dict & b) {
    if(a.length == b.length) {
        return a.sorted < b.sorted;
    } else {
        return a.length < b.length;
    }
};


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <array>

using namespace std;

// Trie木のノード構造体
struct TrieNode {
    array<TrieNode*, 26> child;
    bool is_end;
    TrieNode() {
        child.fill(nullptr);
        is_end = false;
    }
};

void insert(TrieNode* root, const string& s) {
    TrieNode* curr = root;
    for (char c : s) {
        int idx = c - 'a';
        if (!curr->child[idx]) {
            curr->child[idx] = new TrieNode();
        }
        curr = curr->child[idx];
    }
    curr->is_end = true;
}

unordered_map<string, int> memo;
TrieNode* root;

// カウント配列から構成可能な単語をTrie木で探索
void find_sub_words(TrieNode* curr, array<int, 26>& counts, string& current_word, int start_idx, vector<string>& matched_words) {
    if (curr->is_end) {
        matched_words.push_back(current_word);
    }
    for (int i = start_idx; i < 26; ++i) {
        if (counts[i] > 0 && curr->child[i]) {
            counts[i]--;
            current_word.push_back('a' + i);
            find_sub_words(curr->child[i], counts, current_word, i, matched_words);
            current_word.pop_back();
            counts[i]++;
        }
    }
}

// メモ化再帰 (DFS)
bool dfs(const string& s, bool is_root) {
    if (s.empty()) return true;
    if (memo.count(s)) return memo[s];

    array<int, 26> counts;
    counts.fill(0);
    for (char c : s) counts[c - 'a']++;

    vector<string> matched_words;
    string temp_word = "";
    find_sub_words(root, counts, temp_word, 0, matched_words);

    for (const string& q : matched_words) {
        if (is_root && q == s) continue;

        array<int, 26> next_counts = counts;
        for (char c : q) next_counts[c - 'a']--;
        
        string next_s = "";
        for (int i = 0; i < 26; ++i) {
            next_s.append(next_counts[i], 'a' + i);
        }

        if (dfs(next_s, false)) {
            return memo[s] = true;
        }
    }
    return memo[s] = false;
}

int main() {
    ifstream input_file("./input/test_word.txt");

    vector<string> original_words;
    unordered_map<string, int> freq; // ソート済み文字列の出現回数
    string word;

    while (getline(input_file, word)) {
        original_words.push_back(word);
        string sorted_word = word;
        sort(sorted_word.begin(), sorted_word.end());
        freq[sorted_word]++;
    }

    root = new TrieNode();
    for (auto& p : freq) {
        insert(root, p.first);
    }

    memo[""] = 1;
    vector<string> dictionary;
    for (const string& w : original_words) {
        string sorted_w = w;
        sort(sorted_w.begin(), sorted_w.end());
        if (freq[sorted_w] > 1) {
            continue;
        }
        if (dfs(sorted_w, true)) {
            continue;
        }
        dictionary.push_back(w);
    }

    cout << dictionary.size() << endl;


}

// int main() {
//     ios::sync_with_stdio(false);
//     cin.tie(nullptr);

//     ifstream words("words.txt");
//     ifstream small("small.txt");
//     ofstream small_ans("small_answer_hw2.txt");
//     ifstream large("large.txt");
//     ofstream large_ans("large_answer_hw2.txt");

//     string word;
//     vector<dict> dictionary;
//     vector<dict> unique_anagram;
//     while(getline(words, word)) {
//         string sorted_word = word;
//         sort(word.begin(), word.end());
//         dictionary.push_back({ (int) word.size(), sorted_word , word, to_vec(word)});
//     }
//     sort(dictionary.begin(), dictionary.end(), compare_word);
//     set<string> reachable;
//     for(auto & w : dictionary) {
//         if(!reachable.contains(w.word)) {
//             unique_anagram.push_back(w);
//             vector<string> 
//         }
//     }

// }