use std::collections::{HashMap, HashSet, VecDeque};
use std::hash::Hash;
use std::num::NonZeroUsize;
use std::{env, vec};
use std::fs::File;
use std::io::{self, BufRead, BufReader};
use std::process;

struct Wikipedia {
    // A mapping from a page ID (integer) to the page title.
    titles: HashMap<usize, String>, // id が飛び飛びの可能性があるから、HashMap で持つ
    ids: HashMap<String, usize>, 
    // A set of page links.
    links: HashMap<usize, HashSet<usize>>,
}

impl Wikipedia {
    // Initialize the graph of pages.
    fn new(pages_file: &str, links_file: &str) -> io::Result<Self> {
        let mut titles = HashMap::new();
        let mut ids = HashMap::new();
        let mut links = HashMap::new();

        // Read the pages file into self.titles.
        let p_file = File::open(pages_file)?;
        for line in BufReader::new(p_file).lines() {
            let line = line?;
            let parts: Vec<&str> = line.trim_end().splitn(2, ' ').collect();
            if parts.len() == 2 {
                let id: usize = parts[0].parse::<NonZeroUsize>().expect("ID must be an integer").get() - 1;
                let title = parts[1].to_string();

                titles.insert(id.clone(),title.clone());
                ids.insert(title, id.clone());
                links.insert(id, HashSet::new());
            }
        }
        println!("Finished reading {}", pages_file);

        // Read the links file into self.links.
        let l_file = File::open(links_file)?;
        for line in BufReader::new(l_file).lines() {
            let line = line?;
            let parts: Vec<&str> = line.trim_end().split(' ').collect();
            if parts.len() == 2 {
                let src: usize = parts[0].parse::<NonZeroUsize>().expect("Source ID must be an integer").get()  - 1;
                let dst: usize = parts[1].parse::<NonZeroUsize>().expect("Destination ID must be an integer").get() - 1;
                
                links.entry(src).or_insert_with(HashSet::new).insert(dst);
            }
        }
        println!("Finished reading {}\n", links_file);

        Ok(Wikipedia { titles, ids , links })
    }

    // bfs をする
    fn find_shortest_path(&self, _start: &str, _goal: &str) {
        let mut que : VecDeque<usize> = VecDeque::new(); // これはqueueです
        let start = self.ids.get(_start).expect("start must exist");
        let goal = self.ids.get(_goal).expect("goal must exist");
        let mut seen: HashMap<usize, usize> = HashMap::new(); // seen に自分が来たノードを持たせる
        que.push_back(*start);
        while let Some(now) = que.pop_front() {
            if now == *goal {
                break;
            }
            if let Some(dests) = self.links.get(&now) {
                for &next in dests { // 今のノードから行けるノードを queue に入れる
                    if seen.contains_key(&next) {
                        continue;
                    }
                    seen.insert(next, now); // 二度 queue に入れずに済むように、ここで seen に入れる。
                    que.push_back(next);
                }
            }
        }
        // ここから経路復元
        if seen.contains_key(goal) {
            let mut path: Vec<&str> = Vec::new();
            let mut now = *goal;
            
            while now != *start {
                path.push(self.titles[&now].as_str());
                now = *seen.get(&now).unwrap();
            }
            path.push(self.titles[start].as_str());
            path.reverse();
            
            for node in path {
                println!("{} ->", node);
            }
        } else {
            println!("no path");
        }
    }

}

fn main() {
    let pages_file: &str = "../wikipedia_dataset/pages_large.txt";
    let links_file: &str = "../wikipedia_dataset/links_large.txt";

    let wikipedia = Wikipedia::new(pages_file, links_file).unwrap_or_else(|err| {
        eprintln!("Error: {}", err);
        process::exit(1);
    });

    // Homework #1
    wikipedia.find_shortest_path("渋谷", "パレートの法則");
}