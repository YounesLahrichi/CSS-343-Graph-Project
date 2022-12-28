/**
 * Testing BST - Binary Search Tree functions
 *
 * @author Yusuf Pisan
 * @date 19 Oct 2019
 */

#include "graph.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// global value for testing
// NOLINTNEXTLINE
stringstream globalSS;

// need to reset SS before calling this
void vertexPrinter(const string &s) { globalSS << s; }

// need to reset SS before calling this
void edgePrinter(const string &from, const string &to, int weight) {
  globalSS << "[" << from << to << " " << weight << "]";
}

// convert a map to a string so we can compare it
template <typename K, typename L>
static string map2string(const map<K, L> &mp) {
  stringstream out;
  for (auto &p : mp) {
    out << "[" << p.first << ":" << p.second << "]";
  }
  return out.str();
}

// tests add, connect, disconnect, verticesSize,
// edgeSize, vertexDegree, getEdgesAsString, and contains
void testGraphBasic() {
  Graph g;
  assert(g.add("a") && "add vertex a");
  assert(g.add("b") && "add vertex b");
  assert(g.add("c") && "add vertex c");
  assert(g.add("d") && "add vertex d");
  assert(g.add("e") && "add vertex e");
  assert(!g.add("b") && "b added twice");
  assert(g.connect("a", "b", 10) && "connect a b");
  assert(!g.connect("a", "b", 50) && "duplicate connect a b");
  assert(!g.connect("a", "a", 1) && "connect a to itself");
  g.connect("a", "d", 40);
  g.connect("a", "c", 20);
  assert((g.verticesSize() == 5) && "graph number of vertices");
  assert((g.edgesSize() == 3) && "graph number of edges");
  assert((g.vertexDegree("a") == 3) && "vertex number of edges");
  assert((g.vertexDegree("c") == 0) && "no outgoing edges c");
  assert((g.vertexDegree("xxx") == -1) && "no edges for xxx");
  assert(!g.contains("xxx") && "xxx not in graph");
  assert(g.contains("a") && "a in graph");

  // check that they are sorted based on edge end label
  assert(g.getEdgesAsString("a") == "b(10),c(20),d(40)");
  // disconnect non-existent edge/vertex
  assert(!g.disconnect("a", "e") && "disconnecting non-existent vertex");
  assert((g.edgesSize() == 3) && "disconnected nonexisting");
  assert(g.disconnect("a", "c") && "a-c disconnect");
  assert((g.edgesSize() == 2) && "number of edges after disconnect");
  assert((g.vertexDegree("a") == 2) && "a has 2 edges");
  assert(g.getEdgesAsString("a") == "b(10),d(40)" && "removing middle edge");
}

// tests dfs algo
void testGraph0DFS() {
  cout << "testGraph0DFS" << endl;
  Graph g;
  if (!g.readFile("graph0.txt")) {
    return;
  }
  assert(g.contains("A") && "a in graph");
  assert(g.contains("B") && "b in graph");
  assert(g.contains("C") && "c in graph");
  assert(g.getEdgesAsString("A") == "B(1),C(8)");
  assert(g.getEdgesAsString("B") == "C(3)");
  assert(g.getEdgesAsString("C").empty());

  g.dfs("A", vertexPrinter);
  assert(globalSS.str() == "ABC" && "starting from A");

  globalSS.str("");
  g.dfs("B", vertexPrinter);
  assert(globalSS.str() == "BC" && "starting from B");

  globalSS.str("");
  g.dfs("C", vertexPrinter);
  assert(globalSS.str() == "C" && "starting from C");

  globalSS.str("");
  g.dfs("X", vertexPrinter);
  assert(globalSS.str().empty() && "starting from X");
}

