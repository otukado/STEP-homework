use std::collections::{HashMap, HashSet, VecDeque};
use std::{env, vec};
use std::fs::File;
use std::io::{self, BufRead, BufReader};
use std::process;

struct Wikipedia {
    // A mapping from a page ID (integer) to the page title.
    titles: HashMap<u32, String>,
    // A set of page links.
    links: HashMap<u32, Vec<u32>>,
}

impl Wikipedia {
    // Initialize the graph of pages.
    fn new(pages_file: &str, links_file: &str) -> io::Result<Self> {
        let mut titles = HashMap::new();
        let mut links = HashMap::new();

        // Read the pages file into self.titles.
        let p_file = File::open(pages_file)?;
        for line in BufReader::new(p_file).lines() {
            let line = line?;
            let parts: Vec<&str> = line.trim_end().splitn(2, ' ').collect();
            if parts.len() == 2 {
                let id: u32 = parts[0].parse().expect("ID must be an integer");
                let title = parts[1].to_string();
                
                assert!(!titles.contains_key(&id), "{}", id);
                titles.insert(id, title);
                links.insert(id, Vec::new());
            }
        }
        println!("Finished reading {}", pages_file);

        // Read the links file into self.links.
        let l_file = File::open(links_file)?;
        for line in BufReader::new(l_file).lines() {
            let line = line?;
            let parts: Vec<&str> = line.trim_end().split(' ').collect();
            if parts.len() == 2 {
                let src: u32 = parts[0].parse().expect("Source ID must be an integer");
                let dst: u32 = parts[1].parse().expect("Destination ID must be an integer");
                
                assert!(titles.contains_key(&src), "{}", src);
                assert!(titles.contains_key(&dst), "{}", dst);
                
                if let Some(dests) = links.get_mut(&src) {
                    dests.push(dst);
                }
            }
        }
        println!("Finished reading {}\n", links_file);

        Ok(Wikipedia { titles, links })
    }

    // できない、
    fn find_id_from_title(&self, _title: &str) -> u32 {
        for &id in self.titles.keys() {
            if self.titles[&id] == _title {
                let res = id;
                return res;
            }
        }
        return 0;
    }

    // Example: Find the longest titles.
    fn find_longest_titles(&self) {
        let mut titles_vec: Vec<&String> = self.titles.values().collect();
        // In Rust, we use chars().count() to correctly handle multi-byte characters.
        titles_vec.sort_by(|a, b| b.chars().count().cmp(&a.chars().count()));

        println!("The longest titles are:");
        let mut count = 0;
        let mut index = 0;
        
        while count < 15 && index < titles_vec.len() {
            if !titles_vec[index].contains('_') {
                println!("{}", titles_vec[index]);
                count += 1;
            }
            index += 1;
        }
        println!();
    }

    // Example: Find the most linked pages.
    fn find_most_linked_pages(&self) {
        let mut link_count: HashMap<u32, u32> = HashMap::new();
        for &id in self.titles.keys() {
            link_count.insert(id, 0);
        }

        for dests in self.links.values() {
            for &dst in dests {
                if let Some(count) = link_count.get_mut(&dst) {
                    *count += 1;
                }
            }
        }

        println!("The most linked pages are:");
        if let Some(&max_count) = link_count.values().max() {
            for (&dst, &count) in &link_count {
                if count == max_count {
                    println!("{} {}", self.titles[&dst], max_count);
                }
            }
        }
        println!();
    }

    // Homework #1: Find the shortest path.
    // 'start': A title of the start page.
    // 'goal': A title of the goal page.

    // bfs をする
    fn find_shortest_path(&self, _start: &str, _goal: &str) {
        let mut que : VecDeque<u32> = VecDeque::new();
        let start = self.find_id_from_title(_start);
        let goal = self.find_id_from_title(_goal);
        let mut seen: Vec<u32> = [-1; self.titles.size()+1];
        que.push_back(start);
        while !que.is_empty() {
            let now = que.pop_front().unwrap();
            if now == goal {
                break;
            }
            for next in &self.links[&now] {
                if seen[next] != -1 {
                    seen[next] = now;
                    que.push_back(*next);
                }
            }
        }
        if seen[goal] == -1 {
            print!("no path");
        } else {
            let mut path: Vec<&str> = Vec::new();
            let mut now = goal;
            while(now != start) {
                path.push(&now);
                now = seen[now];
            }
        }
    }

    // Homework #2: Calculate the page ranks and print the most popular pages.
    fn find_most_popular_pages(&self) {
        //------------------------#
        // Write your code here!  #
        //------------------------#
    }

    // Homework #3 (optional):
    // Search the longest path with heuristics.
    // 'start': A title of the start page.
    // 'goal': A title of the goal page.
    fn find_longest_path(&self, _start: &str, _goal: &str) {
        //------------------------#
        // Write your code here!  #
        //------------------------#
    }

    // Helper function for Homework #3:
    // Please use this function to check if the found path is well formed.
    // 'path': An array of page IDs that stores the found path.
    //      path[0] is the start page. path[-1] is the goal page.
    //      path[0] -> path[1] -> ... -> path[-1] is the path from the start
    //      page to the goal page.
    // 'start': A title of the start page.
    // 'goal': A title of the goal page.
    fn assert_path(&self, path: &[u32], start: &str, goal: &str) {
        assert!(start != goal);
        assert!(path.len() >= 2);
        assert_eq!(self.titles[&path[0]], start);
        assert_eq!(self.titles[&path[path.len() - 1]], goal);

        for i in 0..(path.len() - 1) {
            assert!(self.links[&path[i]].contains(&path[i + 1]));
        }

        let mut visited = HashSet::new();
        for &node in path {
            assert!(!visited.contains(&node));
            visited.insert(node);
        }
    }
}

fn main() {
    let pages_file: &str = "../wikipedia_dataset/pages_small.txt";
    let links_file: &str = "../wikipedia_dataset/links_small.txt";

    let wikipedia = Wikipedia::new(pages_file, links_file).unwrap_or_else(|err| {
        eprintln!("Error: {}", err);
        process::exit(1);
    });

    // Example
    wikipedia.find_longest_titles();
    // Example
    wikipedia.find_most_linked_pages();
    wikipedia.find_shortest_path("A", "D");
    // Homework #1
    // wikipedia.find_shortest_path("渋谷", "パレートの法則");
    // // Homework #2
    // wikipedia.find_most_popular_pages();
    // // Homework #3 (optional)
    // wikipedia.find_longest_path("渋谷", "池袋");
}