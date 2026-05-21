#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using ll = long long;
using ull = unsigned long long;

constexpr string LOWER = "";
vector<int> SCOREBOARD = {1, 3, 2, 2, 1, 3, 3, 1, 1, 4, 4, 2, 2, 1, 1, 3, 4, 1, 1, 1, 2, 3, 3, 4, 3, 4};

struct word {
    string word;
    vector<int> alp;
};

auto to_vec(const string & word) {
    auto alp = vector(26, 0);
    for(int i=0; i<(int) word.size(); ++i) {
        ++alp[word[i] - 'a'];
    }
    return alp;
}

bool is_subset(const vector<int> & sub, const vector<int> & main) {
    for(int i=0; i<26; ++i) {
        if(sub[i] > main[i]) return false;
    }
    return true;
}

int calculate_word (string word) {
    int score = 0;
    for(int i=0; i<(int) word.size(); ++i) {
        score += SCOREBOARD[word[i] - 'a'];
    }
    return score;
}

int calculate_vec (const vector<int> & alp) {
    int score = 0;
    for(int i=0; i<26; ++i) {
        score += alp[i] * SCOREBOARD[i];
    }
    return score;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<pair<string, string>> dictionary0;
    vector<word> dictionary1;
    ifstream words("../scripts/anagram/words.txt");
    ifstream small("small.txt");
    ofstream small_ans("small_answer_hw2.txt");
    ifstream large("large.txt");
    ofstream large_ans("large_answer_hw2.txt");

    string word;
    string query;

    while(getline(words, word)) {
        string sorted_word = word;
        sort(sorted_word.begin(), sorted_word.end());
        dictionary0.push_back(make_pair(sorted_word, word));
        dictionary1.push_back({word, to_vec(word)});
    }
    sort(dictionary0.begin(), dictionary0.end());

    while(getline(large, query)) {
        int n = query.size();
        sort(query.begin(), query.end());
        string ans;
        int mx = 0;
        if(n < 13) {
            unordered_set<string> set;
            for(int i=1; i<(1<<n); ++i) {
                string s;
                for(int j=0; j<n; ++j) {
                    if(i & (1<<j)) s += query[j];
                }
                set.insert(s);
            }
            for(auto it = set.begin(); it != set.end(); ++it) {
                auto low_it = lower_bound(dictionary0.begin(), dictionary0.end(), make_pair(*it, LOWER));
                if(low_it != dictionary0.end() && low_it->first == *it) {
                    int res = calculate_word(*it);
                    if(res > mx) ans = *it;
                    mx = max(mx, res);
                }
            }
        } else {
            auto alp = to_vec(query);
            for(int i=0; i<(int) dictionary1.size(); ++i) {
                if(is_subset(dictionary1[i].alp, alp)) {
                    int res = calculate_vec(dictionary1[i].alp);
                    if(res > mx) ans = dictionary1[i].word;
                    mx = max(mx, res);
                }
            }
        }
        large_ans << ans << '\n';
    }

}