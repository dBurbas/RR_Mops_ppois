#include <queue>
#include <unordered_set>
#include <map>
#include <utility>
#include <string> 
#include <vector>
std::map<std::string, std::vector<std::pair<std::string, std::string>>> adjacency_list;
// Итеративная реализация - вся логика
void BFSUtil(const std::string& start,std::unordered_set<std::string>& visited){
    std::queue<std::string> queue;
    queue.push(start);
    visited.insert(start);

    while (!queue.empty()) {
        std::string current_vertex = queue.front();
        // Какая то логика
        queue.pop();

        auto iter = adjacency_list.find(current_vertex);
        if (iter == adjacency_list.end()) continue;
        
        for(const auto& [neighbour, edge_info] : iter->second){
            // Какая то логика с инфой вершины
            if (visited.find(neighbour) == visited.end())
            {
                visited.insert(neighbour);
                queue.push(neighbour);
            }
            
        }
    }
}
// Простой метод для вызова
void BFS(/*Здесь передавать или непередавать в зависимости как используешь*/){
    std::unordered_set<std::string> visited;
    std::string start; ///< эту вершину ты берешь из adjacency_list
    BFSUtil(start, visited);
}