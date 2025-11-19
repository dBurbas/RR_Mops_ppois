#include <vector>
#include <map>
#include <utility>
#include <string> 
#include <unordered_set>
std::map<std::string, std::vector<std::pair<std::string, std::string>>> adjacency_list;
// Рекурсивная реализация - вся логика
void DFSUtil(const std::string& vertex, std::unordered_set<std::string>& visited) {
    if (adjacency_list.find(vertex) == adjacency_list.end())
    {
        return;
    }

    visited.insert(vertex);
    // Какая то логика
    for (const auto& edge : adjacency_list[vertex]) {
        const std::string& neighbour = edge.first;
        const std::string& edge_info = edge.second;
        // Какая то логика с данными вершины
        if (visited.find(neighbour) == visited.end())
        {
            DFSUtil(neighbour,visited);
        }
    }
}
// Простой метод для вызова
void DFS(/*Здесь передавать или непередавать в зависимости как используешь*/) {
    std::unordered_set<std::string> visited;
    std::string start; ///< эту вершину ты берешь из adjacency_list
    DFSUtil(start, visited);
}