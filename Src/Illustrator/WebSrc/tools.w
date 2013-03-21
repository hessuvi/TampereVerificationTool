%-*-mode: nuweb; nuweb-source-mode: "c++"; -*-
%%% $Source: Src/Illustrator/WebSrc/tools.w $
%%% $Id: tools.w 1.1 Mon, 25 Aug 2003 15:49:29 +0300 hvi $

\chapter{Automatic deallocation of free space}

In this chapter classes \verb<auto_ptr< and \verb<auto_vec< are defined.
These kinds of objects are owners of memery allocated with operator
\verb<new<. If used as value parameters they transfer the ownership
and when last owner dies, memory is deallocated.

The first one is for single objects.
@O auto_ptr.hh -d
@{
#ifndef TOOLS_AUTO_PTR_HH
#define TOOLS_AUTO_PTR_HH

#define explicit

#ifndef HAS_MEMBER_TEMPLATES
#define HAS_MEMBER_TEMPLATES 1
#endif

template<class X>
class auto_ptr
{

  X* px;

public:

  explicit auto_ptr(X* p=0): px(p) {}

#if HAS_MEMBER_TEMPLATES

  template<class Y>
  auto_ptr(auto_ptr<Y>& r) : px(r.release()) {}

  template<class Y>
  auto_ptr& operator=(auto_ptr& r)
  {
    reset(r.release());
    return *this;
  }

#else

  auto_ptr(auto_ptr& r) : px(r.release()) {}

  auto_ptr& operator=(auto_ptr& r)
  {
    reset(r.release());
    return *this;
  }

#endif

  ~auto_ptr()             { delete px; }

  X& operator*()    const { return *px; }
  X* operator->()   const { return px; }
  X* get()          const { return px; }
  X* release()            { X* p=px; px=0; return p; }
  void reset(X* p=0)      { if (px != p) delete px, px = p; }
};
#endif
@| @}

And the other one for C-style vectors, which are handy when used with
old C libraries.
@O auto_vec.hh -d
@{
#ifndef TOOLS_AUTO_VEC_HH
#define TOOLS_AUTO_VEC_HH

#define explicit

#ifndef HAS_MEMBER_TEMPLATES
#define HAS_MEMBER_TEMPLATES 1
#endif

template<class X>
class auto_vec
{

  X* px;

public:

  explicit auto_vec(X* p=0): px(p) {}

#if HAS_MEMBER_TEMPLATES

  template<class Y>
  auto_vec(auto_vec<Y>& r) : px(r.release()) {}

  template<class Y>
  auto_vec& operator=(auto_vec& r)
  {
    reset(r.release());
    return *this;
  }

#else

  auto_vec(auto_vec& r) : px(r.release()) {}

  auto_vec& operator=(auto_vec& r)
  {
    reset(r.release());
    return *this;
  }

#endif

  ~auto_vec()             { delete [] px; }

  X& operator*()    const { return *px; }
  X* operator->()   const { return px; }
  X* get()          const { return px; }
  X* release()            { X* p=px; px=0; return p; }
  void reset(X* p=0)      { if (px != p) delete [] px, px = p; }
  X& operator [] (size_t idx) { return( px[idx]); }
};

#endif

@| @}



\chapter{Adaptor for pointer container iterator}

In many cases it is more convenient to keep ownership of object and
let containers to handler only pointers to objects. However in this
case referencing to object through iterator is ugly and error prone.
This class can be used to cleanup interface of class using pointer
containers in implementation.
@O iteradap.hh
@{
#ifndef TOOLS_ITERADAP_HH
#define TOOLS_ITERADAP_HH

//Usage example:
// typedef IteratorAdaptor<UserObj, list<UserObj *>::iterator > iterator;

template <class OBJ, class ITER>
class IteratorAdaptor: public ITER
{
  typedef ITER Ancestor;
public:
  IteratorAdaptor(const Ancestor &t ): Ancestor(t) {};
  OBJ &operator * () const { return( *( Ancestor::operator * () ) ); };
  OBJ *operator -> () const { return( Ancestor::operator * () ); };
};
#endif
@| @}



