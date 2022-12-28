#include "graph.h"
#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <stack>
#include <utility>
#include <vector>

using namespace std;

// constructor, empty graph
// directionalEdges defaults to true
Graph::Graph(bool directionalEdges) { directional = directionalEdges; }

// destructor
Graph::~Graph() {
  for (auto const &curr : vertices) {
    delete curr.second;
  }
  vertices.clear();
}

// @return total number of vertices
int Graph::verticesSize() const { return vertices.size(); }

// @return total number of edges
int Graph::edgesSize() const {
  map<Vertex *, set<Vertex *>> visited;
  int total = 0;
  for (auto const &i : vertices) {
    if (directional) {
      total += i.second->connected.size();
    } else {
      for (auto const &j : i.second->connected) {
        if (visited.count(i.second) == 0 ||
            visited[i.second].count(j.first) == 0) {
          visited[i.second].insert(j.first);
          visited[j.first].insert(i.second);
          total++;
        }
      }
    }
  }
  return total;
}

// @return number of edges from given vertex, -1 if vertex not found
int Graph::vertexDegree(const string &label) const {
  return vertices.count(label) == 1 ? vertices.at(label)->connected.size() : -1;
}

// @return true if vertex added, false if it already is in the graph
bool Graph::add(const string &label) {
  if (vertices.count(label) == 1) {
    return false;
  }
  vertices[label] = new Vertex{label, {}};
  return true;
}

/** return true if vertex already in graph */
bool Graph::contains(const string &label) const {
  return vertices.count(label) == 1;
}

// @return string representing edges and weights, "" if vertex not found
// A-3->B, A-5->C should return B(3),C(5)
string Graph::getEdgesAsString(const string &label) const {
  string rtn;
  if (vertices.count(label) == 0 || vertices.at(label)->connected.empty()) {
    return rtn;
  }
  // push string rep into set w/ comparator lambda
  auto comp = [](pair<Vertex *, int> a, pair<Vertex *, int> b) {
    return a.first->val < b.first->val;
  };
  set<pair<Vertex *, int>, decltype(comp)> connected(
      vertices.at(label)->connected.begin(),
      vertices.at(label)->connected.end(), comp);
  // Print the sorted value
  for (auto &i : connected) {
    rtn += i.first->val + "(" + to_string(i.second) + "),";
  }
  rtn.pop_back();
  return rtn;
}

// @return true if successfully connected
bool Graph::connect(const string &from, const string &to, int weight) {
  if (from == to) {
    return false;
  }
  if (vertices.count(from) == 0) {
    vertices[from] = new Vertex{from, {}};
  }
  if (vertices.count(to) == 0) {
    vertices[to] = new Vertex{to, {}};
  }
  if (vertices[from]->connected.count(vertices[to]) == 1) {
    return false;
  }
  vertices[from]->connected[vertices[to]] = weight;
  if (!directional) {
    vertices[to]->connected[vertices[from]] = weight;
  }
  return true;
}

// disconnects two vertex's from each other
bool Graph::disconnect(const string &from, const string &to) {
  if (vertices.count(from) == 0 || vertices.count(to) == 0) {
    return false;
  }
  if (vertices[from]->connected.count(vertices[to]) == 0) {
    return false;
  }
  vertices[from]->connected.erase(vertices[to]);
  if (!directional) {
    vertices[to]->connected.erase(vertices[from]);
  }
  return true;
}

// depth-first traversal starting from given startLabel
void Graph::dfs(const string &startLabel, void visit(const string &label)) {
  if (vertices.count(startLabel) == 1) {
    set<Vertex *> visited;
    visited.insert(vertices[startLabel]);
    dfsTraverse(visited, vertices[startLabel], visit);
  }
}

// recursive method for DFS traversal
void Graph::dfsTraverse(set<Vertex *> &visited, Vertex *curr,
                        void visit(const string &label)) {
  visit(curr->val);
  auto comp = [](pair<Vertex *, int> a, pair<Vertex *, int> b) {
    return a.first->val < b.first->val;
  };
  set<pair<Vertex *, int>, decltype(comp)> connected(
      curr->connected.begin(), curr->connected.end(), comp);
  for (auto const &i : connected) {
    if (visited.count(i.first) == 0) {
      visited.insert(i.first);
      dfsTraverse(visited, i.first, visit);
    }
  }
}

// breadth-first traversal starting from startLabel
void Graph::bfs(const string &startLabel, void visit(const string &label)) {
  set<Vertex *> visited;
  queue<Vertex *> q;
  q.push(vertices[startLabel]);
  visited.insert(vertices[startLabel]);
  Vertex *curr;
  auto comp = [](pair<Vertex *, int> a, pair<Vertex *, int> b) {
    return a.first->val < b.first->val;
  };
  while (!q.empty()) {
    curr = q.front();
    q.pop();
    visit(curr->val);
    set<pair<Vertex *, int>, decltype(comp)> connected(
        curr->connected.begin(), curr->connected.end(), comp);
    for (auto const &i : connected) {
      if (visited.count(i.first) == 0) {
        q.push(i.first);
        visited.insert(i.first);
      }
    }
  }
}

