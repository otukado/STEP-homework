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
    ifstream pages("../wikipedia_dataset/pages_small.txt");
    ifstream links("../wikipedia_dataset/links_small.txt");    
    // ofstream output("./output/test1_output.txt");
    vector<string> page_name;
    string page;

    while(getline(pages, page)) {
        stringstream ss(page);
        string _idx, name;
        ss >> _idx >> name;
    }

    while(getline(words, word)) {
        string sorted_word = word;
        sort(sorted_word.begin(), sorted_word.end());
        dictionary.push_back(make_pair(sorted_word, word));
    }
    sort(dictionary.begin(), dictionary.end());

}