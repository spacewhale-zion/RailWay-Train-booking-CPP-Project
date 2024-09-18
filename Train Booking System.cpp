#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <climits>
#include <algorithm>

using namespace std;

// Define a structure to represent a graph edge
struct Edge {
    int destination;
    int weight;
};
// Define a class to represent the graph
class Graph {
    int V;
    vector<vector<Edge>> adjList;

public:
    Graph(int V) : V(V) {
        adjList.resize(V);
    }

    void addEdge(int src, int dest, int weight) {
        adjList[src].push_back({dest, weight});
        adjList[dest].push_back({src, weight});
    }

    vector<int> dijkstra(int start) const {
        vector<int> dist(V, INT_MAX);
        vector<int> parent(V, -1);
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

        dist[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();

            for (const Edge& edge : adjList[u]) {
                int v = edge.destination;
                int weight = edge.weight;

                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        return dist;
    }

    vector<int> getRoute(int start, int end, const vector<int>& parent) const {
        vector<int> route;
        for (int v = end; v != -1; v = parent[v]) {
            route.push_back(v);
        }
        reverse(route.begin(), route.end());
        return route;
    }
};




// Define a structure to represent a city with coordinates
struct City {
    string name;
    float x, y;
};

// Define a class to manage the map and routes
class Map {
    sf::RenderWindow window;
    sf::Texture mapTexture;
    sf::Sprite mapSprite;
    vector<City> cities;
    map<pair<string, string>, sf::Color> routes;
    map<string, int> cityMap;

public:
    Map() : window(sf::VideoMode(800, 600), "India Map") {
        // Load map image
        if (!mapTexture.loadFromFile("india_map.png")) {
            cerr << "Error loading map image" << endl;
            exit(1);
        }
        mapSprite.setTexture(mapTexture);

        // Add cities and their coordinates
        cities.push_back({"Delhi", 100, 100});
        cities.push_back({"Mumbai", 400, 300});
        cities.push_back({"Kolkata", 200, 400});
        cities.push_back({"Chennai", 500, 500});

        // Populate city map
        for (int i = 0; i < cities.size(); ++i) {
            cityMap[cities[i].name] = i;
        }

        // Define routes between cities
        routes[{"Delhi", "Mumbai"}] = sf::Color::Red;
        routes[{"Delhi", "Kolkata"}] = sf::Color::Blue;
        routes[{"Mumbai", "Chennai"}] = sf::Color::Green;
    }

    void draw(const vector<int>& distances, const vector<int>& parent, int start, int end) {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear();
            window.draw(mapSprite);

            // Draw routes
            for (const auto& route : routes) {
                const auto& startCity = route.first.first;
                const auto& endCity = route.first.second;
                const auto& color = route.second;

                auto startIt = find_if(cities.begin(), cities.end(), [&](const City& c) { return c.name == startCity; });
                auto endIt = find_if(cities.begin(), cities.end(), [&](const City& c) { return c.name == endCity; });

                if (startIt != cities.end() && endIt != cities.end()) {
                    sf::Vertex line[] = {
                        sf::Vertex(sf::Vector2f(startIt->x, startIt->y), color),
                        sf::Vertex(sf::Vector2f(endIt->x, endIt->y), color)
                    };
                    window.draw(line, 2, sf::Lines);
                }
            }

            // Draw city markers and distances
            for (const City& city : cities) {
                sf::CircleShape marker(5);
                marker.setFillColor(sf::Color::Red);
                marker.setPosition(city.x - marker.getRadius(), city.y - marker.getRadius());

                // Draw marker
                window.draw(marker);

                // Draw city label
                sf::Font font;
                if (!font.loadFromFile("arial.ttf")) {
                    cerr << "Error loading font" << endl;
                    exit(1);
                }
                sf::Text text;
                text.setFont(font);
                text.setString(city.name + " (" + to_string(distances[cityMap[city.name]]) + ")");
                text.setCharacterSize(15);
                text.setFillColor(sf::Color::White);
                text.setPosition(city.x + 10, city.y - 10);

                window.draw(text);
            }

            // Draw the route from start to end
            if (start != end) {
                vector<int> route = getRoute(start, end, parent);
                for (size_t i = 0; i < route.size() - 1; ++i) {
                    const City& startCity = cities[route[i]];
                    const City& endCity = cities[route[i + 1]];
                    sf::Vertex line[] = {
                        sf::Vertex(sf::Vector2f(startCity.x, startCity.y), sf::Color::Yellow),
                        sf::Vertex(sf::Vector2f(endCity.x, endCity.y), sf::Color::Yellow)
                    };
                    window.draw(line, 2, sf::Lines);
                }
            }

            window.display();
        }
    }

private:
    vector<int> getRoute(int start, int end, const vector<int>& parent) {
        vector<int> route;
        for (int v = end; v != -1; v = parent[v]) {
            route.push_back(v);
        }
        reverse(route.begin(), route.end());
        return route;
    }
};

// Define a class to manage passenger details
class Passenger {
public:
    int id;
    string name;

