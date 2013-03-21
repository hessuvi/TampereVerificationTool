/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is
TVT-tools.

Copyright © 2001 Nokia and others. All Rights Reserved.

Contributor(s): Heikki Virtanen.
*/



#ifndef TOOLS_GRAPH_HH
#define TOOLS_GRAPH_HH

#ifndef MAKEDEPEND
#include <list>
#include <vector>
using namespace std;
#endif

#ifndef TOOLS_ITERADAP_HH
#include "iteradap.hh"
#endif



template <class NodeType, class EdgeType>
class Graph;

template <class NodeType, class EdgeType>
class GraphNode;

template <class NodeType, class EdgeType>
class GraphEdge;





template <class NodeType, class EdgeType>
class GraphNode: public NodeType
{
  typedef NodeType Ancestor;
  typedef Graph<NodeType,EdgeType> Container;
  typedef GraphEdge<NodeType,EdgeType> Edge;
public:
  typedef list<Edge *>  AdjacencyList;
  typedef IteratorAdaptor<Edge, typename AdjacencyList::iterator> AdjacencyIterator;

  AdjacencyIterator beginEntering();
  AdjacencyIterator endEntering();

  AdjacencyIterator beginLeaving();
  AdjacencyIterator endLeaving();



private:
  AdjacencyList d_enterting;
  AdjacencyList d_leaving;

public:
  GraphNode();
  GraphNode(const GraphNode &);
  GraphNode & operator = (const GraphNode &);
  ~GraphNode() {};

    //private:
  void addEntering(Edge &);
  void addLeaving(Edge &);

    //  friend class Container;


};





template <class NodeType, class EdgeType>
class GraphEdge: public EdgeType
{
  typedef EdgeType Ancestor;
  typedef Graph<NodeType,EdgeType> Container;
  typedef GraphNode<NodeType,EdgeType> Node;
public:
  Node &tail() const;
  Node &head() const;



private:
  Node *d_head;
  Node *d_tail;
public:
  GraphEdge();
  GraphEdge(const GraphEdge &);
  GraphEdge &operator = (const GraphEdge &);
  ~GraphEdge() {};
    //private:
  void head(Node &);
  void tail(Node &);

    //  friend class Container;


};





template <class NodeType, class EdgeType>
class Graph
{
public:
  typedef GraphNode<NodeType, EdgeType> Node;
  typedef GraphEdge<NodeType, EdgeType> Edge;
private: 
  typedef vector<Node> VertexSet;
  typedef vector<Edge> EdgeSet;
public:
  typedef typename VertexSet::iterator NodeIterator;
  typedef typename EdgeSet::iterator   EdgeIterator;
public:
  NodeIterator beginNodes();
  NodeIterator endNodes();
  long         nofNodes() const;

  EdgeIterator beginEdges();
  EdgeIterator endEdges();
  long         nofEdges() const;

  ~Graph();



  Graph();
  Graph(long nofNodes, long nofEdges);

  void nodeIndexing(long startIndex, long endIndex);
  void edgeIndexing(long startIndex, long endIndex);

  Node &getNode(long index);
  Edge &getEdge(long index);

  Edge &addConnection(Node &u, Node &v,
                      const Edge &e=Edge());


private:
  Graph(const Graph &); // Not implemented
  Graph &operator = (const Graph &); // Not implemented


private:
  VertexSet d_vertices;
  EdgeSet   d_edges;
  long      d_nodeStartIndex;
  long      d_edgeStartIndex;


};



#endif // TOOLS_GRAPH_HH