// tests bfs algo
void testGraph0BFS() {
  cout << "testGraph0BFS" << endl;
  Graph g;
  if (!g.readFile("graph0.txt")) {
    return;
  }

  globalSS.str("");
  g.bfs("A", vertexPrinter);
  assert(globalSS.str() == "ABC" && "starting from A");

  globalSS.str("");
  g.dfs("B", vertexPrinter);
  assert(globalSS.str() == "BC" && "starting from B");

  globalSS.str("");
  g.dfs("C", vertexPrinter);
  assert(globalSS.str() == "C" && "starting from C");

  globalSS.str("");
  g.dfs("X", vertexPrinter);
  assert(globalSS.str().empty() && "starting from X");
}

// tests Dijkstra's algo
void testGraph0Dijkstra() {
  cout << "testGraph0Dijkstra" << endl;
  Graph g;
  if (!g.readFile("graph0.txt")) {
    return;
  }
  map<string, int> weights;
  map<string, string> previous;
  tie(weights, previous) = g.dijkstra("A");
  // cout << "Dijkstra(A) weights is " << map2string(weights) << endl;
  assert(map2string(weights) == "[B:1][C:4]" && "Dijkstra(A) weights");
  // cout << "Dijkstra(A) previous is " << map2string(previous) << endl;
  assert(map2string(previous) == "[B:A][C:B]" && "Dijkstra(A) previous");

  tie(weights, previous) = g.dijkstra("B");
  assert(map2string(weights) == "[C:3]" && "Dijkstra(B) weights");
  assert(map2string(previous) == "[C:B]" && "Dijkstra(B) previous");

  tie(weights, previous) = g.dijkstra("X");
  assert(map2string(weights).empty() && "Dijkstra(C) weights");
  assert(map2string(previous).empty() && "Dijkstra(C) previous");
}

// tests graph functionalities with an undirected graph
void testGraph0NotDirected() {
  cout << "testGraph0NotDirected" << endl;
  bool isDirectional = false;
  Graph g(isDirectional);
  if (!g.readFile("graph0.txt")) {
    return;
  }

  globalSS.str("");
  g.bfs("A", vertexPrinter);
  assert(globalSS.str() == "ABC" && "starting from A");

  globalSS.str("");
  g.dfs("B", vertexPrinter);
  assert(globalSS.str() == "BAC" && "starting from B");

  globalSS.str("");
  g.dfs("C", vertexPrinter);
  assert(globalSS.str() == "CAB" && "starting from C");

  globalSS.str("");
  g.dfs("X", vertexPrinter);
  assert(globalSS.str().empty() && "starting from X");

  map<string, int> weights;
  map<string, string> previous;
  tie(weights, previous) = g.dijkstra("A");
  // cout << "Dijkstra(A) weights is " << map2string(weights) << endl;
  assert(map2string(weights) == "[B:1][C:4]" && "Dijkstra(A) weights");
  // cout << "Dijkstra(A) previous is " << map2string(previous) << endl;
  assert(map2string(previous) == "[B:A][C:B]" && "Dijkstra(A) previous");

  tie(weights, previous) = g.dijkstra("B");
  assert(map2string(weights) == "[A:1][C:3]" && "Dijkstra(B) weights");
  assert(map2string(previous) == "[A:B][C:B]" && "Dijkstra(B) previous");

  tie(weights, previous) = g.dijkstra("X");
  assert(map2string(weights).empty() && "Dijkstra(C) weights");
  assert(map2string(previous).empty() && "Dijkstra(C) previous");

  globalSS.str("");
  int mstLength = g.mstPrim("A", edgePrinter);
  assert(mstLength == 4 && "mst A is 4");
  assert(globalSS.str() == "[AB 1][BC 3]" && "mst A is [AB 1][BC 3]");

  globalSS.str("");
  mstLength = g.mstPrim("B", edgePrinter);
  assert(mstLength == 4 && "mst 4 is 4");
  assert(globalSS.str() == "[BA 1][BC 3]");

  globalSS.str("");
  mstLength = g.mstPrim("C", edgePrinter);
  assert(mstLength == 4 && "mst C is 4");
  assert(globalSS.str() == "[CB 3][BA 1]");

  globalSS.str("");
  mstLength = g.mstPrim("X", edgePrinter);
  assert(mstLength == -1 && "mst X is -1");
  assert(globalSS.str().empty() && "mst for vertex not found");
}