    Passenger(int id, string name) : id(id), name(name) {}
};

// Define a class to manage bookings
class Booking {
    int passengerId;
    int trainId;
    int startStation;
    int endStation;

public:
    Booking(int passengerId, int trainId, int startStation, int endStation)
        : passengerId(passengerId), trainId(trainId), startStation(startStation), endStation(endStation) {}

    void printBooking(const map<int, string>& idToStationMap) {
        cout << "Passenger ID: " << passengerId << ", Train ID: " << trainId
             << ", From Station: " << idToStationMap.at(startStation)
             << " to Station: " << idToStationMap.at(endStation) << endl;
    }

    bool validateRoute(const Graph& g, int start, int end, vector<int>& parent) {
        vector<int> distances = g.dijkstra(start);
        return distances[end] != INT_MAX;
    }
};

// Define a class to manage train details
class Train {
public:
    int id;
    string name;
    vector<int> stations;

    Train(int id, string name, vector<int> stations) : id(id), name(name), stations(stations) {}

    void printTrainInfo() {
        cout << "Train ID: " << id << ", Name: " << name << endl;
    }
};

// Define a class to manage travel times
class TravelTime {
    map<pair<int, int>, int> travelTimes;  // (startStation, endStation) -> time

public:
    void addTravelTime(int start, int end, int time) {
        travelTimes[{start, end}] = time;
    }

    int getTravelTime(int start, int end) {
        if (travelTimes.find({start, end}) != travelTimes.end()) {
            return travelTimes[{start, end}];
        }
        return INT_MAX;  // Return a large value if no travel time is found
    }

    string formatTime(int minutes) {
        int hours = minutes / 60;
        minutes %= 60;
        return to_string(hours) + "h " + to_string(minutes) + "m";
    }
};

// Main function
int main() {
    int numStations = 4;
    Graph g(numStations);

    map<string, int> stationMap = {
        {"Delhi", 0},
        {"Mumbai", 1},
        {"Kolkata", 2},
        {"Chennai", 3}
    };

    map<int, string> idToStationMap = {
        {0, "Delhi"},
        {1, "Mumbai"},
        {2, "Kolkata"},
        {3, "Chennai"}
    };

    g.addEdge(stationMap["Delhi"], stationMap["Mumbai"], 10);
    g.addEdge(stationMap["Delhi"], stationMap["Chennai " ],20);
      g.addEdge(stationMap["Delhi"], stationMap["Chennai"], 20);
    g.addEdge(stationMap["Mumbai"], stationMap["Chennai"], 30);
    g.addEdge(stationMap["Mumbai"], stationMap["Kolkata"], 25);
    g.addEdge(stationMap["Kolkata"], stationMap["Chennai"], 15);

    // Create the map instance
    Map map;

    // Create a travel time manager and add travel times
    TravelTime travelTime;
    travelTime.addTravelTime(stationMap["Delhi"], stationMap["Mumbai"], 10);
    travelTime.addTravelTime(stationMap["Delhi"], stationMap["Chennai"], 20);
    travelTime.addTravelTime(stationMap["Mumbai"], stationMap["Chennai"], 30);
    travelTime.addTravelTime(stationMap["Mumbai"], stationMap["Kolkata"], 25);
    travelTime.addTravelTime(stationMap["Kolkata"], stationMap["Chennai"], 15);

    // Example of booking
    int startStation = stationMap["Delhi"];
    int endStation = stationMap["Chennai"];

    vector<int> parent(numStations, -1);
    vector<int> distances = g.dijkstra(startStation);

    Booking booking(1, 101, startStation, endStation);

    if (booking.validateRoute(g, startStation, endStation, parent)) {
        cout << "Route exists between " << idToStationMap[startStation] << " and " << idToStationMap[endStation] << "." << endl;
        booking.printBooking(idToStationMap);
        
        int travelTimeMinutes = travelTime.getTravelTime(startStation, endStation);
        cout << "Travel time: " << travelTime.formatTime(travelTimeMinutes) << endl;

        // Draw the map with the route
        map.draw(distances, parent, startStation, endStation);
    } else {
        cout << "No route found between " << idToStationMap[startStation] << " and " << idToStationMap[endStation] << "." << endl;
    }

    return 0;
}

