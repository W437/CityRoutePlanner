#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

// struct to represent an edge with a destination node & weight
struct Edge {
    string destination;
    int weight;
};

// each node points to a vector of edges
unordered_map<string, vector<Edge>> graph;

// trim whitespace from a string
string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

void loadGraphFromCSV(const string& filename) {
    ifstream file(filename);
    string line, source, destination, weight;

    getline(file, line); // Skip header

    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, source, ',');
        getline(ss, destination, ',');
        getline(ss, weight, ',');

        source = trim(source);
        destination = trim(destination);
        graph[source].push_back({ destination, stoi(weight) });
        graph[destination].push_back({ source, stoi(weight) }); // bi-dir edges
    }
}

// Dijkstra's algo to find the shortest path
unordered_map<string, pair<int, string>> dijkstra(const string& start) {
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;
    unordered_map<string, pair<int, string>> distances; // Pair: (distance, previous node)

    for (auto& pair : graph) {
        distances[pair.first] = { numeric_limits<int>::max(), "" };
    }
    distances[start] = { 0, "" };
    pq.push({ 0, start });

    while (!pq.empty()) {
        int current_distance = pq.top().first;
        string current_node = pq.top().second;
        pq.pop();

        for (const auto& edge : graph[current_node]) {
            int new_distance = current_distance + edge.weight;
            if (new_distance < distances[edge.destination].first) {
                distances[edge.destination] = { new_distance, current_node };
                pq.push({ new_distance, edge.destination });
            }
        }
    }

    return distances;
}

void printShortestPaths(const unordered_map<string, pair<int, string>>& distances, const string& start, const string& end) {
    for (const auto& pair : distances) {
        cout << "Shortest distance from " << "[" + start + "]" << " to " << "[" + pair.first + "]" << " is ";
        if (pair.second.first == numeric_limits<int>::max()) {
            cout << "infinity (no path)" << endl;
        }
        else {
            cout << pair.second.first << endl;
        }
    }

    cout << "Shortest distance from " << start << " to " << end << " is ";
    if (distances.at(end).first == numeric_limits<int>::max()) {
        cout << "infinity (no path)" << endl;
    }
    else {
        cout << distances.at(end).first << endl;
        cout << "\nPATH: ";
        string path_node = end;
        vector<string> path;
        while (path_node != start) {
            path.push_back(path_node);
            path_node = distances.at(path_node).second;
        }
        path.push_back(start);
        reverse(path.begin(), path.end());
        for (size_t i = 0; i < path.size(); ++i) {
            cout << "[" + path[i] + "]";
            if (i != path.size() - 1) {
                cout << " --> ";
            }
        }
        cout << endl;

        // Save the path to a file for visualization
        ofstream path_file("path.txt");
        for (size_t i = 0; i < path.size(); ++i) {
            path_file << path[i];
            if (i != path.size() - 1) {
                path_file << " --> ";
            }
        }
        path_file.close();


        // Trying to run the python script here, couldn't get it to run
        // current working dirr
        //fs::path current_path = fs::current_path();
        //cout << "Current working directory: " << current_path << endl;

        //// construct relative path
        //fs::path python_script_path = current_path / "Resource Files" / "draw_path.py";
        //cout << "Python script path: " << python_script_path << endl;

        //// Check if the Python script exists
        //if (!fs::exists(python_script_path)) {
        //    cerr << "Error: Python script not found at " << python_script_path << endl;
        //    return;
        //}

        //// Call the Python script to draw the path
        //string command = "python \"" + python_script_path.string() + "\" cities.csv path.txt";
        //cout << "Executing command: " << command << endl;
        //int result = system(command.c_str());
        //if (result != 0) {
        //    cerr << "Error executing Python script. Return code: " << result << endl;
        //}
    }
}

void displayAvailableNodes() {
    cout << "Available nodes (cities):" << endl;
    for (const auto& pair : graph) {
        cout << pair.first << endl;
    }
}

bool validateNode(const string& node) {
    return graph.find(node) != graph.end();
}

int main() {
    string filename, start_node, end_node;

    cout << "Enter the input file name (with extension (cities.csv)): ";
    cin >> filename;

    loadGraphFromCSV(filename);
    displayAvailableNodes();

    cout << "Enter the start node: ";
    cin.ignore();
    getline(cin, start_node);
    start_node = trim(start_node);

    if (!validateNode(start_node)) {
        cerr << "Invalid start node!" << endl;
        return 1;
    }

    cout << "Enter the end node: ";
    getline(cin, end_node);
    end_node = trim(end_node);

    if (!validateNode(end_node)) {
        cerr << "Invalid end node!" << endl;
        return 1;
    }

    unordered_map<string, pair<int, string>> distances = dijkstra(start_node);

    printShortestPaths(distances, start_node, end_node);

    return 0;
}