\chapter{List of pointers}

@O ptrlist.hh
@{
#ifndef TOOLS_PTRLIST_HH
#define TOOLS_PTRLIST_HH

#ifndef TOOLS_ITERADAP_HH
#include "iteradap.hh"
#endif

template <class OBJ>
class PointerList: public list<OBJ *>
{
  typedef list<OBJ *> Ancestor;

public:
  typedef IteratorAdaptor<OBJ, Ancestor::iterator> iterator;

  iterator begin() {return Ancestor::begin(); };
  iterator end()   {return Ancestor::end(); };}

  iterator insert(iterator pos, OBJ &obj)
    { return Ancestor::insert(pos, &obj) ; };
};

#endif
@| @}



\chapter{Plane geometry}


In this component there is some very low level facilities for
computational geometry in two dimensions.  At least visualization tool
uses these classes to represent graphical objects.

\section{Point}

This is simple 2D point
@o point.hh -d
@{
#ifndef TOOLS_POINT_HH
#define TOOLS_POINT_HH

#ifndef MAKEDEPEND
#include <utility>
using namespace std;
#endif

@<Definition of class Point@>

@<Declaration of operations of Point@>

#endif
@| @}
@o point.cc -d
@{
#include "point.hh"

#ifndef MAKEDEPEND
#include <cmath>
using namespace std;
#endif

@<Implementation of operations of Point@>
@| @}


@D Definition of class Point
@{
class Point : public pair<double,double>
{
  typedef pair<double,double> Ancestor;
public:
  Point():Ancestor() {};
  Point(const Ancestor &p):Ancestor(p) {};
  Point(double x, double y):Ancestor(x,y) {};

  double xCoord() const {return( first );};
  void   xCoord(double x) { first = x; };

  double yCoord() const {return( second );} ;
  void   yCoord(double y) {second = y ;} ;

@<Methods for to scale and move point@>
};
@| @}
2D is vector space and operatios like addition, subtraction, multiply with scaler, inner product and various norms are supported.
@D Declaration of operations of Point
@{
Point
operator + (const Point &u, const Point &v);

Point
operator - (const Point &u, const Point &v);

Point
constProduct(double c, const Point &p);

double
innerProduct(const Point &u, const Point &v);

// Distance between two points measured in infinite norm
double
maxNorm(const Point &u, const Point &v);

// Squared distance between two points measured in Euclidean norm
double
euclNormP2(const Point &u, const Point &v);

// Distance between two point measured in Euclidean norm
double
euclNorm(const Point &u, const Point &v);

// Distance between point and line measured in Euclidean norm
double
euclNorm(const Point &p,
	 const Point &lineBeg,
	 const Point &lineEnd);

double
angleOfVector(const Point &p);
@| @}


@d Methods for to scale and move point
@{
public:
  void move(double distance, const Point &direction);
  void move(const Point &delta)
    {
      first += delta.first;
      second += delta.second;
    };
  void scale( double factor)
    {
      first *= factor;
      second *= factor;
    };
  void scale( double xFactor, double yFactor)
    {
      first *= xFactor;
      second *= yFactor;
    };
@| @}



