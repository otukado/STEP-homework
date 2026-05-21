#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using ll = long long;
using ull = unsigned long long;

constexpr string LOWER = "";


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // ファイルから入力もらうやり方知らなくて検索しちゃいました...
    vector<pair<string, string>> dictionary;
    ifstream words("../scripts/anagram/words.txt");
    ifstream input("./input/test1_input.txt");
    ofstream output("./output/test1_output.txt");
    string word;
    string query;

    while(getline(words, word)) {
        string sorted_word = word;
        sort(sorted_word.begin(), sorted_word.end());
        dictionary.push_back(make_pair(sorted_word, word));
    }
    sort(dictionary.begin(), dictionary.end());

    while(getline(input, query)) {
        string sorted_query = query;
        sort(sorted_query.begin(), sorted_query.end());
        auto first_it = lower_bound(dictionary.begin(), dictionary.end(), make_pair(sorted_query, LOWER));
        for(auto it = first_it; it != dictionary.end() && it->first == sorted_query; ++it) {
            output << query << ": " << it->second << '\n';
        }
    }

}