// tests graph functionalities with graph1.txt
void testGraph1() {
  cout << "testGraph1" << endl;
  Graph g;
  if (!g.readFile("graph1.txt")) {
    return;
  }
  globalSS.str("");
  g.dfs("A", vertexPrinter);
  assert(globalSS.str() == "ABCDEFGH" && "dfs starting from A");

  globalSS.str("");
  g.bfs("A", vertexPrinter);
  assert(globalSS.str() == "ABHCGDEF" && "bfs starting from A");

  globalSS.str("");
  g.dfs("B", vertexPrinter);
  assert(globalSS.str() == "BCDEFG" && "dfs starting from B");

  globalSS.str("");
  g.bfs("B", vertexPrinter);
  assert(globalSS.str() == "BCDEFG" && "bfs starting from B");

  map<string, int> weights;
  map<string, string> previous;
  auto p = g.dijkstra("A");
  weights = p.first;
  previous = p.second;
  assert(map2string(weights) == "[B:1][C:2][D:3][E:4][F:5][G:4][H:3]" &&
         "Dijkstra(B) weights");
  assert(map2string(previous) == "[B:A][C:B][D:C][E:D][F:E][G:H][H:A]" &&
         "Dijkstra(B) previous");
}

// tests graph functionalities with directed graph4.txt
void testGraph4Directed() {
  cout << "testGraph4Directed" << endl;
  Graph g;
  if (!g.readFile("graph4.txt")) {
    return;
  }
  globalSS.str("");
  g.dfs("A", vertexPrinter);
  assert(globalSS.str() == "ABDKEGIJHLF" && "dfs starting from A");

  globalSS.str("");
  g.bfs("A", vertexPrinter);
  assert(globalSS.str() == "ABEFHDGIJLK" && "bfs starting from A");

  map<string, int> weights;
  map<string, string> previous;
  auto p = g.dijkstra("A");
  weights = p.first;
  previous = p.second;
  assert(map2string(weights) ==
             "[B:6][D:15][E:2][F:2][G:3][H:8][I:3][J:5][K:4][L:11]" &&
         "Dijkstra(A) weights");
  assert(map2string(previous) ==
             "[B:A][D:B][E:A][F:A][G:E][H:A][I:E][J:E][K:I][L:H]" &&
         "Dijkstra(A) previous");
}

// tests graph functionalities with Undirected graph4.txt
void testGraph4Undirected() {
  cout << "testGraph4Undirected" << endl;
  Graph g(false);
  if (!g.readFile("graph4.txt")) {
    return;
  }
  assert(g.edgesSize() == 17 && "undirected edge count");

  globalSS.str("");
  g.dfs("A", vertexPrinter);
  assert(globalSS.str() == "ABDKCLHIEGFJ" && "dfs starting from A");

  globalSS.str("");
  g.bfs("A", vertexPrinter);
  assert(globalSS.str() == "ABEFHDGIJLKC" && "bfs starting from A");

  int mstLength = g.mstPrim("A", edgePrinter);
  assert(mstLength == 22 && "mst A is 22");

  mstLength = g.mstPrim("C", edgePrinter);
  assert(mstLength == 22 && "mst C is 22");

  mstLength = g.mstKruskal("A", edgePrinter);
  assert(mstLength == 22 && "mst A is 22");

  mstLength = g.mstKruskal("C", edgePrinter);
  assert(mstLength == 22 && "mst C is 22");
}

// runs all test methods
void testAll() {
  testGraphBasic();
  testGraph0DFS();
  testGraph0BFS();
  testGraph0Dijkstra();
  testGraph0NotDirected();
  testGraph4Directed();
  testGraph4Undirected();
  testGraph1();
}