Implementations of those is not complicated, but take some space.
These implementation do not introduce complicated memory management
problems but might be little slow.
@D Implementation of operations of Point
@{
// Distance between two points measured in infinite norm
double
maxNorm(const Point &u, const Point &v)
{
  double dx=fabs(u.xCoord()-v.xCoord());
  double dy=fabs(u.yCoord()-v.yCoord());

  return( (dy > dx ) ? dy : dx );
}

// Distance between two points measured in Euclidean norm power 2
double
euclNormP2(const Point &u, const Point &v)
{
  double dx=u.xCoord()-v.xCoord();
  double dy=u.yCoord()-v.yCoord();

  return( dx*dx + dy*dy );
}

// Distance between two point measured in Euclidean norm
double
euclNorm(const Point &u, const Point &v)
{
  return( sqrt( euclNormP2(u,v) ) );
}

double
innerProduct(const Point &u, const Point &v)
{
  return( u.xCoord()*v.xCoord() + u.yCoord()*v.yCoord() );
}

Point
constProduct(double c, const Point &p)
{
  return( Point(c*p.xCoord(), c*p.yCoord()) );
}

Point
operator + (const Point &u, const Point &v)
{
  return( Point(u.xCoord()+v.xCoord(), u.yCoord()+v.yCoord()) );
}

Point
operator - (const Point &u, const Point &v)
{
  return( Point(u.xCoord()-v.xCoord(), u.yCoord()-v.yCoord()) );
}

// Distance between point and line measured in Euclidean norm
double
euclNorm(const Point &p,
	 const Point &lineBeg,
	 const Point &lineEnd)
{
  Point a = p-lineBeg;
  Point b = lineEnd-lineBeg;

  double coff = innerProduct(a,b) / innerProduct(b,b) ;

  return( euclNorm( a , constProduct(coff,b) ) );
}

double
angleOfVector(const Point &p)
{
  return( atan2(p.yCoord(),p.xCoord()) );
}

void Point::move(double distance, const Point &direction)
{
  Point vector = direction - *this;
  vector.scale(distance/euclNorm(direction,*this));
  move( vector );
}


@| @}


\subsection{Moving and scaling point}



\section{Line}

Line is simply list of points.
@O line.hh -d
@{
#ifndef TOOLS_LINE_HH
#define TOOLS_LINE_HH

#ifndef TOOLS_POINT_HH
#include "point.hh"
#endif

#ifndef MAKEDEPEND
#include <list>
using namespace std;
#endif

class Line: public list<Point>
{
@<Geometric manipulations for line@>
};

@<Operators for line@>

#endif
@| @}


@D Geometric manipulations for line
@{
public:
void trimBegin(double dist)
  {
    iterator two = begin();
    iterator one = two++;
    (*one).move(dist,*two);
  };
void trimEnd(double dist)
  {
    iterator prev = end();
    prev --;
    iterator last = prev--;
    (*last).move(dist,*prev);
  };
void move(const Point &delta);
void scale(double factor)
  {
    scale(factor,factor);
  };
void scale(double xFactor, double yFactor);
@| @}

@D Operators for line
@{
double
maxNorm(const Line &line);

double
euclNorm(const Line &line);

double
euclNormP2(const Line &line);

double
euclNorm(const Point &p, const Line &line);

bool
linesCross(const Line &a, const Line &b);

bool
linesCross(const Point &a1, const Point &a2,
	   const Point &b1, const Point &b2);

Point
crossingPoint
  (
  const Point &p0,
  const Point &p1,
  const Point &q0,
  const Point &q1
  );

double
crossingFractionInP
  (
  const Point &p0,
  const Point &p1,
  const Point &q0,
  const Point &q1
  );

bool
operator < (const Line &a, const Line &b);
@| @}


