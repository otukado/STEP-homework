#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using ll = long long;
using ull = unsigned long long;


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // ファイルから入力もらうやり方知らなくて検索しちゃいました...
    vector<pair<string, string>> dictionary;
    ifstream words("../scripts/anagram/words.txt");
    ifstream input("./input/test1_input.txt");
    ofstream output("./output/test1_output.txt");
    string word;

    int mx = 0;
    while(getline(words, word)) {
        mx = max(mx, (int) word.size());
    }
    cout << mx << '\n';
}