#include <bits/stdc++.h>
#include <atcoder/all>

class Timer {
    std::chrono::high_resolution_clock::time_point start_time;
public:
    Timer() { start_time = std::chrono::high_resolution_clock::now(); }
    int elapsed_ms() {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(now - start_time).count();
    }
};


namespace otukado {


struct list {
    std::vector<int> next_node;
    std::vector<int> prev_node;

    list(int n): next_node(n, -1), prev_node(n, -1){};

    int begin = -1;
    int end = -1;
    int size = 0;

    void push_back(int next) {
        if(begin == -1) {
            begin = next; // 最初の要素の時
        } else {
            next_node[end] = next;
            prev_node[next] = end;
        }
        end = next; // end iterator は挿入した要素

        ++size;
    };
    void reverse() {
        std::swap(begin, end);
        prev_node.swap(next_node);
    }
    // u と v を含んだ new_path を渡す
    void change_path(int u, int v, const std::vector<int>& new_path) {
        int curr = u;
  
        while(curr != v) {
            prev_node[next_node[curr]] = 0;
            next_node[curr] = 0;
            ++curr;
            --size;
        }
        curr = u;
        for(const auto& next: new_path) {
            next_node[curr] = next;
            prev_node[next] = curr;
            ++size;
        }
    }
};
    
};
class Wikipedia {
public:
    std::vector<std::string> titles;
    std::unordered_map<std::string, int> ids;
    std::vector<std::vector<int>> links;
    int num_nodes = 0;

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

        // あらかじめ隣接リストを1回だけランダムシャッフルしておく
        std::random_device seed_gen;
        std::mt19937 engine(seed_gen());
        for (int i = 0; i < num_nodes; ++i) {
            std::shuffle(links[i].begin(), links[i].end(), engine);
        }
    }

    
    // dfs して経路を返す
    const otukado::list initial_path(const int& start, const int& goal, const int& n) {
        struct StackState {
            int node;
            int edge_idx;
        };
        std::vector<StackState> stack;
        stack.reserve(n);

        std::vector<bool> seen(n, false);
        std::vector<int> path_nodes;

        thread_local std::random_device seed_gen;
        thread_local std::mt19937 engine(seed_gen());

        // 開始ノードの初期化
        int start_m = links[start].size();
        int start_offset = start_m > 0 ? (engine() % start_m) : 0;
        stack.push_back({start, start_offset});
        seen[start] = true;
        path_nodes.push_back(start);

        while (!stack.empty()) {
            auto& [curr, edge_idx] = stack.back();
            if (curr == goal) {
                break; // ゴール到達
            }
            const auto& current_links = links[curr];
            int m = current_links.size();

            // 隣接頂点を探す
            bool moved = false;
            for (int i = 0; i < m; ++i) {
                int idx = edge_idx;
                // 次のループのためにインデックスを進める
                edge_idx = (edge_idx + 1) % m;

                int next = current_links[idx];
                if (!seen[next]) {
                    seen[next] = true;
                    path_nodes.push_back(next);
                    
                    int next_m = links[next].size();
                    int next_offset = next_m > 0 ? (engine() % next_m) : 0;
                    stack.push_back({next, next_offset});
                    
                    moved = true;
                    break; 
                }
            }

            if (!moved) {
                // 行き止まりのときバックトラックするが、seen は false に戻さない
                path_nodes.pop_back();
                stack.pop_back();
            }
        }

        otukado::list path(n);
        for (int node : path_nodes) {
            path.push_back(node);
        }
        return path;
    }

    // Homework #3 (optional): ヒューリスティクスを用いた最長経路の探索
    void find_longest_path(const std::string& _start, const std::string& _goal, const int& times) {
        const int start = this->ids.at(_start);
        const int goal = this->ids.at(_goal);
        
        int n = this->titles.size();
        auto current_path = initial_path(start, goal, n);

        #pragma omp parallel for
        for(int i=0; i<times; ++i) {
            auto candidate = initial_path(start, goal, n);

            #pragma omp critical
            {
                if(current_path.size < candidate.size) {
                    current_path = candidate;
                    std::cout << current_path.size <<'\n';
                }
            }
        }
        
        int it = current_path.begin;
        std::vector<int> ans;
        while(it != current_path.end) {
            ans.push_back(it);
            it = current_path.next_node[it];
        }
        ans.push_back(current_path.end);
        assert_path(ans, _start, _goal);
        std::cout << ans.size() << '\n';

        //ここから山登り法
    };

    // Homework #3 のためのヘルパー関数
    void assert_path(const std::vector<int>& path, const std::string& start, const std::string& goal) {
        assert(start != goal);
        assert(path.size() >= 2);
        assert(titles[path.front()] == start);
        assert(titles[path.back()] == goal);

        for (size_t i = 0; i < path.size() - 1; ++i) {
            const auto& current_links = links[path[i]];
            assert(std::find(current_links.begin(), current_links.end(), path[i + 1]) != current_links.end());
        }

        std::vector<bool> visited(num_nodes, false);
        for (int node : path) {
            assert(!visited[node]);
            visited[node] = true;
        }
    }
};

int main() {
    const auto pages_file = "../wikipedia_dataset/pages_medium.txt";
    const auto links_file = "../wikipedia_dataset/links_medium.txt";
    Wikipedia wikipedia_med(pages_file, links_file);
    // Homework #3 (optional)
    wikipedia_med.find_longest_path("渋谷", "池袋", 5000);

    const auto pages_file_large = "../wikipedia_dataset/pages_large.txt";
    const auto links_file_large = "../wikipedia_dataset/links_large.txt";
    Wikipedia wikipedia_large(pages_file_large, links_file_large);
    wikipedia_large.find_longest_path("渋谷", "池袋", 5000);


    return 0;
}