@O line.cc -d
@{
#include "line.hh"

void
Line::move(const Point &delta)
{
  for( iterator ii=begin(); ii != end() ; ++ii)
    {
      (*ii).move(delta);
    }
}

void
Line::scale(double xFactor, double yFactor)
{
  for( iterator ii=begin(); ii != end() ; ++ii)
    {
      (*ii).scale(xFactor, yFactor);
    }
}

double
maxNorm(const Line &line)
{
  return( maxNorm(line.front(),line.back()));
}

double
euclNorm(const Line &line)
{
  return( euclNorm(line.front(),line.back()));
}

double
euclNormP2(const Line &line)
{
  return( euclNormP2(line.front(),line.back()));
}

double
euclNorm(const Point &p, const Line &line)
{
  return( euclNorm(p, line.front(),line.back()));
}

static double
detTwo(const Point &p1, const Point &p2)
{
  return( p1.xCoord()*p2.yCoord() - p1.yCoord()*p2.xCoord());
}

static double
detThree(const Point &p1, const Point &p2, const Point &p3)
{
  return( detTwo(p2,p3) - detTwo(p1,p3) + detTwo(p1,p2));
}


bool
linesCross(const Point &a1, const Point &a2,
	   const Point &b1, const Point &b2)
{
  // Close hits to end points are not counted
  double A1 = detThree(a1, a2, b1);
  double A2 = detThree(a1, a2, b2);
  if( (A1<0 && A2 > 0) || (A1 > 0 && A2 < 0 ))
    {
      double B1 = detThree(b1, b2, a1 );
      double B2 = detThree(b1, b2, a2 );
      return(  (B1<0 && B2 > 0) || (B1 > 0 && B2 < 0 ) );
    }
  else
    {
      return( false );
    }
}

bool
linesCross(const Line &a, const Line &b)
{
  // Close hits to end points are not counted
  double A1 = detThree(a.front(), a.back(), b.front());
  double A2 = detThree(a.front(), a.back(), b.back());
  if( (A1<0 && A2 > 0) || (A1 > 0 && A2 < 0 ))
    {
      double B1 = detThree(b.front(), b.back(), a.front() );
      double B2 = detThree(b.front(), b.back(), a.back() );
      return(  (B1<0 && B2 > 0) || (B1 > 0 && B2 < 0 ) );
    }
  else
    {
      return( false );
    }
}
@| @}

Crossing point of too lines given as they endpoints is \texttt{p0 +
  \(\lambda\) (p1-p0)}, where \texttt{p0} and \texttt{p1} are
endpoints of the first line. Needed coefficient \(\lambda\) is get by
solving equation
\[
\texttt{p0} + \lambda (\texttt{p1} - \texttt{p0}) =
\texttt{q0} + \delta (\texttt{q1} - \texttt{q0})
\]
where \texttt{q0} and \texttt{q1} are endpoints of the second line.

Then above equation is simplified we get a equation
\[
  \left[ 
    \begin{array}{cc}
      a & b\\
      c & d
    \end{array}
  \right ]
  \left[ 
    \begin{array}{c}
      \lambda \\
      \delta
    \end{array}
  \right ]
 =
  \left[ 
    \begin{array}{c}
      u \\
      v
    \end{array}
  \right ]
\]
which solution is
\[
  \left[ 
    \begin{array}{c}
      \lambda \\
      \delta
    \end{array}
  \right ]
 =
 \frac{1}{ad-bc}
 \left[ 
    \begin{array}{cc}
      d & -b\\
      -c & a
    \end{array}
  \right ]
  \left[ 
    \begin{array}{c}
      u \\
      v
    \end{array}
  \right]
\]

@O line.cc -d
@{
Point
crossingPoint
  (
  const Point &p0,
  const Point &p1,
  const Point &q0,
  const Point &q1
  )
{
  Point uv = q0 - p0 ;
  Point deltaP = p1 - p0;
  Point deltaQ = q1 - q0;

  double a = deltaP.xCoord();
  double c = deltaP.yCoord();

  double b = deltaQ.xCoord();
  double d = deltaQ.yCoord();

  double u = uv.xCoord();
  double v = uv.yCoord();

  double determinant = a*d - b*c;

  double lambda = (d*u - b*v) / determinant ;

  deltaP.scale(lambda);
  return (p0 + deltaP) ;
}


double
crossingFractionInP
  (
  const Point &p0,
  const Point &p1,
  const Point &q0,
  const Point &q1
  )
{
  Point uv = q0 - p0 ;
  Point deltaP = p1 - p0;
  Point deltaQ = q1 - q0;

  double a = deltaP.xCoord();
  double c = deltaP.yCoord();

  double b = deltaQ.xCoord();
  double d = deltaQ.yCoord();

  double u = uv.xCoord();
  double v = uv.yCoord();

  double determinant = a*d - b*c;

  return (d*u - b*v) / determinant ;
}


static const Point &
min(const Point &a, const Point &b)
{
  if( a < b )
    {
      return a;
    }
  else
    {
      return b;
    }
}

bool
operator < (const Line &a, const Line &b)
{
  Point minA = min(a.front(),a.back());
  Point minB = min(b.front(),b.back());
  return (minA < minB);
}
@| @}




