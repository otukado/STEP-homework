#include <bits/stdc++.h>

class Wikipedia {
public:
    std::vector<std::string> titles;
    std::unordered_map<std::string, int> ids;
    std::vector<std::vector<int>> links;
    int num_nodes = 0;

    // ほぼ呪文、ファイルから Wikipedia を作る
    Wikipedia(const std::string& pages_file, const std::string& links_file) {
        
        // 1. pages_file の読み込み
        FILE* p_file = fopen(pages_file.c_str(), "r");
        assert(p_file != nullptr && "Failed to open pages_file");
        
        int original_id;
        std::unordered_map<int, int> original_to_internal;
        // %d の後のスペースで区切り文字を消費
        while (fscanf(p_file, "%d ", &original_id) == 1) {
            char title_buf[4096];
            if (fgets(title_buf, sizeof(title_buf), p_file)) {
                std::string title(title_buf);
                while (!title.empty() && (title.back() == '\n' || title.back() == '\r')) {
                    title.pop_back();
                }
                
                int internal_id = num_nodes++;
                original_to_internal[original_id] = internal_id;
                titles.push_back(title);
                ids[title] = internal_id;
            }
        }
        fclose(p_file);
        std::cout << "Finished reading " << pages_file << std::endl;

        links.resize(num_nodes);
        FILE* l_file = fopen(links_file.c_str(), "r");
        assert(l_file != nullptr && "Failed to open links_file");
        
        int src, dst;
        // リンク情報は単純な数値ペアなので、substr 等を介さず fscanf で直接拾うのが最速
        while (fscanf(l_file, "%d %d", &src, &dst) == 2) {
            if (original_to_internal.contains(src) && original_to_internal.contains(dst)) {
                links[original_to_internal[src]].push_back(original_to_internal[dst]);
            }
        }
        fclose(l_file);
        std::cout << "Finished reading " << links_file << "\n\n";
    }

    // ここから課題
    struct Node {
        double prev_score = 1.0; //初期値は全てのノードで 1
        double new_score = 0.0;
    };

    bool is_converge(auto ranks) { // 収束したかどうかを判定する関数
        double sum = 0;
        for(const Node& rank : ranks) {
            sum += (rank.prev_score - rank.new_score) * (rank.prev_score - rank.new_score);
        }
        if(sum < 0.01) return true; // prev_score と new_score の差の2乗の和が 0.01 以下であれば収束と判定
        return false;
    }

    void assert_sum(std::vector<Node> ranks) { //全てのノードのページランクの合計が一致するかの判定
        double sum = 0;
        for(const auto& rank: ranks) {
            sum += rank.new_score;
        }
        assert(abs(sum - ranks.size()) < 0.00001);
    }

    void find_most_popular_pages() {
        int n = titles.size();
        std::vector<Node> page_rank(n);

        while(!is_converge(page_rank)) {
            for(int i=0; i<n; ++i) {
                page_rank[i].prev_score = page_rank[i].new_score;
                page_rank[i].new_score = 0.0;
            }
            for(int i=0; i<n; ++i) {
                auto& page = page_rank[i];
                int m = links[i].size();
                if(m == 0) {
                    page.new_score += page.prev_score; // 自分からのエッジがなければ、すべてのページランクがそのまま。
                }
                page.new_score += page.prev_score * 0.15; //15% が自分自身に残る
                page.prev_score -= page.new_score;
                for(const auto& link: links[i]) {
                    page_rank[link].new_score += page.prev_score / m; // 残りの85%は各子ノードに等分に分配する
                }
            }
        }

        std::vector<std::pair<int, int>> ranking(n); // page_rank と index の pair を持つ
        for(int i=0; i<n; ++i) {
            ranking[i] = std::make_pair(page_rank[i].new_score, i);
        }
        sort(ranking.rbegin(), ranking.rend()); //new_core の大きい順に並べる
        assert(ranking.size() >= 10);
        for(int i=0; i<10; ++i) {
            std::cout << titles[ranking[i].second] << '\n'; // 上位10個を出力
        }
    }

    

};

int main() {
    const auto pages_file = "../wikipedia_dataset/pages_medium.txt";
    const auto links_file = "../wikipedia_dataset/links_medium.txt";
    Wikipedia wikipedia_med(pages_file, links_file);
    wikipedia_med.find_most_popular_pages();
    
    const auto pages_file_large = "../wikipedia_dataset/pages_large.txt";
    const auto links_file_large = "../wikipedia_dataset/links_large.txt";
    Wikipedia wikipedia_large(pages_file_large, links_file_large);
    wikipedia_large.find_most_popular_pages();


    return 0;
}