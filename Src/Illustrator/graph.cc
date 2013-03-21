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



#include "graph.hh"



template <class NodeType, class EdgeType>
typename GraphNode<NodeType,EdgeType>::AdjacencyIterator
GraphNode<NodeType,EdgeType>::beginEntering()
{
  return( d_enterting.begin() );
}

template <class NodeType, class EdgeType>
typename GraphNode<NodeType,EdgeType>::AdjacencyIterator
GraphNode<NodeType,EdgeType>::endEntering()
{
  return( d_enterting.end() );
}

template <class NodeType, class EdgeType>
typename GraphNode<NodeType,EdgeType>::AdjacencyIterator
GraphNode<NodeType,EdgeType>::beginLeaving()
{
  return( d_leaving.begin() );
}

template <class NodeType, class EdgeType>
typename GraphNode<NodeType,EdgeType>::AdjacencyIterator
GraphNode<NodeType,EdgeType>::endLeaving()
{
  return( d_leaving.end() );
}


template <class NodeType, class EdgeType>
GraphNode<NodeType,EdgeType>::GraphNode()
  : Ancestor()
  , d_enterting()
  , d_leaving()
{
}

template <class NodeType, class EdgeType>
GraphNode<NodeType,EdgeType>::GraphNode(const GraphNode &n)
  : Ancestor(n)
  , d_enterting()
  , d_leaving()
{
}

template <class NodeType, class EdgeType>
GraphNode<NodeType,EdgeType> &
GraphNode<NodeType,EdgeType>:: operator = ( const GraphNode &n)
{
  if( &n == this )
    {
      return( *this );
    }
  Ancestor::operator = ( n );
  d_enterting.erase( d_enterting.begin(), d_enterting.end() );
  d_leaving.erase( d_leaving.begin(), d_leaving.end() );
  return( *this );
}


template <class NodeType, class EdgeType>
void
GraphNode<NodeType,EdgeType>::addEntering(Edge &e)
{
  d_enterting.push_back(&e);
}

template <class NodeType, class EdgeType>
void
GraphNode<NodeType,EdgeType>::addLeaving(Edge &e)
{
  d_leaving.push_back(&e);
}





#include <cassert>

template <class NodeType, class EdgeType>
GraphNode<NodeType,EdgeType> &
GraphEdge<NodeType,EdgeType>::tail() const
{
  assert( d_tail );
  return( *d_tail );
}

template <class NodeType, class EdgeType>
GraphNode<NodeType,EdgeType> &
GraphEdge<NodeType,EdgeType>::head() const
{
  assert( d_head );
  return( *d_head );
}


template <class NodeType, class EdgeType>
GraphEdge<NodeType,EdgeType>::GraphEdge()
  : Ancestor()
  , d_head(0)
  , d_tail(0)
{
}

template <class NodeType, class EdgeType>
GraphEdge<NodeType,EdgeType>::GraphEdge(const GraphEdge &e)
  : Ancestor(e)
  , d_head(0)
  , d_tail(0)
{
}

template <class NodeType, class EdgeType>
GraphEdge<NodeType,EdgeType> &
GraphEdge<NodeType,EdgeType>:: operator = (const GraphEdge &e)
{
  if ( &e == this )
    {
      return( *this );
    }
  Ancestor::operator = ( e );
  d_head = 0;
  d_tail = 0;

  return( *this );
}


#include <cassert>

template <class NodeType, class EdgeType>
void
GraphEdge<NodeType,EdgeType>::head(Node &n)
{
  assert( ! d_head );
  d_head = &n;
}

template <class NodeType, class EdgeType>
void
GraphEdge<NodeType,EdgeType>::tail(Node &n)
{
  assert( ! d_tail );
  d_tail = &n;
}





template <class NodeType, class EdgeType> 
Graph<NodeType,EdgeType>::~Graph()
{
  // All cleanups are made by destructors of members
}


template <class NodeType, class EdgeType>
Graph<NodeType,EdgeType>::Graph(long nofNodes, long nofEdges)
  :d_vertices(nofNodes)
  ,d_edges()
  ,d_nodeStartIndex(0)
  ,d_edgeStartIndex(0)
{
  // Constructor of vector inserts that much elements to vector
  // and edges do not exist yet.
  // Space is still needed bacause we must prevent relocation of
  // vector content.
  d_edges.reserve(nofEdges);
}


template <class NodeType, class EdgeType>
Graph<NodeType,EdgeType>::Graph()
{
}

#include <cassert>

template <class NodeType, class EdgeType>
void
Graph<NodeType,EdgeType>::nodeIndexing(long startIndex, long endIndex)
{
  assert(d_vertices.size() <= 0 );
  long nofNodes = endIndex-startIndex+1;
  Node dummy;
  d_vertices.insert(d_vertices.end(),nofNodes,dummy);
  d_nodeStartIndex = startIndex;
}

template <class NodeType, class EdgeType>
void
Graph<NodeType,EdgeType>::edgeIndexing(long startIndex, long endIndex)
{
  assert(d_edges.capacity() <= 0 );
  long nofEdges = endIndex-startIndex+1;
  d_edges.reserve(nofEdges);
  d_edgeStartIndex = startIndex;
}

template <class NodeType, class EdgeType>
GraphEdge<NodeType,EdgeType> &
Graph<NodeType,EdgeType>::addConnection(Node &u, Node &v,
                                        const Edge &e)
{
  assert( d_edges.size() < d_edges.capacity() );

  Edge &ins = *(d_edges.insert(d_edges.end(),e));
  u.addLeaving(ins);
  v.addEntering(ins);
  ins.tail(u);
  ins.head(v);
  return( ins );
}

template <class NodeType, class EdgeType>
typename Graph<NodeType,EdgeType>::Node &
Graph<NodeType,EdgeType>::getNode(long index)
{
  assert( (index >= d_nodeStartIndex)
          && ( static_cast<size_t>(index)
               < d_nodeStartIndex + d_vertices.size()) );

  return( d_vertices[index - d_nodeStartIndex] );
}

template <class NodeType, class EdgeType>
typename Graph<NodeType,EdgeType>::Edge &
Graph<NodeType,EdgeType>::getEdge(long index)
{
  assert( (index >= d_edgeStartIndex)
          && (static_cast<size_t>(index) < d_edgeStartIndex + d_edges.size()) );
  return( d_edges[index - d_edgeStartIndex]);
}



template <class NodeType, class EdgeType>
typename Graph<NodeType,EdgeType>::NodeIterator
Graph<NodeType,EdgeType>::beginNodes()
{
  return( d_vertices.begin() );
}

template <class NodeType, class EdgeType>
typename Graph<NodeType,EdgeType>::NodeIterator
Graph<NodeType,EdgeType>::endNodes()
{
  return( d_vertices.end() );
}

template <class NodeType, class EdgeType>
long
Graph<NodeType,EdgeType>::nofNodes() const
{
  return( d_vertices.size() );
}

template <class NodeType, class EdgeType>
typename Graph<NodeType,EdgeType>::EdgeIterator
Graph<NodeType,EdgeType>::beginEdges()
{
  return( d_edges.begin());
}

template <class NodeType, class EdgeType>
typename Graph<NodeType,EdgeType>::EdgeIterator
Graph<NodeType,EdgeType>::endEdges()
{
  return( d_edges.end());
}

template <class NodeType, class EdgeType>
long
Graph<NodeType,EdgeType>::nofEdges() const
{
  return( d_edges.size());
}