\section{Line drawing algorithm}

This is standard 2-D line drawing algorithm used in some libraries. As
difference to other implementations interface of this one is complient
to STL forward iterator.
@O linealg.hh -d
@{
#ifndef TOOLS_LINEALG_HH
#define TOOLS_LINEALG_HH

#ifndef TOOLS_POINT_HH
#include "point.hh"
#endif

class LineAlgorithm
{
public:
  @<STL forward iterator like interface for linedrawing algorithm@>
private:
  @<Attributes and private methods of LineAlgorithm@>;
};
#endif
@| @}

@D STL forward iterator like interface for linedrawing algorithm
@{
               LineAlgorithm(int x0, int y0, int x1, int y1);
               LineAlgorithm(Point p1, Point p2);
Point          operator * () const;
LineAlgorithm &operator ++ ();
LineAlgorithm  operator ++ (int);
bool           operator == (const LineAlgorithm &o);
bool           operator != (const LineAlgorithm &o)
                 {
		   return( !(*this == o) );
		 }
bool           operator == (const Point &p);
bool           exact() const { return( rem == 0 ); };
bool           horizontalOrVertical() const { return( delta_min == 0 ); };
@| @}


@D Attributes and ...
@{
int cur_x, cur_y;
int rem;
int delta_max, delta_min;
int step_max, step_min;
int *cp_max, *cp_min;

void         init(int x0, int y0, int x1, int y1);
void         step();
@| @}


@O linealg.cc -d
@{
#include "linealg.hh"

#ifndef MAKEDEPEND
#include <cmath>
#include <algorithm>
#include <cassert>
using namespace std;
#endif

Point
LineAlgorithm::operator * () const
{
  return( Point(cur_x,cur_y));
}

LineAlgorithm &
LineAlgorithm::operator ++ ()
{
  step();
  return( *this );
}

LineAlgorithm
LineAlgorithm::operator ++ (int)
{
  LineAlgorithm cur = *this;
  step();
  return( cur );
}

bool
LineAlgorithm::operator == (const LineAlgorithm &o)
{
  return( cur_x == o.cur_x && cur_y == o.cur_y );
}
bool
LineAlgorithm::operator == (const Point &p)
{
  return( p == Point(cur_x,cur_y) );
}


LineAlgorithm::LineAlgorithm(int x0, int y0, int x1, int y1)
{
  init(x0,y0,x1,y1);
}

static inline int oma_rint(double x)
{ return x<0 ? -(int((-x)+.5)) : int(x+.5); }

LineAlgorithm::LineAlgorithm(Point p1, Point p2)
{
  init(static_cast<int>(oma_rint(p1.xCoord())),
       static_cast<int>(oma_rint(p1.yCoord())),
       static_cast<int>(oma_rint(p2.xCoord())),
       static_cast<int>(oma_rint(p2.yCoord())));
  assert( euclNormP2(p1,*(*this)) < 0.01 
	  && "LineAlgorithm: Given starting point is not close to integer" );
}

void
LineAlgorithm::init(int x0, int y0, int x1, int y1)
{
  cur_x = x0;
  cur_y = y0;
  rem = 0;
  delta_max = abs(x1 - x0);
  delta_min = abs(y1 - y0);
  step_max = (x1 < x0) ? -1 : 1;
  step_min = (y1 < y0) ? -1 : 1;
  cp_max = &cur_x;
  cp_min = &cur_y;
  if( delta_max < delta_min )
    {
      swap( delta_max, delta_min );
      swap( step_max, step_min );
      swap( cp_max, cp_min );
    }
}

void
LineAlgorithm::step()
{
  *cp_max += step_max;
  rem += delta_min * 2;
  if( rem > delta_max )
    {
      rem -= delta_max *2 ;
      *cp_min += step_min;
    }
}
@| @}