// find the total weight to every Vertex on the
// connected graph from the start Label
pair<map<string, int>, map<string, string>>
Graph::dijkstra(const string &startLabel) const {
  map<string, int> weights;
  map<string, string> previous;
  if (vertices.count(startLabel) == 0) {
    return make_pair(weights, previous);
  }
  set<string> visited;
  queue<Edge> edges;
  visited.insert(startLabel);
  for (auto const &j : vertices.at(startLabel)->connected) {
    edges.push({vertices.at(startLabel), j.second, j.first});
  }
  int prevWeight;
  int toWeight;
  while (!edges.empty()) {
    Edge edge = edges.front();
    edges.pop();
    prevWeight =
        weights.count(edge.to->val) == 1 ? weights[edge.to->val] : INT_MAX;
    toWeight = weights.count(edge.from->val) == 1
                   ? edge.weight + weights[edge.from->val]
                   : edge.weight;
    if (prevWeight > toWeight) {
      weights[edge.to->val] = toWeight;
      previous[edge.to->val] = edge.from->val;
    }
    if (visited.count(edge.to->val) == 0) {
      visited.insert(edge.to->val);
      for (auto const &i : edge.to->connected) {
        if (i.first->val != startLabel) {
          edges.push({edge.to, i.second, i.first});
        }
      }
    }
  }
  return make_pair(weights, previous);
}

// minimum spanning tree using Prim's algorithm
int Graph::mstPrim(const string &startLabel,
                   void visit(const string &from, const string &to,
                              int weight)) const {
  if (directional || vertices.empty() || vertices.count(startLabel) == 0) {
    return -1;
  }
  set<Vertex *> visited;
  // making comparator
  auto compare = [](Edge a, Edge b) { return a.weight > b.weight; };
  // declaring prio queue
  priority_queue<Edge, vector<Edge>, decltype(compare)> edges(compare);
  // finding + adding all edges from starting vertex
  visited.insert(vertices.at(startLabel));
  for (auto const &i : vertices.at(startLabel)->connected) {
    edges.push({vertices.at(startLabel), i.second, i.first});
  }
  int weight = 0;
  while (!edges.empty()) {
    // choose lowest weight edge and "add" to the mst
    Edge curr = edges.top();
    edges.pop();
    if (visited.count(curr.to) == 0) {
      weight += curr.weight;
      visited.insert(curr.to);
      visit(curr.from->val, curr.to->val, curr.weight);
      // finding + adding all edges from end node of lowest
      // weight edge chosen previously
      for (auto const &i : curr.to->connected) {
        if (visited.count(i.first) == 0) {
          edges.push({curr.to, i.second, i.first});
        }
      }
    }
  }
  return weight;
}

// minimum spanning tree using Kruskal's algorithm
int Graph::mstKruskal(const string &startLabel,
                      void visit(const string &from, const string &to,
                                 int weight)) const {
  // edge case check
  if (directional || vertices.empty() || vertices.count(startLabel) == 0) {
    return -1;
  }
  // prio queue for edges sorted by lowest weight
  auto compare = [](Edge a, Edge b) { return a.weight > b.weight; };
  priority_queue<Edge, vector<Edge>, decltype(compare)> edges(compare);
  // strcutures used to formulate all edges of the graph
  set<Vertex *> visited;
  queue<Vertex *> q;
  Vertex *curr;
  visited.insert(vertices.at(startLabel));
  q.push(vertices.at(startLabel));
  map<Vertex *, set<Vertex *>> origin;
  // use bfs to add all edges to the prio queue
  while (!q.empty()) {
    curr = q.front();
    q.pop();
    for (auto const &i : curr->connected) {
      if (visited.count(i.first) == 0) {
        q.push(i.first);
        visited.insert(i.first);
      }
      if (origin[curr].count(i.first) == 0) {
        origin[i.first].insert(curr);
        edges.push({curr, i.second, i.first});
      }
    }
  }
  // go through the prio queue, adding if not a cycle
  // ending when the prio queue is empty
  origin.clear();
  Edge edge;
  int weight = 0;
  while (!edges.empty()) {
    edge = edges.top();
    edges.pop();
    if (!cycle(origin, edge)) {
      origin[edge.from].insert(edge.to);
      origin[edge.to].insert(edge.from);
      visit(edge.from->val, edge.to->val, edge.weight);
      weight += edge.weight;
    }
  }
  return weight;
}

// checks to see if the two vertexs on an edge have a cycle to each other
bool Graph::cycle(const map<Vertex *, set<Vertex *>> &origin, Edge edge) const {
  // using dfs to see if any vertex on the path from edge.from
  // on the current mst is connected to edge.to, if so return
  // true
  stack<Vertex *> s;
  set<Vertex *> cycVisited;
  cycVisited.insert(edge.from);
  if (origin.count(edge.from) == 1) {
    for (auto const &i : edge.from->connected) {
      if (origin.at(edge.from).count(i.first) == 1) {
        s.push(i.first);
      }
    }
  }
  while (!s.empty()) {
    Vertex *curr = s.top();
    s.pop();
    if (origin.at(curr).count(edge.to) == 1) {
      return true;
    }
    for (auto const &i : curr->connected) {
      if (origin.at(curr).count(i.first) == 1 &&
          cycVisited.count(i.first) == 0) {
        s.push(i.first);
        cycVisited.insert(i.first);
      }
    }
  }
  return false;
}

// read a text file and create the graph
bool Graph::readFile(const string &filename) {
  ifstream myfile(filename);
  if (!myfile.is_open()) {
    cerr << "Failed to open " << filename << endl;
    return false;
  }
  int edges = 0;
  int weight = 0;
  string fromVertex;
  string toVertex;
  myfile >> edges;
  for (int i = 0; i < edges; ++i) {
    myfile >> fromVertex >> toVertex >> weight;
    connect(fromVertex, toVertex, weight);
  }
  myfile.close();
  return true;
}
