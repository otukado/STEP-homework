#include <bits/stdc++.h>
#include <cmath>
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
        for (size_t i = 0; i < new_path.size() - 1; ++i) {
            int from = new_path[i];
            int to = new_path[i + 1];
            
            next_node[from] = to;
            prev_node[to] = from;
            
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
        // for (int i = 0; i < num_nodes; ++i) {
        //     std::shuffle(links[i].begin(), links[i].end(), engine);
        // }
        for (int i = 0; i < num_nodes; ++i) {
            std::sort(links[i].begin(), links[i].end(), 
            [&](int a, int b) {
                // 接続先（aとb）が持つリンク数（次数）を比較し、少ない順にする
                return links[a].size() < links[b].size();
            });
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

private:
    // 山登り法の処理と状態をカプセル化したヘルパー構造体
    struct OptimizerState {
        int num_nodes;
        const std::vector<std::vector<int>>& links;
        uint32_t seed;
        
        // 探索状態を保持するバッファ（メンバ変数にすることで再確保を防ぐ）
        std::vector<int> visited_time;
        int current_time;
        
        struct StackState { int node; int edge_idx; };
        std::vector<StackState> stack;
        std::vector<int> current_dfs_path;

        // コンストラクタでバッファを一度だけ確保
        OptimizerState(int n, const std::vector<std::vector<int>>& l)
            : num_nodes(n), links(l), seed(2463534242), visited_time(n, 0), current_time(0) {
            stack.reserve(num_nodes);
            current_dfs_path.reserve(num_nodes);
        }

        inline uint32_t next_rand() {
            seed ^= (seed << 13);
            seed ^= (seed >> 17);
            seed ^= (seed << 5);
            return seed;
        }

        // 1. パス上の頂点 u, v をランダムに選択する
        bool select_segment(const otukado::list& path, int& u, int& v, float ratio) {
            // 確率 1/10 でパス上の頂点 u を探す
            while (true) {
                int candidate = next_rand() % num_nodes;
                if (path.next_node[candidate] != -1) {
                    u = candidate;
                    break;
                }
            }

            // u から 3〜10歩後ろの頂点 v を選ぶ
            int max_skip = 50 - static_cast<int>(45 * ratio); 
            int min_skip = 3;
            int skip = min_skip + (next_rand() % (max_skip - min_skip + 1));
            v = u;
            for (int i = 0; i < skip; ++i) {
                v = path.next_node[v];
                if (v == -1) return false; // パスの終端を突き抜けた場合は無効
            }
            return (v != path.end);
        }

        // 2. 既存のパスを切り離し、古い部分パスを退避させる
        std::vector<int> disconnect_segment(otukado::list& path, int u, int v) {
            std::vector<int> old_subpath;
            int curr = u;
            while (curr != v) {
                old_subpath.push_back(curr);
                int nxt = path.next_node[curr];
                
                path.next_node[curr] = -1;
                path.prev_node[nxt] = -1;
                --path.size;
                curr = nxt;
            }
            old_subpath.push_back(v);
            return old_subpath;
        }

        bool run_local_dfs(int u, int v, const otukado::list& path) {
            if (current_time > 2000000000) {
                std::fill(visited_time.begin(), visited_time.end(), 0);
                current_time = 0;
            }

            ++current_time;
            stack.clear();
            current_dfs_path.clear();

            int start_m = links[u].size();
            stack.push_back({u, start_m > 0 ? (int)(next_rand() % start_m) : 0});
            visited_time[u] = current_time;
            current_dfs_path.push_back(u);

            while (!stack.empty()) {
                auto& [c_node, edge_idx] = stack.back();
                if (c_node == v) return true; 

                const auto& current_links = links[c_node];
                int m = current_links.size();
                bool moved = false;

                for (int i = 0; i < m; ++i) {
                    int idx = edge_idx;
                    edge_idx = (edge_idx + 1) % m;
                    int next_node = current_links[idx];

                    if (visited_time[next_node] != current_time && 
                       (path.next_node[next_node] == -1 && (next_node != path.end) || (next_node == v))) {
                        
                        visited_time[next_node] = current_time;
                        current_dfs_path.push_back(next_node);
                        

                        if (next_rand() % 100 < 95) {
                            stack.push_back({next_node, 0}); // ランダムに選んだ隣接ノードから DFS を開始
                        } else {
                            int next_m = links[next_node].size();
                            stack.push_back({next_node, next_m > 0 ? (int)(next_rand() % next_m) : 0});
                        }
                        moved = true;
                        break;
                    }
                }

                if (!moved) {
                    current_dfs_path.pop_back();
                    stack.pop_back();
                }
            }
            return false;
        }

        // 4. 改悪時に元のパスへ戻す（ロールバック）
        void rollback(otukado::list& path, const std::vector<int>& old_subpath) {
            for (size_t i = 0; i < old_subpath.size() - 1; ++i) {
                int a = old_subpath[i];
                int b = old_subpath[i + 1];
                path.next_node[a] = b;
                path.prev_node[b] = a;
                ++path.size;
            }
        }
    };

public:
    void simulated_annealing(otukado::list& path, OptimizerState& state) {
        
        Timer timer;
        const int TIME_LIMIT_MS = 150000; 

        const double start_temp = 50.0;
        const double end_temp = 0.1;

        int iter_count = 0;
        int update_count = 0;

        const int CHECK_INTERVAL = 1024;
        // otukado::list max_state = path;
        int elapsed = 0;

        while (true) {
            if ((iter_count & (CHECK_INTERVAL - 1)) == 0) {
                elapsed = timer.elapsed_ms();
                if (elapsed >= TIME_LIMIT_MS) break;
                
                double time_ratio = static_cast<double>(elapsed) / TIME_LIMIT_MS;
                auto temp = start_temp * std::pow(end_temp / start_temp, time_ratio);
            }
            ++iter_count;
            ++iter_count;

            int u, v; //u,v を決定
            if (!state.select_segment(path, u, v, static_cast<double>(elapsed) / TIME_LIMIT_MS)) continue;

            std::vector<int> old_subpath = state.disconnect_segment(path, u, v);

            // 局所DFSの実行
            bool found = state.run_local_dfs(u, v, path);
            bool accept = false;

            if(found) {
                int diff = (state.current_dfs_path.size()) - (old_subpath.size());
                if(diff > 0) accept = true;
                else {
                    double time_ratio = static_cast<double>(elapsed) / TIME_LIMIT_MS;
                    double temp = start_temp * pow(end_temp / start_temp, time_ratio);
                    
                    double prob = std::exp(diff / temp);
                    
                    // 0.0 ~ 1.0 の乱数を生成して確率判定
                    double r = (state.next_rand() % 1000000000) / 1e9;
                    if (r < prob) {
                        accept = true;
                    }
                }
            }

            if (accept) {
                path.change_path(u, v, state.current_dfs_path);
                ++update_count;
                // if(path.size > max_state.size) {
                //     max_state = path;
                // }
            } else {
                state.rollback(path, old_subpath);
            }
        }
        std::cout << "update_count: " << update_count << '\n';
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

        OptimizerState state(num_nodes, links);

        //ここから焼きなまし
        simulated_annealing(current_path, state);
        
        int it = current_path.begin;
        std::vector<int> ans;
        while(it != current_path.end) {
            ans.push_back(it);
            it = current_path.next_node[it];
        }
        ans.push_back(current_path.end);
        assert_path(ans, _start, _goal);
        std::cout << ans.size() << '\n';

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
    // const auto pages_file = "../wikipedia_dataset/pages_medium.txt";
    // const auto links_file = "../wikipedia_dataset/links_medium.txt";
    // Wikipedia wikipedia_med(pages_file, links_file);
    // wikipedia_med.find_longest_path("渋谷", "池袋", 300);

    const auto pages_file_large = "../wikipedia_dataset/pages_large.txt";
    const auto links_file_large = "../wikipedia_dataset/links_large.txt";
    Wikipedia wikipedia_large(pages_file_large, links_file_large);
    wikipedia_large.find_longest_path("渋谷", "池袋", 500);


    return 0;
}