@D Original linedrawing algorithm (just for example)
@{
void VGA::lineTo(int x1, int y1)
{
  int x,y,lx,ly;
  int dx,dy,e,tmp;
  if( cur_x > x1 ) {
    x=x1;
    lx=cur_x;
    y=y1;
    ly=cur_y;
  } else {
    x=cur_x;
    lx=x1;
    y=cur_y;
    ly=y1;
  }    
  cur_x=x1;
  cur_y=y1;

  dx=lx-x;
  e=0;
  if( y <=ly ) {
    dy= ly-y;
    if( dx>=dy ) {
      while(1) {
        point(x,y);
        if( x==lx ) return;
        x++;
        e+=(dy<<1);
        if( e > dx ) {
          e-=(dx<<1);
          y++;
        }
      }
    } else {
      while(1) {
        point(x,y);
        if( y==ly ) return;
        y++;
        e+=(dx << 1);
        if( e > dy ) {
          e-=(dy << 1 );
          x++;
        }
      }
    }
  } else {
    dy=y-ly;
    if( dx>=dy ) {
      while(1) {
        point(x,y);
        if( x==lx ) return;
        x++;
        e+=(dy<<1);
        if( e > dx ) {
          e-=(dx<<1);
          y--;
        }
      }
    } else {
      while(1) {
        point(x,y);
        if( y==ly ) return;
        y--;
        e+=(dx << 1);
        if( e > dy ) {
          e-=(dy << 1 );
          x++;
        }
      }
    }
  }
}


@| @}


\chapter{Graph}

In this context \verb<Graph< is directed graph and pair \((V,E)\)
where \(V\) is set of \emph{vertices} and \(E\) is set of
\emph{edges}. In additions to that basic structure \verb<Graph<
implements to mappings \(\textrm{Nodecontent:} E \longrightarrow
\textrm{NodeType}\) and \(\textrm{Edgecontent:} V \longrightarrow
\textrm{EdgeType}\).


\subparagraph{Warning!}

This component provides only static graph in sence that once space is
allocated for vertices and edges, it may not enlarged afterward. And
it provides no means to add and delete nodes and only addition of
connections is supported.
\bigskip

This component consists of three template classes \verb<Graph<,
\verb<GraphNode< and \verb<GraphEdge<.
@d Declaration of Graph classes
@{
template <class NodeType, class EdgeType>
class Graph;

template <class NodeType, class EdgeType>
class GraphNode;

template <class NodeType, class EdgeType>
class GraphEdge;
@| @}
Class \verb<GraphNode< is derived from \verb<NodeType< and class
\verb<GraphEdge< from \verb<EdgeType<.

\section{Graph interface}

Graph interface offers very basic set operators for vertices and edges.
@D Interface of class Graph
@{
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
  long	       nofNodes() const;

  EdgeIterator beginEdges();
  EdgeIterator endEdges();
  long	       nofEdges() const;

  ~Graph();

@<Interface of class Graph continued@>
};
@| @}

To keep things simple graph do not offer dynamic creation of vertices
nor edges. This graph can be used only in context there number of
nodes and edges is know beforehand. There is two possible ways to
initialize graph\\
\begin{minipage}[t]{0.4\textwidth}\tiny
\begin{verbatim}
Graph g(nofNodes,nofEdges);
// nodeIndexing(0,nofNodes-1)
// edgeIndexing(0,nofEdges-1)
\end{verbatim}
\end{minipage}
\begin{minipage}[t]{0.4\textwidth}\tiny
\begin{verbatim}
Graph g();
g.nodeIndexing(startIndex,endIndex);
g.edgeIndexing(startIndex,endIndex);
\end{verbatim}
\end{minipage}\\
@d Interface of class Graph continued
@{
  Graph();
  Graph(long nofNodes, long nofEdges);

  void nodeIndexing(long startIndex, long endIndex);
  void edgeIndexing(long startIndex, long endIndex);

  Node &getNode(long index);
  Edge &getEdge(long index);

  Edge &addConnection(Node &u, Node &v,
		      const Edge &e=Edge());
@| @}
When graph is initialized, vertices are initialized with they default
values. Edges are not initialized until added.

Copying of graph is very difficult to implement and it not realy needed.
@d Interface of class Graph continued
@{
private:
  Graph(const Graph &); // Not implemented
  Graph &operator = (const Graph &); // Not implemented
@| @}


\section{Vertex interface}

Interface of vertex offers possibility to iterate trough edges
connected to it. These edges are divided into two sets, entering and
leaving edges. For example if \(e=(u,v)\) then \(e\) is in entering
edges of \(v\) and leaving edges of \(u\).
@D Interface of class GraphNode
@{
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

@<Interface of class GraphNode continued@>
};
@| @}



\section{Edge interface}

Interface of edge offers ability to retrieve nodes it is connected to.
If \(e=(u,v)\) then tail of \(e\) is \(u\) and head is \(v\).
@D Interface of class GraphEdge
@{
template <class NodeType, class EdgeType>
class GraphEdge: public EdgeType
{
  typedef EdgeType Ancestor;
  typedef Graph<NodeType,EdgeType> Container;
  typedef GraphNode<NodeType,EdgeType> Node;
public:
  Node &tail() const;
  Node &head() const;

@<Interface of class GraphEdge continued@>
};
@| @}


\section{Implementation}

@O graph.hh -d
@{
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

@<Declaration of Graph classes@>

@<Interface of class GraphNode@>

@<Interface of class GraphEdge@>

@<Interface of class Graph@>

#endif // TOOLS_GRAPH_HH

@| @}

@O graph.cc -d
@{
#include "graph.hh"

@<Implementation of class GraphNode@>

@<Implementation of class GraphEdge@>

@<Implementation of class Graph@>
@| @}


\subsection{Graph implementation}

Graph has following datamembers.
@d Interface of class Graph continued
@{
private:
  VertexSet d_vertices;
  EdgeSet   d_edges;
  long      d_nodeStartIndex;
  long      d_edgeStartIndex;
@| @}

@D Implementation of class Graph
@{
template <class NodeType, class EdgeType> 
Graph<NodeType,EdgeType>::~Graph()
{
  // All cleanups are made by destructors of members
}
@| @}

In first version of initialization sequence the sizes of vertex and edge sets are given as constructor parameters
@D Implementation of class Graph
@{
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
@| @}

And in second version sizes of sets are given afterward using indexing
commands.
@D Implementation of class Graph
@{
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
}@| @}

Method \verb<addConnection< introduces a new edge into graph. All
edges have to be inserted this way. Note that methods in interface of node and edge which build connections are private to those classes. Class \verb<Graph< is friend of those.
@D Implementation of class Graph
@{
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
}@| @}

Node and edge retrieval are just for array boundary checking.
@D Implementation of class Graph
@{
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
@}

More convenient way of geting nodes and edges are iterators. In this
version of implementation used containers are vectors and iterators
are random access iterators, so it is possible to bypass array
boundary checking (may change in future)
@D Implementation of class Graph
@{

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
@| @}


\subsection{GraphNode implementation}

@D Interface of class GraphNode continued
@{
private:
  AdjacencyList d_enterting;
  AdjacencyList d_leaving;

public:
  GraphNode();
  GraphNode(const GraphNode &);
  GraphNode & operator = (const GraphNode &);
  ~GraphNode() {};

private:
  void addEntering(Edge &);
  void addLeaving(Edge &);

  friend class Container;
@| @}

@D Implementation of class GraphNode
@{
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
@| @}

As user point of view \verb<GraphNode< behaves as ancestor
\verb<NodeType< and has needed constructors and assignment operator.
Connection information is not copied but removed.
@D Implementation of class GraphNode
@{
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
@| @}

@D Implementation of class GraphNode
@{
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
@| @}



\subsection{GraphEdge implementation}

@D Interface of class GraphEdge continued
@{
private:
  Node *d_head;
  Node *d_tail;
public:
  GraphEdge();
  GraphEdge(const GraphEdge &);
  GraphEdge &operator = (const GraphEdge &);
  ~GraphEdge() {};
private:
  void head(Node &);
  void tail(Node &);

  friend class Container;
@| @}



@D Implementation of class GraphEdge
@{
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
@| @}

@D Implementation of class GraphEdge
@{
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
@| @}

@D Implementation of class GraphEdge
@{
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
@| @}




\chapter{Random numbers}

Since there is differences in random number generation between
different systems, single interface is provided here for more easy porting.
@O random.hh
@{
#ifndef TOOLS_RANDOM_HH
#define TOOLS_RANDOM_HH

class Random
{
  static const unsigned int RANDOM_SEED = 0xab53ae75;
public:
  static double normal();
  static double generate(double maximum, double minimum=0.0);
  static int    generate(int    maximum, int    minimum=0);
  static void   reseed( unsigned int newSeed = RANDOM_SEED );
  ~Random();
private:
  Random();
  int  value();
  int  maxValue();
  static Random &instance();
};
#endif
@| Random::generate@}
User interface is simply \verb<Random::generate< or
\verb<Random::normal< and no prior initialization are not needed,
because implementation take care of them. \verb<Random::normal< returns
numbers from range \([0,1)\).

@O random.cc -d
@{
#include "random.hh"
#include <stdlib.h>

double
Random::normal()
{
  Random &obj = instance();
  return( obj.value() / ( 1.0 + obj.maxValue() ) );
}

double
Random::generate(double maximum, double minimum)
{
  return( minimum + normal() * (maximum - minimum) );
}

int
Random::generate(int maximum, int minimum)
{
  int delta = maximum - minimum ;
  Random &obj = instance();

  int limit = (obj.maxValue() / delta ) * delta ;
  int v;
  for( v = obj.value() ; v > limit ; v = obj.value() )
    ;
  return( minimum + v % delta );
}

void
Random::reseed( unsigned int newSeed )
{
  instance();
  srand( newSeed );
}

Random::~Random()
{
}

Random::Random()
{
  srand( RANDOM_SEED );
}

Random &
Random::instance()
{
  static Random onlyObject;
  return( onlyObject );
}

int
Random::value()
{
  return( rand() );
}

int
Random::maxValue()
{
  return( RAND_MAX );
}
@}





\section{Test programs}


\section{Test program for line drawing algorithm}

@O linetest.cc -d
@{
#include <iostream.h>
#include "linealg.hh"

int
main()
{
  LineAlgorithm hor_beginnig(10,10,2,10);
  LineAlgorithm hor_end(2,10,2,10);
  for( ; hor_beginnig != hor_end ; ++ hor_beginnig)
    {
      Point cur = *hor_beginnig;
      cout << cur.xCoord() << "," << cur.yCoord() << endl;
    }

  LineAlgorithm ver_beginnig(10,10,10,20);
  LineAlgorithm ver_end(10,20,10,20);
  for( ; ver_beginnig != ver_end ; ++ ver_beginnig)
    {
      Point cur = *ver_beginnig;
      cout << cur.xCoord() << "," << cur.yCoord() << endl;
    }

  cout << endl;
  {
    LineAlgorithm diag_beginnig(10,10,7,6);
    LineAlgorithm diag_end(7,6,7,6);
    for( ; diag_beginnig != diag_end ; ++ diag_beginnig)
      {
	Point cur = *diag_beginnig;
	cout << cur.xCoord() << "," << cur.yCoord() << endl;
      }
    cout << endl;
  }
  {
    LineAlgorithm diag_beginnig(0,0,3,4);
    LineAlgorithm diag_end(3,4,3,4);
    for( ; diag_beginnig != diag_end ; ++ diag_beginnig)
      {
	Point cur = *diag_beginnig;
	cout << cur.xCoord() << "," << cur.yCoord() << endl;
      }
    cout << endl;
  }

}
@| @}


%%% Local variables:
%%% mode: outline-minor
%%% End:
