%%% $Source: Src/Illustrator/WebSrc/algor.w $
%%% $Id: algor.w 1.1 Mon, 25 Aug 2003 15:49:29 +0300 hvi $

\part{LTS drawing algorithm}


\chapter{Problem}

Overal problem is to draw labelled transition system automaticly. In
this chapter problem is described detailed and solved in following chapters.


\section{Labelled transition system}

LTS is 4-tuble \((\stateset,\Sigma,\Delta,\istate)\), where
\(\stateset\) is set of \emph{states}, \(\Sigma\) is set of
\emph{actions} or \emph{alphabet}, \(\Delta\) is set of
\emph{transitions} and
\(\Delta \subset (\stateset \times \Sigma \times \stateset) \).
And \istate{} is \emph{initial state} and \(\istate\in\stateset\).


In this context the invisible action \(\tau\) do not have to be treated
specially. In theory it has special meaning since it is not a part of
observable behaviour and it does not syncronice. In theory \(\tau\) is
never part of alphabet but it is allways present. In this program
\(\tau\) is treated as any other action in alphabet.

Labelled transition system is very much same as finite automaton
except there is no accepting states in LTS. On the other hand LTS has
similar structure as ordinary directed graph where states are treated
as vertices and transitions as edges.


In this program main datastructure represent LTS combined with
extended information described in \cite{ALA:ltsformat} and information
needed for visualization.

Information and interface of LTS are divided to tree classes, which
are \verb<IlLTS<, \verb<IlLTS::State< and \verb<IlLTS::Transition<.
These classes uses \verb<Graph< class as base \cite{ALA:prog:tools}.
Classes needed for base for graph are
@d Types in LTS container component
@{
class IlStateBase;
class IlTransitionBase;
class IlLTS;
@| @}

Main class is \verb<IlLTS< which defines types for actual states and
transitions and other container specific stuff.
@D Labelled transition system class
@{
class IlLTS :public iTransitionsAP,
             public iActionNamesAP,
             private Graph<IlStateBase,IlTransitionBase>
{
  typedef Graph<IlStateBase,IlTransitionBase> Ancestor;
public:
  typedef Ancestor::Node State;
  typedef Ancestor::Edge Transition;

  typedef Ancestor::NodeIterator StateIterator;
  typedef Ancestor::EdgeIterator TransitionIterator;

  StateIterator beginStates();
  StateIterator endStates();
  long		nofStates() const;

  unsigned getStateNumber(StateIterator stIter)
  { return stIter-beginStates()+1; }

  TransitionIterator beginTransitions();
  TransitionIterator endTransitions();
  long		     nofTransitions() const;

  const StatePropsContainer& getStatePropsContainer() { return spcontainer; }

  bool initializeFromFile( InStream&, const string& comment );

  IlLTS();
  ~IlLTS();

@<IlLTS class definition continued@>

};
@| @}
What is above, it is derived directly from interface of graph and
almost entire public interface of labelled transitions system class.
Only missing piece is interface for managing alphabet.  Notice that
LTS can be initialized only from file using method
\verb<initializeFromFile<

As from definition of class \verb<IlLTS< can be seen generic graph
class is used in implementation. There is no real need to hide
interface of graph in this point, but graph has some limitations and
it is safer this way \cite{ALA:prog:tools}.


\subsection{States and transitions}

In the other hand public interfaces of vertex and edge classes of
graph is used as part of public interface of state and transition
classes. As quick summary, node class offers iterators for adjacent
edges as \verb<beginEntering()<, \verb<endEntering()<,
\verb<beginLeaving()<, \verb<endLeaving()<, and edge class offers
methods \verb<head()< and \verb<tail()<.  See \cite{ALA:prog:tools}
for more information of using them.

Additional information needed for state contains name of state and
some flags describing what kind of state it is.
@D Types for state and transition
@{
class IlStateBase
{
public:
  enum StateFlag
  {
    INITIAL_STATE	= 1,
    REJECTION_FLAG 	= 2,
    DEADLOCK_FLAG	= 4,
    LIVELOCK_FLAG	= 8,
    HASTRANSITIONS_FLAG = 16,
    CUTSTATE_FLAG       = 32
  };
  void addFlag(StateFlag f) { d_flags |= f; };
  int  flags() const { return d_flags ; };
  void clearFlags() {d_flags = 0 ;};

private:
  int  d_flags;

public:
  static const state_t UNDEFINED_NAME = 0;
  state_t name() const { return d_name; };
  void    name(state_t n) { d_name = n; };

  bool operator == (const IlStateBase & o) const 
     { return (d_name == o.d_name); };
  bool operator < (const IlStateBase & o) const 
     { return (d_name < o.d_name); };

private:
  state_t d_name;

public:
  IlStateBase():d_flags(0),d_name(UNDEFINED_NAME) {};
  ~IlStateBase() {};
@<State class definition continued@>
};
@| @}
As defined in \cite{ALA:ltsformat} names of states are integers from 1
to some \(N\). That is why 0 can be used as illegal state name.

Only additional information besides in edge of graph needed for
transitions is action.  Type \verb<action_t< is defined in headerfile
\verb<LTScommon.h< in LTS io library and it is \verb<int<.
@D Types for state and transition
@{
class IlTransitionBase
{
public:
  IlTransitionBase():d_action(~1) {};
  ~IlTransitionBase() {};

  action_t action() const {return d_action; };
  void action(action_t a) {d_action = a ;};
private:
  action_t d_action;
@<Transition class definition continued@>
};
@| @}
Definition of state and transition classes are continued later. See
section \ref{sec:model}


\subsection{Alphabet}

Alphabet \(\Sigma\) is set of strings as described in
\cite{ALA:ltsformat}. In LTS actions are represented as numbers.
Natural data structure for actions is map from integral type
\verb<action_t< to \verb<string<.
@d IlLTS class definition continued
@{
private:
  typedef map<action_t,string> Alphabet;
  Alphabet d_alphabet;
  StatePropsContainer spcontainer;
@| @}
Interface of LTS offers method \verb<nameOfAction< and iterators into
datastructure. Iterator to alphabet is \textsf{STL} \verb<pair<, of
which first item is action number and second item action name.
@d IlLTS class definition continued
@{
public:
  const string &nameOfAction(action_t action);
  typedef Alphabet::iterator ActionIterator;
  ActionIterator beginAlphabet();
  ActionIterator endAlphabet();
@| @}
Action \(\tau\) (\verb<tau<) has always number \(0\) and is always
present. Normally it is not part of alphabet, but in this context it
has no special meaning.

\subsection{Implementation}

After this point there is no such interface declarations of labelled
transition system interface that efect user in any way. Implementation
can be skipped by continuing from section \ref{sec:model} on page
\pageref{sec:model}.

LTS as data structure forms component \verb<lts< which files are
@d lts.hh
@{
typedef lsts_index_t state_t;
typedef lsts_index_t action_t;
@<Types in LTS container component@>
@<Types for state and transition@>
@<Labelled transition system class@>

inline
bool
operator == (const IlLTS::Transition &t1,
	     const IlLTS::Transition &t2)
{
  return( &t1 == &t2 );
}
@| @}
and
@d lts.cc
@{
@<Implementation of methods of LTS container@>
@| @}

Iterator retrievals are just wrappers to corresponding methods of
\verb<Graph<.
@D Implementation of methods of LTS container
@{
IlLTS::StateIterator
IlLTS::beginStates()
{
  return( beginNodes() );
}

IlLTS::StateIterator
IlLTS::endStates()
{
  return( endNodes() );
}

long
IlLTS::nofStates() const
{
  return( nofNodes() );
}

IlLTS::TransitionIterator
IlLTS::beginTransitions()
{
  return( beginEdges() );
}

IlLTS::TransitionIterator
IlLTS::endTransitions()
{
  return( endEdges() );
}

long
IlLTS::nofTransitions() const
{
  return( nofEdges() );
}

IlLTS::ActionIterator
IlLTS::beginAlphabet()
{
  return( d_alphabet.begin());
}

IlLTS::ActionIterator
IlLTS::endAlphabet()
{
  return( d_alphabet.end());
}

const string &
IlLTS::nameOfAction(action_t action)
{
  ActionIterator place = d_alphabet.find(action);
  if( d_alphabet.end() == place )
    {
      // Action was not found, return tau
      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      return( (*d_alphabet.find(0)).second );
    }
  return( (*place).second );
}
@| @}

Constructor and destructor do nothing else, but adds \(\tau\) to alphabet.
@D Implementation of methods of LTS container
@{
IlLTS::IlLTS()
  :  Ancestor()
{
  d_alphabet.insert(make_pair((action_t)0,string("tau")));
  // d_alphabet[0]="tau";
}

IlLTS::~IlLTS()
{
}
@| @}

Copy constructor and assignment operator are not implemented. They are
complicated and not really neaded. And class \verb<Graph< do not have
them either.
@d IlLTS class definition continued
@{
private:
  IlLTS(const IlLTS &); // Not implemented
  IlLTS &operator = (const IlLTS &); // Not implemented
@| @}


\subsection{Reading LSTS file}


Lts is read in using LSTS-file handling library~\cite{tvt:lsts}.
Library is connected to the program using callback functions. These
callbacks have to be in interface of datastructure object. Callbacks
are defined in class \verb<LSTS_AP<.

@d Implementation of methods of LTS container
@{
bool
IlLTS::initializeFromFile(InStream& lsts_file, const string& comment)
{
  iLSTS_File lsts( lsts_file );
  lsts.AddActionNamesReader( *this );
  lsts.AddTransitionsReader( *this );
  lsts.AddStatePropsReader(spcontainer);
  lsts.SetNoReaderAction( iLSTS_File::WARN );

  initializeDataStructures( lsts.GiveHeader(), comment );

  lsts.ReadFile();
  return true;
}
@}

\subsubsection{Initializing datastructures}

Space for data can be allocated only after reading LSTS header.

@d IlLTS class definition continued
@{
private:
void initializeDataStructures(const Header&, const string& comment);
@| @}

@d Implementation of methods of LTS container
@{void IlLTS::initializeDataStructures(const Header& header,
                                       const string& comment) {
    LTS_Draw_Write  piirros( cout );

    nodeIndexing(1, header.GiveStateCnt());
    for( lsts_index_t node_name = 1 ;
	 node_name < header.GiveStateCnt() ;
	 ++node_name ) {
	getNode(node_name).name(node_name);
    }
    edgeIndexing(1, header.GiveTransitionCnt() );
    getNode( header.GiveInitialState()).addFlag(State::INITIAL_STATE);
    if(comment.size())
        piirros.TulostaHistoria(comment);
    piirros.TulostaHistoria
        ( valueToMessage("No of states: " , header.GiveStateCnt(), ""));
    piirros.TulostaHistoria
        ( valueToMessage("  No of arcs: " , header.GiveTransitionCnt(), ""));
}@| @}



\subsubsection{History of file}

%%% LTS_Draw_Write  piirros( cout );
%%  for( int idx = 0; parser.GetHistoryString(idx); ++idx )   
%%    {                                                       
%%      piirros.TulostaHistoria(parser.GetHistoryString(idx));
%%    }                                                       


\subsubsection{Reading alphabet}

@d IlLTS class definition continued
@{
void lsts_StartActionNames( Header& ); // <<--Timo muutti parametrin 
void lsts_ActionName(lsts_index_t action, const string& name);
void lsts_EndActionNames( );
@| @}


@d Implementation of methods of LTS container
@{void IlLTS::lsts_StartActionNames( Header& ) {  }
void IlLTS::lsts_ActionName(lsts_index_t action, const string& name) {
    d_alphabet.insert( make_pair(action,name) );
    }
void IlLTS::lsts_EndActionNames( ) { }
@| @}



\subsubsection{Reading transitions}

@d IlLTS class definition continued
@{void lsts_StartTransitions( Header& );
void lsts_StartTransitionsFromState( lsts_index_t start_state );
void lsts_Transition( lsts_index_t start_state,
                      lsts_index_t dest_state,
                      lsts_index_t action );
void lsts_EndTransitionsFromState( lsts_index_t start_state );
void lsts_EndTransitions();
@| @}


@d Implementation of methods of LTS container
@{void IlLTS::lsts_StartTransitions( Header& hd)
{
    if(spcontainer.isInitialized())
    {
        vector<bool> cutProps(spcontainer.getMaxStatePropNameNumber()+1);
        for(unsigned i=1; i<cutProps.size(); ++i)
            cutProps[i] = (spcontainer.getStatePropName(i)[0] == '%');

        for(unsigned state = 1; state <= hd.GiveStateCnt(); ++state)
        {
            StatePropsContainer::StatePropsPtr spptr =
                spcontainer.getStateProps(state);
            for(unsigned sp = spcontainer.firstProp(spptr); sp;
                sp = spcontainer.nextProp(spptr))
            {
                if(cutProps[sp])
                {
                    getNode(state).addFlag(State::CUTSTATE_FLAG);
                    break;
                }
            }
        }
    }
};

void IlLTS::lsts_StartTransitionsFromState( lsts_index_t ) {};

void IlLTS::lsts_Transition( lsts_index_t start_state,
                      lsts_index_t dest_state,
                      lsts_index_t action ) {
    Transition trans;
    trans.action(action);
    addConnection(getNode(start_state),getNode(dest_state),trans);
    getNode(start_state).addFlag(State::HASTRANSITIONS_FLAG);
}

void IlLTS::lsts_EndTransitionsFromState( lsts_index_t ) {}
void IlLTS::lsts_EndTransitions() {}
@| @}



@D Read test states if exist NOT USED
@{
if( parser.CheckExtension(EXTENSION_TEST_STATES) )
  {
    for( state_t s= parser.GetNextRejectionState() ;
    	 0 != s ;
    	 s = parser.GetNextRejectionState() )
      {
        getNode(s).addFlag(State::REJECTION_FLAG);
      }
    for( state_t s= parser.GetNextDeadlockCheckState() ;
    	 0 != s ;
    	 s = parser.GetNextDeadlockCheckState() )
      {
        getNode(s).addFlag(State::DEADLOCK_FLAG);
      }
    for( state_t s= parser.GetNextLivelockCheckState() ;
    	 0 != s ;
    	 s = parser.GetNextLivelockCheckState() )
      {
        getNode(s).addFlag(State::LIVELOCK_FLAG);
      }
  }
@| @}

@D Skip acceptance sets if exist NOT USED
@{
if( parser.CheckExtension(EXTENSION_ACC_GRAPH_INFO) )
  {
    state_t s;
    long setcount;
    action_t a;
    // cout << "Divergence states: ";
    while((s=parser.GetNextDivergenceState()) != 0) {
      // cout << s << " ";
    }
    // cout << endl << "Acceptance sets" << endl;
    
    while( (s = parser.GetNextStateAccSet(setcount)) != 0) {
      // cout << "State " << s << ": ";
      if (setcount == 0) {
	// cout << "none";
      } else {
	while (setcount--) {
	  // cout << " {";
	  while(true) {
	    a = parser.GetNextAccSetMember();
	    if (a==0) { 
	      // cout << "}";
	      break;
	    } else {
	      // cout << a << " ";
	    }
	  }
	}
      }
      // cout << endl;
    }
  }
@| @}


\section{Optimization model}\label{sec:model}

Full objective of this program is to get automatically reasonably good
picture of given small LTS. Any prior knowledge of graph drawing helps
a lot, since goal is basically same.
For algorithm design problem is formulated as optimization model.

\subsection{Decision variables}

At decision varible level picture consists of points and arrow headed
bended lines. Types of those entities, \verb<Point< and \verb<Line<
are defined in \cite{ALA:prog:tools}
@d State class definition continued
@{
public:
  Point &coords() {return d_coords;};
  const Point &coords() const {return d_coords;};
private:
  Point d_coords;
@| @}
@d Transition class definition continued
@{
public:
  Line &line() {return d_line;};
  const Line &line() const {return d_line;};
private:
  Line d_line;
@| @}
This definition is brute forse. It might be cleaner to derive state
and transition from correspondig graphical objects.

Many places the integer coordinates of state is needed, so simple
shortcut is defined.
@d State class definition continued
@{
public:
  int Xcoord() {return (int)(coords().xCoord()); };
  int Ycoord() {return (int)(coords().yCoord()); };
@| @}
This implementation is dangerous, but fast.
@d Fixed (safe) integercoordinates of state
@{
static int
safeRound(double x)
{
  double rval = floor(x+0.5);
  assert( (fabs(rval-x) < 0.1) && "Value is not close to integer" );
  return( (int)(rval) );
}

int
IlStateBase::Xcoord()
{
  return( safeRound(coords().xCoord()) );
}
int
IlStateBase::Ycoord()
{
  return( safeRound(coords().yCoord()) );
}
@| @}



\subsection{Objective function}

Objective is to mimimize sum of lengths of transitions and number of
needed bends. In this context length of tranisition is max-norm of
vector defined by head and tail states of transition.

@d objfunc.hh
@{
inline
double
IlSingleTransitionCost(const IlLTS::Transition &trans)
{
  return( maxNorm(trans.head().coords(),trans.tail().coords())
	  + 0.00001*euclNormP2(trans.head().coords(),trans.tail().coords())
	  );
}

template<class ITER>
inline double
IlObjectiveFunction(ITER start,
		    ITER end)
{
  double cost = 0.0;
  for( ; start != end ; ++start )
    {
      cost += IlSingleTransitionCost(*start);
    }
  return( cost );
}
@| @}

In addition to these low level funtions, there is algorithm specific
versions of objective function. Their purpose is to make calculation
of change of objective function more effective then solution is
modified.

\subsection{Constrains}

Most important constraints are that coordinates of states have to be
integers and states have to have distinct coordinates. This is
obtained using special grid class defined in section \ref{sec:grid} p.
\pageref{sec:grid}

Constraints for transitions are similar, they have to be distinct and
any transition may not cross a state.


\subsection{Analyzing of model}

Optimization model defined earlier is extreamly simple. Only little
difficult part is obey constraints of transitions. Most cases it is
sufficient to select directions in which transitions leave from or enter to
states. This can be done easily with linear optimization model as seen
in section \ref{sec:angles}. But when transition goes to close to
state, additional bends are needed.

Other question is whether this kind of model is sufficient to produce
nice layout. Simple answer is no, but more complicated models are more
difficult to implement, they run slower and there is not any guarantee
of better picture. According to previus versions of illustrator this
simple model produces reasonable layout, if LTS is small enough.


%======================================================================
%======================================================================
%======================================================================



\chapter{Existing similar algorithms}

\section{Graph drawing optimization models and algorithms in literature}

Graph drawing is important problem since graph is handy representation
of many real world constructs. That is why there is very active
research community trying to find models and algorithms to model and
solve graph drawing problems.

There are three main 

\begin{description}
\item[layering method] Sugiyama, Tagawa, Toda 1981\\
  directed relations, edge crossings, edge length \cite{stt-mvuhs-81}

\item[force directed] spring embedder Eades 1984 \cite{e-hgd-84}\\
  magnetic spring algorithm Sugiyama Misue 1994 (GD '94) \cite{sm-sumdg-95}\\
edge length, node distances, arrow directions


\item[generic combinatorial optimization model] Davidson Harrel 1996 (1989)
ACM graphics  (Tech rep) \cite{dh-dgnus-96}\\
and Tungelang 1994 \cite{t-padud-1994}\\
Uniforme edgelength,
node distance according to topologial distance,
edge crossings,
node distribution,
picture area (boderlines),
node edge distance\\
solved by simulated annealing.
\end{description}



\section{Exinsting software}


\begin{description}
\item[Graphlet] http://www.uni-passau.de/Graphlet/\\ (GD '96 Berkeley)
  http://portal.research.bell-labs.com/orgs/ssr/people/north/p2/himsolt.ps\\ 
  http://portal.research.bell-labs.com/orgs/ssr/people/north/gd96.html\\ 
  has many nice graph drawing algorithms, but do not handle loops and
  multible edges reasonably.
\item[GraphEd] Former version of graphlet? Grapheditor, GD '94, p. 182. [[Tutki lis‰‰]]
\item[daVinci] GD '94 p. 266 and inline www-documentation, overview-reference-attributes \\
http://www.informatik.uni-bremen.de/~davinci/docs/referenceF.html\\{}
[[Ei tue merkkijonoja kaarien tunnisteina, v‰rej‰ ja viivatyylej‰ kyll‰kin]].
layering algorithm.

\item[VCG --- Visualization of Compiler Graphs] GD '94 \\
http://www.cs.uni-sb.de/RW/users/sander/html/gsvcg1.html \\
fast and suitable for large graphs implicate that it is layering algorithm ?


\item[dot and dotty] http://www.research.att.com/sw/tools/reuse/\\
  Made by At\&t. Has many capabilities needed for
  automaton visualization, like edge labels, multible edges and so on.
  Has some editing capabilities too. License is against ALA
  programming conventions.\\

\item[graphplace]
ftp://ftp.Germany.EU.net/pub/research/theory/graphs/graphplace.tar.gz\\
layering algorithm, simple, 1994


\item[ffgraph] http://www.fmi.uni-passau.de/~friedric/ffgraph/main.shtml

\item[dot and dotty] http://seclab.cs.ucdavis.edu/~hoagland/Dot.html

\item[GLT] Commercial http://www.tomsawyer.com/

\end{description}

These are earlier version of same program. All of them have very
similar approach than this program
\begin{description}
\item[araillus] is part of ARA tool.
\item[lts2ps] is made by students and converts labelled transition
  systems to postscript files.
\end{description}


%======================================================================
%======================================================================
%======================================================================


\chapter{Algorithm}


Interface of graph layout algorithm consists only of
\verb<IlLayoutAlgorithm<-function.
@d layout.hh
@{
bool
IlLayoutAlgorithm(IlLTS &lts);
@| @}
The parameter \verb<lts< is used as input and output parameter.
@d layout.cc
@{
@<Local functions in top level layout algorithm@>

bool
IlLayoutAlgorithm(IlLTS &lts)
{
  @<Allocate auxiliary datastructures@>

  @<Find initial placement of states@>
  @<Find state coordinates using local search optimization method@>
  @<Normalize state coordinates@>
  @<Creating arrows for transitions@>
  // 
  return true;
}
@| IlLayoutAlgorithm @}


In MVC pattern this function is minor controller meaning, it has
neither presistent data nor view, but has low level access to data.



\section{Allocate auxiliary datastructures for layout algorithm}

Using article \cite{s-dgrg-95} as hint, suitable and natural drawing
area is grid of \(N\times N\) points for states, where \(N\) is number
of states.  It might be too large in some cases, but as visualized
labelled transitions systems are usually quite small, memory
consumption is not too big.
@d Allocate auxiliary datastructures
@{
  IlGrid stateGrid(lts.nofStates());
  double costFuncVal;
@| @}



\section{Initial solution}

In old version of this tool, initial solution was constructed by
placing states along spiral starting from out edge of fixed size
drawing area. For new version my first guess do not perform well in
combination of simple local search method. So there is several methods
to construct initial solution.
@D Find initial placement of states
@{
const int VERY_SMALL_LTS = 3;
const int MEDIUM_SIZE_LTS = 300;
if( lts.nofStates() <= VERY_SMALL_LTS )
  {
    IlInitialSolution(lts.nofStates(),
		      stateGrid,
		      lts.beginStates(),
		      lts.endStates());
  }
else if ( lts.nofStates() <= MEDIUM_SIZE_LTS )
  {
    IlNonSymmetricInitialSolution(lts.nofStates(),
				  stateGrid,
				  lts.beginStates(),
				  lts.endStates());
  }
else
  {
    IlBigInitialSolution(lts.nofStates(),
			 stateGrid,
			 lts.beginStates(),
			 lts.endStates());
  }
costFuncVal = IlObjectiveFunction(lts.beginTransitions(),
				  lts.endTransitions());
@| VERY_SMALL_LTS MEDIUM_SIZE_LTS@}
No construction method is selected according to size of LTS. In
general there should be way to select used method by user.


\section{Optimization algorithms}

Most of these algorithms are based to local modifications and can not
be used witout initial solution.
@d Find state coordinates using local search optimization method
@{
#if 0
IlLocalSearchCoordinates( stateGrid,
			  lts.beginStates(),
			  lts.endStates(),
			  costFuncVal);
#endif
double lowerBoundForCost = lts.nofTransitions();
@<Remove loops from \verb<lowerBoundForCost<@>
if( lts.nofStates() >= 3 ) {
    IlSimulatedAnnealing(stateGrid,
			 lts.beginStates(),
			 lts.endStates(),
			 costFuncVal,
			 lowerBoundForCost);
    IlLocalSearchCoordinates( stateGrid,
			      lts.beginStates(),
			      lts.endStates(),
			      costFuncVal);
}
#if 0
  IlGeneticAlgorithForCoordinates(lts.beginStates(),
				  lts.endStates(),
				  lts.beginTransitions(),
				  lts.endTransitions());
#endif
@| @}

@D Remove loops from \verb<lowerBoundForCost<
@{
for( IlLTS::TransitionIterator ii = lts.beginTransitions() ;
     ii != lts.endTransitions() ;
     ++ ii )
  {
    if( ii->head() == ii->tail() )
      {
	lowerBoundForCost -= 1.0;
      }
  }
@| @}



\section{Move states to right place}

Plain optimization methods may let states anywhere in drawing area.
They have to be moved to top left hand corner.
@d Normalize state coordinates
@{
  moveStatesToOrigin(lts.beginStates(),
		     lts.endStates());
 //!!!!!!!!!!!!!! update grid !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
@| @}

@D Local functions in top level layout algorithm
@{
static void
moveStatesToOrigin(IlLTS::StateIterator start_I,
		   IlLTS::StateIterator end_I)
{
@<Move states to top left corner@>
}
@| @}
@D Move states to top left corner
@{
  double minXCoord = (*start_I).coords().xCoord();
  double minYCoord = (*start_I).coords().yCoord();
  for( IlLTS::StateIterator ii = start_I;
       ii != end_I;
       ++ii )
    {
      if( (*ii).coords().xCoord() < minXCoord )
	{
	  minXCoord = (*ii).coords().xCoord();
	}
      if( (*ii).coords().yCoord() < minYCoord )
	{
	  minYCoord = (*ii).coords().yCoord();
	}
    }
  Point delta(1-minXCoord,1-minYCoord);
  for( IlLTS::StateIterator ii = start_I;
       ii != end_I;
       ++ii )
    {
      (*ii).coords().move(delta);
    }
@| @}



\section{Routing transitions}

@d Creating arrows for transitions
@{
IlTransitionArrows(lts.beginStates(),
		   lts.endStates(),
		   lts.beginTransitions(),
		   lts.endTransitions());
@| @}



\chapter{Algorithms for initial solution}

\section{Symmetric full area initial solution}

@d initial.hh
@{
void
IlInitialSolution(int             N_I,
		  IlGrid          &grid_O,
                  IlLTS::StateIterator start_I,
                  IlLTS::StateIterator end_I);
@| @}

@d initial.cc
@{

#include <iostream>
using namespace std;

@<Initial solution@>
@| @}


\marginpar{\textbf{Comment}\\It test phase it come out, that initial
  solution is too symmetric. In big cases search stops too early.
  Solution might be to use full strength simulated annealing algorithm or
  other kind initial solution. Later is tried here.} Old version of
this algorithm places states to a spiral at the edge of grid. My first
idea was to put them to diagonal. But now I think, that even more
better idea is to put them evenly distributed around the edge of grid
and one at center of grid. Initial solution can not be formed in
earlier version this way because size of drawing area is program
constant. In this version size of drawing area depends on number of
states. Using article \cite{s-dgrg-95} as hint, suitable drawing area
is \(N\times N\) grid points for states, where \(N\) is number of
states. It might be too large in some cases, but it gives time to find
good solution.

@D Initial solution
@{
void
IlInitialSolution(int             N_I,
		  IlGrid          &grid_O,
                  IlLTS::StateIterator start_I,
                  IlLTS::StateIterator end_I)
{
  int curX;
  int curY;
  IlLTS::StateIterator ii;
  if( N_I < 4 )
    {
      @<Spesial case for few state@>
    }
  else
    {
      curX=0;
      curY=0;
  
      for( ii=start_I ;
           (ii != end_I) && (curX < N_I) ;
           ++ii )
        {
          if( ! grid_O.placeState(*ii,curX,curY) )
            {
              @<Report internal error at IlInitialSolution@>
              exit(1);
            }
          curX += 4;
        }
      curY += curX - N_I + 1;
      curX  = N_I - 1;
      for( ;
           (ii != end_I) && (curY < N_I) ;
           ++ii )
        {
          if( ! grid_O.placeState(*ii,curX,curY) )
            {
              @<Report internal error at IlInitialSolution@>
              exit(1);
            }
          curY += 4;
        }
      curX -= curY - N_I + 1;
      curY  = N_I -1 ;
      for( ;
           (ii != end_I) && (curX >= 0) ;
           ++ii )
        {
          if( ! grid_O.placeState(*ii,curX,curY) )
            {
              @<Report internal error at IlInitialSolution@>
              exit(1);
            }
          curX -= 4;
        }
      curY += curX;   // -= 0-curX
      curX  = 0;
      for( ;
           (ii != end_I) && (curY > 0) ;
           ++ii )
        {
          if( ! grid_O.placeState(*ii,curX,curY) )
            {
              @<Report internal error at IlInitialSolution@>
              exit(1);
            }
          curY -= 4;
        }
      if( ii != end_I )
        {
          if( ! grid_O.placeState(*ii,N_I/2,N_I/2) )
            {
              @<Report internal error at IlInitialSolution@>
              exit(1);
            }
          ++ii;
        }
      if( ii != end_I )
        {
           cerr << "illus: Strange, too much states!" << endl;
           exit(1);
        }
    }
}		  
@| @}
Method above do not work properly, if there is only less than four
states. The most simples way is to put then to the diagonal of grid
@d Spesial case for few state
@{
for( ii = start_I, curX = 0 ;
     ii != end_I;
     ++ii, ++curX )
  {
    // cerr << "Olemme lis‰‰m‰ss‰ tilaa koordinaatteihin "
    // << curX << endl;
    if( ! grid_O.placeState(*ii,curX,curX) )
      {
        @<Report internal error at IlInitialSolution@>
        exit(1);
      }
  }
@| @}

@d Report internal error at IlInitialSolution
@{
  std::cerr << "illus: Internal error at funtion "
       << "IlInitialSolution: "
       << __FILE__ << " "
       << __LINE__
       << std::endl;
@| @}



\section{Initial solution useing layering}

The algorithm used above do not take count any information about
structure of LTS. This one make levelization according to graph
geometric distance from selected starting node and arrange other nodes
around starting node according them level.
@d level.hh
@{
void
IlBigInitialSolution(int             N_I,
		     IlGrid          &grid_O,
		     IlLTS::StateIterator start_I,
		     IlLTS::StateIterator end_I);
@| @}


@D level.cc
@{
void
IlBigInitialSolution(int             N_I,
		     IlGrid          &grid_O,
		     IlLTS::StateIterator start_I,
		     IlLTS::StateIterator /*end_I*/)
{
  typedef map<state_t,int> NodeSet;
  NodeSet done;
  typedef list<IlLTS::State *> Queue;
  typedef IteratorAdaptor<IlLTS::State, Queue::iterator> QueueIterator;
  Queue order;
  vector<int> levelSize;


  done.insert(make_pair(start_I->name(),0));
  order.push_back(&(*start_I));
  levelSize.push_back(1);

  for( QueueIterator current = order.begin();
       current != order.end() ;
       ++current )
    {
      int level = (done.find(current->name()))->second + 1;
      if( static_cast<size_t>(level) >= levelSize.size() )
	{
	  levelSize.push_back(0);
	}

      typedef IlLTS::State::AdjacencyIterator AdjacencyIterator;

      for( AdjacencyIterator ii = current->beginLeaving() ;
	   ii != current->endLeaving();
	   ++ii )
	{
	  IlLTS::State &other = (*ii).head();
	  if( done.end() == done.find(other.name()) )
	    {
	      done.insert(make_pair(other.name(),level));
	      levelSize[level] += 1;
	      order.push_back(&other);
	    }
      }
      for( AdjacencyIterator ii = current->beginEntering() ;
	   ii != current->endEntering();
	   ++ii )
	{
	  IlLTS::State &other = (*ii).tail();
	  if( done.end() == done.find(other.name()) )
	    {
	      done.insert(make_pair(other.name(),level));
	      levelSize[level] += 1;
	      order.push_back(&other);
	    }
	}
    }

  int center = N_I / 2 ;
  QueueIterator current = order.begin();
  grid_O.placeState(*current, center, center);
  ++current;
  int radius_add = 0;

  while( current != order.end() )
    {
      int level = (done.find(current->name()))->second;
      int radius = level + radius_add;
      for( ; radius * 8 < levelSize[level] ; radius = level + radius_add )
	{
	  ++ radius_add;
	}
      int diam = radius * 2;
      int offset = (radius * 8 ) / (levelSize[level]+1);
      if ( 0 == offset ) offset = 1;
      for( int ii = 0; ii < levelSize[level] ; ++ii , ++current )
	{
	  int side = (ii *offset ) / diam;
	  int item = (ii *offset ) % diam;
	  int x=0;
	  int y=0;

	  if( 0 == side )
	    {
	      x = item;
	      y=0;
	    }
	  else if ( 1 == side )
	    {
	      x = diam;
	      y = item;
	    }
	  else if ( 2 == side )
	    {
	      x = diam - item;
	      y = diam;
	    }
	  else
	    {
	      x = 0;
	      y = diam - item;
	    }
	  x += (center - radius);
	  y += (center - radius);
	  grid_O.placeState(*current,x,y);
	}
    }
}
@| @}


\section{Non-symmetric initial solution}

Layering method can not be used to small or medium size systems. How ever


@d nonsym.hh
@{

void
IlNonSymmetricInitialSolution( int             N_I,
			       IlGrid          &grid_O,
			       IlLTS::StateIterator start_I,
			       IlLTS::StateIterator end_I);
@| @}
@D nonsym.cc
@{

#include <iostream>
using namespace std;

void
IlNonSymmetricInitialSolution( int             N_I,
			       IlGrid          &grid_O,
			       IlLTS::StateIterator start_I,
			       IlLTS::StateIterator end_I)
{
  IlLTS::StateIterator ii=start_I;
  if( ! grid_O.placeState(*ii,0,0) )
    {
      @<Report internal error at IlInitialSolution@>
	  exit(1);
    }
  int cur_loc=2;
  
  for( ++ii ; (ii != end_I) && (cur_loc < N_I) ;  )
    {
      if( ! grid_O.placeState(*ii,cur_loc,0) )
	{
	  @<Report internal error at IlInitialSolution@>
              exit(1);
	}
      ++ii;
      if( ! grid_O.placeState(*ii,0,cur_loc) )
	{
	  @<Report internal error at IlInitialSolution@>
              exit(1);
	}
      ++ii;
      cur_loc += 2;
    }
  if( ii != end_I )
    {
      cur_loc = N_I / 2;
      if( ! grid_O.placeState(*ii,cur_loc,cur_loc) )
	{
	  @<Report internal error at IlInitialSolution@>
              exit(1);
	}
      ++ii;
    }
  if( ii != end_I )
    {
      cerr << "illus: Strange, too much states!" << endl;
      exit(1);
    }
}

@| @}




\chapter{Search methods}


\section{Local search method for state coordinates}

@d localsea.hh
@{
void
IlLocalSearchCoordinates( IlGrid          grid_IO,
			  IlLTS::StateIterator start_I,
			  IlLTS::StateIterator end_I,
                          double &costFuncVal_IO);
@| IlLocalSearchCoordinates @}

@d localsea.cc
@{
@<Partial objective functions@>

@<Local search function@>
@| @}

Basic idea in local search optimization method is to do small
modification to solution and only modification which make solution
better is allowed. In this version there is two different local
modification methods. They are swap of coordinates of two states and
move of single state. These operations are applied until no better
solution can be found. The order these operations are applied is
critical for geting optimal solution.

@D Local search function
@{
void
IlLocalSearchCoordinates( IlGrid          grid_IO,
			  IlLTS::StateIterator start_I,
			  IlLTS::StateIterator end_I,
                          double &costFuncVal_IO)
{
  double currentCost = costFuncVal_IO;
  double lastCost = currentCost;
  
#if 0
Seuraava koodi on kommentoitu pois
  cout << "Valitulos" << endl;
  for( int ii=0; ii<N_I ; ++ii )
    {
      cout << ii + 1 << " "
	   << states_IO[ii].Xcoord() << " "
	   << states_IO[ii].Ycoord() << endl;
    }
#endif
  do {
      lastCost = currentCost;
      //cerr << "Kohdefunktio: " << currentCost << endl;
#if 1
      @<Apply swap operator to minimum@>
#endif
      // cerr << "Kohdefunktio (vaihto): " << currentCost << endl;
#if 1
      @<Alternate apply move operator to minimum@>
#endif
#if 0
      if( currentCost >= lastCost )
	{
	  IlShrinkLayout(grid_IO,start_I,end_I,currentCost);
          cerr << "Kutistettu kohdefunktio " << currentCost << endl;
	}
#endif
  } while ( currentCost < lastCost );

  // Comment out to end of function, if arrow
  // placement function needs grid to be up to date
#if 0
  @<Move states to top left corner@>
#endif
  costFuncVal_IO = currentCost ;
} 
@| @}

@D Apply swap operator to minimum
@{
  double swapCost;
  const double ZERO_LIMIT = 1.0e-12;
  //const double ZERO_LIMIT = 0.0;
  // Etenemisviesti:
  //cerr << "Ennen Swap-silmukkaa" << endl;
  do {
    // Etenemisviesti:
    //cerr << "Swap-silmukan alussa" << endl;
    swapCost = currentCost ;
    for( IlLTS::StateIterator ii = start_I ;
         ii != end_I ;
         ++ii )
      {
        IlLTS::StateIterator jj = ii;
        for( ++jj ;
             jj != end_I ;
             ++jj )
          {
            if( *ii == *jj ) continue;
            double markCost = IlStatePairCost( *ii, *jj );
            grid_IO.swapStates( *ii, *jj );
            markCost -= IlStatePairCost( *ii, *jj );
            // Etenemisviesti:
            // cerr << "local Swap testi " << markCost << endl;
            if( markCost > ZERO_LIMIT )
              {
                currentCost -= markCost ;
              }
            else
              {
                grid_IO.undoLast();
              }
          }
      }
  } while( currentCost < swapCost );
@| @}

@D Alternate apply move operator to minimum
@{
  double moveCost;
  const int nof_neighbour = 8;
  static const int neighbourX[nof_neighbour]={-1,0,1,-1,1,-1,0,1};
  static const int neighbourY[nof_neighbour]={-1,-1,-1,0,0,1,1,1};

  vector<IlLTS::StateIterator> applyStack;
  long stateCnt = 0;
  for( IlLTS::StateIterator ii = start_I ; ii != end_I ; ++ii)
    {
      applyStack.push_back(ii);
      ++stateCnt;
    }
  do {
    moveCost = currentCost ;
    make_heap( applyStack.begin(),
	       applyStack.end(),
	       IlGreaterSingleStateCost());
    long outFromLoop = stateCnt ;
    // cerr << "Uusi kierros " << currentCost << " " << outFromLoop <<  endl;

    for( vector<IlLTS::StateIterator>::iterator ii = applyStack.begin() ;
	 outFromLoop && ii != applyStack.end() ;
	 ++ii, --outFromLoop )
      {
	IlLTS::State &currentState = *(*ii);
        int bestX = 0;
        int bestY = 0;
        double bestDeltaCost = -0.1;
        double originalCost = IlSingleStateCost(currentState) ;
	for( int jj = 0; jj < nof_neighbour ; ++jj )
	  {
	    int tx = currentState.Xcoord() + neighbourX[jj] ;
	    int ty = currentState.Ycoord() + neighbourY[jj] ;
	    if( grid_IO.isFree(tx,ty) )
	      {
		double markCost = originalCost ;
		grid_IO.moveState(currentState,tx,ty);
		markCost -= IlSingleStateCost(currentState) ;
		if( markCost > bestDeltaCost )
		  {
                    bestDeltaCost = markCost ;
                    bestX = tx;
                    bestY = ty;
 		    grid_IO.undoLast();
		  }
		else
		  {
 		    grid_IO.undoLast();
		  }
	      }
	  }
        // Etenemisviesti:
        //cerr << "local Move testi " << bestDeltaCost << endl;
        if( bestDeltaCost > ZERO_LIMIT )
	  {
	    grid_IO.moveState(currentState,bestX,bestY);
	    currentCost -= bestDeltaCost ;
	    // outFromLoop /= 2;
            // -- outFromLoop;
	  }
      }
  } while( currentCost < moveCost );
@| @}


@D Apply move operator to minimum
@{
  double moveCost;
  const int nof_neighbour = 8;
  static const int neighbourX[nof_neighbour]={-1,0,1,-1,1,-1,0,1};
  static const int neighbourY[nof_neighbour]={-1,-1,-1,0,0,1,1,1};

  do {
    moveCost = currentCost ;
    for( IlLTS::StateIterator ii = start_I ;
         ii != end_I ;
         ++ii )
      {
        for( int jj = 0;
             jj < nof_neighbour ;
             ++jj )
          {
            int tx = (*ii).Xcoord() + neighbourX[jj] ;
            int ty = (*ii).Ycoord() + neighbourY[jj] ;
            if( grid_IO.isFree(tx,ty) )
              {
                double markCost = IlSingleStateCost(*ii) ;
                grid_IO.moveState(*ii,tx,ty);
                markCost -= IlSingleStateCost(*ii) ;
                if( markCost > 0.0 )
                  {
                    currentCost -= markCost ;
                    break;
                  }
                else
                  {
                    grid_IO.undoLast();
                  }
              }
          }
      }
  } while( currentCost < moveCost ) ;
@| @}


\subsection{Partial objective functions}

The above algorithm uses function \verb<IlStatePairCost< and
\verb<IlSingleStateCost< to calculate partial objective function. Only
transitions, of which cost can be chance, in counted in.
@d Partial objective functions
@{
static double
IlStatePairCost(IlLTS::State &u, IlLTS::State &v)
{
  double cost = 0.0;
  cost += IlObjectiveFunction(u.beginLeaving(),u.endLeaving());
  cost += IlObjectiveFunction(u.beginEntering(),u.endEntering());
  cost += IlObjectiveFunction(v.beginLeaving(),v.endLeaving());
  cost += IlObjectiveFunction(v.beginEntering(),v.endEntering());
  // Transitions, which connect states u and v are counted twise.
  // This do not matter, iff costfuntion for single transition is
  // symmetric and swap operator do not change cost of those transitions.
  return cost;
}
@| @}
\textbf{WARNING!} cost of transitions, which connect states \(u\) and
\(v\) is counted twice. This do not matter in this contex, since cost
of transitions do not chance if direction of transition is changed and
costs counted twice are first added to and then subtracted from total
objective function.

@d Partial objective functions
@{
static double
IlSingleStateCost(IlLTS::State &s)
{
  double cost = 0.0;
  cost += IlObjectiveFunction(s.beginLeaving(),s.endLeaving());
  cost += IlObjectiveFunction(s.beginEntering(),s.endEntering());
  return cost;
}

#ifndef DO_NOT_INCLUDE
class IlGreaterSingleStateCost
{
public:
  bool operator () (IlLTS::StateIterator &a, IlLTS::StateIterator &b);
};

bool
IlGreaterSingleStateCost::operator () (IlLTS::StateIterator &a,
				       IlLTS::StateIterator &b)
{
  Point pA(0.0,0.0);
  Point pB(0.0,0.0);
  for(IlLTS::State::AdjacencyIterator jj = (*a).beginEntering() ;
      jj != (*a).endEntering() ;
      ++jj )
    {
      pA.move( (*jj).tail().coords() - (*a).coords() );
    }

  for(IlLTS::State::AdjacencyIterator jj = (*a).beginLeaving() ;
      jj != (*a).endLeaving() ;
      ++jj )
    {
      pA.move( (*jj).head().coords() - (*a).coords() );
    }

  for(IlLTS::State::AdjacencyIterator jj = (*b).beginEntering() ;
      jj != (*b).endEntering() ;
      ++jj )
    {
      pB.move( (*jj).tail().coords() - (*b).coords() );
    }

  for(IlLTS::State::AdjacencyIterator jj = (*b).beginLeaving() ;
      jj != (*b).endLeaving() ;
      ++jj )
    {
      pB.move( (*jj).head().coords() - (*b).coords() );
    }

  return( innerProduct(pA,pA) > innerProduct(pB,pB) );
}
#endif
@| @}



\section{Simulated annealing}

Simulated annealing is randomized local search method. It is simple
and slow, but it sometimes find good solutions if anything else fails.
@d sa.hh
@{
void
IlSimulatedAnnealing(IlGrid grid_IO,
		     IlLTS::StateIterator start_I,
		     IlLTS::StateIterator end_I,
		     double &costFuncVal_IO,
                     double absolutMinumunEstimate = 0.0 );
@| @}
Then this function is called, provide number of transitions as last
parameter, since it is good lower bound for objective function.
And be careful that \verb<IlLTS::StateIterator< is random access iterator.

Pseudo code for simulated annealing can be found in \cite[p. 16]{ak-sabm-1981}
@D sa.cc
@{
@<Local functions for simulated annealing@>

void
IlSimulatedAnnealing(IlGrid grid_IO,
		     IlLTS::StateIterator start_I,
		     IlLTS::StateIterator end_I,
		     double &costFuncVal_IO,
                     double absolutMinumunEstimate )
{
  int N = end_I - start_I;
  double currentCost = costFuncVal_IO;
  double bestFound = currentCost ;
  int loopCnt;
  int solutionsGenerated = 0;
  double temperature = currentCost;
  bool   solutionGoodEnought = false;
  //cerr << "Starting SA" << endl;
  //cerr << currentCost << "  " << absolutMinumunEstimate << endl;
  @<Initialize loopcount@>
  @<Adjust or initialize temperature@>
  @<Adjust or initialize end condition@>
  while( ! solutionGoodEnought )
    {
      for( int ii=0; ii < loopCnt ; ++ii)
	{
	  @<Select next solution@>
	  @<If solution is not accepted, restore old one@>
	}
      @<Update variables and loopcount@>
      @<Adjust or initialize temperature@>
      @<Adjust or initialize end condition@>
    }
  /*
  cerr << "End of SA" << endl;
  cerr << solutionsGenerated << "  " << currentCost << " "
       << costFuncVal_IO << endl;
  */
  costFuncVal_IO = currentCost ;
}
@| @}


\subsection{Generation of new solutions}

Selection of new solution is made randomly using same local
modification operatotors as local search optimization methods.
@d Select next solution
@{
double markCost = 0.0;
@| @}
First operator type is selected
@d Select next solution
@{
const double SWAP_RATIO_IN_SA = 0.5;
if( Random::normal() < SWAP_RATIO_IN_SA )
  {
    @<Generate next solution using node swap@>
  }
else
  {
    @<Generate next solution by moving node@>
  }
@| SWAP_RATIO_IN_SA @}

For state swap, two states are selected randomly
@d Generate next solution using node swap
@{
  int inx = Random::generate(N);
  int jnx = Random::generate(N-1);
  if( jnx >= inx ) ++jnx;
  markCost = IlStatePairCost(*(start_I+inx), *(start_I+jnx));
  grid_IO.swapStates( *(start_I+inx), *(start_I+jnx) );
  markCost -= IlStatePairCost(*(start_I+inx), *(start_I+jnx));
@| @}

For state move one state and the new position is selected randomly.
One legal moves are allowed.
@D Generate next solution by moving node
@{
  static const int NOF_NEIGHBOUR = 8;
  static const int neighbourX[NOF_NEIGHBOUR]={-1,0,1,-1,1,-1,0,1};
  static const int neighbourY[NOF_NEIGHBOUR]={-1,-1,-1,0,0,1,1,1};
  int sidx = Random::generate(N);
  int neighb = Random::generate(NOF_NEIGHBOUR);
  int tx = (start_I+sidx)->Xcoord() + neighbourX[neighb];
  int ty = (start_I+sidx)->Ycoord() + neighbourY[neighb];
  while( !grid_IO.isFree(tx,ty) )
    {
      sidx = Random::generate(N);
      neighb = Random::generate(NOF_NEIGHBOUR);
      tx = (start_I+sidx)->Xcoord() + neighbourX[neighb];
      ty = (start_I+sidx)->Ycoord() + neighbourY[neighb];
    }
  markCost = IlSingleStateCost(*(start_I+sidx));
  grid_IO.moveState(*(start_I+sidx),tx,ty);
  markCost -= IlSingleStateCost(*(start_I+sidx));
@| @}

This newly generated solution is accepted if it is better than old one
or with probability
\(e^{\mathtt{markCost}/\mathtt{temperature}}\).
@d If solution is not accepted, restore old one
@{
if( markCost < 0.0 && exp(markCost / temperature) < Random::normal() )
  {
    grid_IO.undoLast();
  }
else
  {
     currentCost -= markCost ;
  }
@| @}

\subsection{Cooling and end conditions}

Most difficult part of simulated annealing is selecting cooling
scheduler and stop criteria. In this case the assumption that lower
bound of objective function is strict enough,  is made.

Temperature is adjusted so, that probability accepting solution which
is 30\% further from lower bound is \(e^{-1}\).
@d Adjust or initialize temperature
@{
{
  bestFound = min( bestFound , currentCost );
  double costDistance = bestFound - absolutMinumunEstimate ;
  const double COOLING_FACTOR = 0.999;
  if( 0.3 * costDistance < COOLING_FACTOR * temperature )
    {
      temperature = 0.3 * costDistance;
      //cerr << "Temp = " << temperature << endl;
    }
  else
    {
      temperature *= COOLING_FACTOR;
    }
}
@| COOLING_FACTOR @}

Calculation is interrupted if 500 modifications per grid point are made or
quality of solution is consider good enough.
@d Adjust or initialize end condition
@{
{
  const int ROUND_CNT_COFF = 500;
  solutionGoodEnought = (ROUND_CNT_COFF * N * N < solutionsGenerated
			 || currentCost < 1.11 * absolutMinumunEstimate );
}
@| ROUND_CNT_COFF @}

Speed of cooling depends about how many new solutions are generated
for each temperature. Simple solution to use constant is used.
@d Initialize loopcount
@{
  loopCnt = N;
@| @}
@d Update variables and loopcount
@{
  solutionsGenerated += loopCnt;
@| @}


\subsection{Local functions}

@d Local functions for simulated annealing
@{
#define DO_NOT_INCLUDE 1
@<Partial objective functions@>
#undef DO_NOT_INCLUDE
@| @}




\section{Forced shrink}

Above method of using only swap and move states do not produce good
solution, when states are initially far from each other. Purpose of
this method is pull other states towards initial node.

@d shrink.hh
@{
void
IlShrinkLayout(IlGrid &grid_IO,
	       IlLTS::StateIterator start,
	       IlLTS::StateIterator end,
	       double &curObjFuncVal);
@| @}


@D shrink.cc
@{
@<Local functions of \verb<shrink<@>
void
IlShrinkLayout(IlGrid &grid_IO,
	       IlLTS::StateIterator start,
	       IlLTS::StateIterator end,
	       double &curObjFuncVal)
{
  @<Store old solution to \verb<oldSolution< and build \verb<notMoved<@>
  @<Move states using breadth first traversal@>
  @<If solution is not better, restore old one@>
}
@| @}

@d Store old solution to \verb<oldSolution<...
@{
vector<int> oldSolution;
double oldObjective = curObjFuncVal;
set<state_t> notMoved;
{
  long idx;
  IlLTS::StateIterator ss;
  set<state_t>::iterator pos = notMoved.begin();
  for( idx=0, ss = start ; ss != end ; ++ss )
    {
      oldSolution.push_back(ss->Xcoord());
      oldSolution.push_back(ss->Ycoord());
      pos = notMoved.insert(pos,ss->name());
    }
}
@| @}

@d Local functions of \verb<shrink<
@{

const int NEIGHBOUR_RADIUS = 1;
const int NEIGHBOUR_DIAMETER = NEIGHBOUR_RADIUS + NEIGHBOUR_RADIUS + 1;
const int NEIGHBOUR_SIZE = NEIGHBOUR_DIAMETER * NEIGHBOUR_DIAMETER;

inline static bool
localNeighbourAdjust(int place, int &x, int &y, int cx=0, int cy=0)
{
  int xDelta = (place / NEIGHBOUR_DIAMETER) - NEIGHBOUR_RADIUS;
  int yDelta = (place % NEIGHBOUR_DIAMETER) - NEIGHBOUR_RADIUS;
  x = cx + xDelta;
  y = cy + yDelta;
  return ( xDelta != 0 || yDelta != 0 );
}
@| @}


@d Move states using breadth first traversal
@{
{
typedef list<IlLTS::State *> StateList;
typedef IlLTS::State::AdjacencyIterator AdjacencyIterator;
typedef IlLTS::State State;
StateList handlingQueue;
handlingQueue.push_back( &(*start) );
notMoved.erase(start->name());

for( ; ! handlingQueue.empty() ; handlingQueue.pop_front() )
  {
    State &current = *(handlingQueue.front());
    for( AdjacencyIterator ii = current.beginLeaving() ;
	 ii != current.endLeaving();
	 ++ii )
      {
	State &other = (*ii).head();
        @<Select place for state \verb<other<@>
      }
    for( AdjacencyIterator ii = current.beginEntering() ;
	 ii != current.endEntering();
	 ++ii )
      {
	State &other = (*ii).tail();
        @<Select place for state \verb<other<@>
      }
  }  
}@| @}
\textsc{[[Irtopisteet eiv‰t sovi LTS:n tulkintaan.]]}

@D Select place for state \verb<other<
@{
if( notMoved.end() != notMoved.find(other.name() ) )
  {
    handlingQueue.push_back( &other );
    notMoved.erase(other.name());

    double baseCost = IlSingleStateCost(other);
            double bestDeltaCost = 0.0;
            int    bestPlace = -1;
    int nx, ny;
    for( int neix = 0 ; neix < NEIGHBOUR_SIZE ; ++neix )
      {
        if( localNeighbourAdjust(neix,nx,ny,
				 current.Xcoord(),
				 current.Ycoord())
	    && grid_IO.isFree(nx,ny) )
	  {
	    grid_IO.moveState(other,nx,ny);
	    double markCost = baseCost - IlSingleStateCost(other);
	    if( bestPlace < 0 || bestDeltaCost < markCost )
	      {
		bestPlace = neix;
		bestDeltaCost = markCost;
	      }
	    grid_IO.undoLast();
	  }
      }
    if( bestPlace >= 0 )
      {
	localNeighbourAdjust(bestPlace,nx,ny,
			     current.Xcoord(),
			     current.Ycoord());
	grid_IO.moveState(other,nx,ny);
	curObjFuncVal -= bestDeltaCost;
      }
  }
@| @}


@D If solution is not better, restore old one
@{
#if 1
if( oldObjective < curObjFuncVal )
  {
    grid_IO.reInit();
    long idx;
    IlLTS::StateIterator ss;
    for( idx=0, ss = start ; ss != end ; ++ss, idx+=2 )
      {
	grid_IO.placeState(*ss,oldSolution[idx],oldSolution[idx+1]);
      }
    curObjFuncVal = oldObjective;
  }  
#endif
@| @}


\section{Node coordinates using genetic algorithm}

This is very simple continuous genetic algorithm. It produces
sometimes reasonable layouts, but is slow.

@d genetic.hh
@{
void
IlGeneticAlgorithForCoordinates(IlLTS::StateIterator statesBeg_I,
				IlLTS::StateIterator statesEnd_I,
				IlLTS::TransitionIterator start_I,
				IlLTS::TransitionIterator end_I);
@| @}


@D genetic.cc
@{

class IlFitnessFunctionBase
{
public:
  virtual ~IlFitnessFunctionBase() {};
  virtual double operator () (double x) const = 0;
};

class IlConnectedFitness: public IlFitnessFunctionBase
{
double d_multiple;
public:
  IlConnectedFitness( int multiple ):d_multiple(multiple) {};
  double operator () (double x) const;
};

class IlNotConnectedFitness: public IlFitnessFunctionBase
{
public:
  double operator () (double x) const;
};


double
IlConnectedFitness::operator () (double x) const
{
  double xMP2 = (x-1.0);
  xMP2 *= xMP2;
  xMP2 += 1.0;
  return( 0.1 * d_multiple * x / xMP2 );
}

double
IlNotConnectedFitness::operator () (double x) const
{
  if( fabs(x) < 1.0 )
    {
      return x*x;
    }
  else
    {
      return 1.0;
    }
  //  x -= 0.5;
  //  x = exp(-x) + 1.0;
  //  return( 1.0 / x );
}

class IlFitnessFunctionItem
{
  
  int d_multiple;
  IlFitnessFunctionBase *d_func;

public:
  IlFitnessFunctionItem():d_multiple(0),d_func(0) {};

  int operator ++ () {return (++d_multiple);};
  int operator ++ (int) {return (d_multiple++);};

  void make()
    {
      if( d_func )
	{
	  delete d_func;
	}
      if( d_multiple )
	{
	  d_func = new IlConnectedFitness(d_multiple);
	} else {
	d_func = new IlNotConnectedFitness;
	}
    };
  double operator () (double x) const {return( (*d_func)(x) ); };
};


class Chromosome: public vector<double>
{
  typedef vector<double> Ancestor;
public:
  Chromosome(long size):Ancestor(2*size) {};

  void crossOver( double lambda,
		  const Chromosome &mom, const Chromosome &dad );
  void mutate(double amplitude = 2.0);
  void scale(double factor);
};

//**********************************************************************
//*** HUOM!
//*** drand48() *EI* ole standardin mukainen funktio. Siisp‰ t‰m‰ ei
//*** k‰‰nny standardin mukaisilla k‰‰nt‰jill‰ (tai -ansi-flagilla).
//*** Seuraava on *KLUDGE* ja se pit‰isi korjata!
//**********************************************************************
double drand48(void);

void
Chromosome::mutate(double amplitude)
{
  for( iterator idx=begin() ; idx != end() ; ++idx)
    {
      (*idx) += amplitude * ( drand48() - 0.49999999);
    }
}

void
Chromosome::scale(double factor)
{
  for( iterator idx=begin() ; idx != end() ; ++idx)
    {
      double demi = 1 + exp(-(*idx)/factor);
      (*idx) = factor / demi ;
    }
}

#include <cassert>

void
Chromosome::crossOver( double lambda,
		       const Chromosome &mom, const Chromosome &dad )
{
  assert( (mom.size() == dad.size() && size() == mom.size() )
	  && "WHOOOW!! Variable length cromosomes" );
  const_iterator idxMom;
  const_iterator idxDad;
  iterator idxMe;
  for( idxMe=begin(), idxMom=mom.begin(), idxDad=dad.begin() ;
       idxMe != end() ;
       ++idxMe, ++idxMom, ++idxDad )
    {
      *idxMe = *idxMom * lambda + *idxDad * (1.0 - lambda ) ;
    }
};

inline
static long
aritmSeries(long i)
{
  return( (i*i - i) / 2 );
}


class IlFitnessFunction: public vector<IlFitnessFunctionItem>
{
  typedef vector<IlFitnessFunctionItem> Ancestor;
public:
  IlFitnessFunction(long stateCnt): Ancestor(aritmSeries(stateCnt)) {};

  void addTransition(long xidx, long yidx);
  void make();
  double operator () (const Chromosome &chrom) const;
private:
  double apply(double x, long xidx, long yidx) const;

};

double 
IlFitnessFunction::apply(double x, long xidx, long yidx) const
{
  long idx = aritmSeries(xidx) + yidx ;
  return( ((*this)[idx])(x) );
}


inline
static double
localMaxNorm(double x1, double y1,
	     double x2, double y2)
{
  return( max( fabs(x1-x2),fabs(y1-y2)));
}

double
IlFitnessFunction::operator () (const Chromosome &chrom) const
{
  long var_size = chrom.size() / 2;
  long fsize = 2*size();
  fsize += var_size;
  fsize -= var_size*var_size;
  assert( 0==fsize && "Not chromosome for this fitness function" );

  double retVal = 0.0;
  for( long xidx = 1 ; xidx < var_size ; ++xidx )
    {
      for( long yidx = 0; yidx < xidx ; ++yidx)
	{
	  long vidx = xidx+xidx;
	  long uidx = yidx+yidx;
	  retVal += apply(localMaxNorm(chrom[vidx],chrom[vidx+1],
				       chrom[uidx],chrom[uidx+1]),
			  xidx, yidx);
	  
	}
    }
  return( retVal );
}

void
IlFitnessFunction::make()
{
  for( iterator idx=begin(); idx != end() ; ++idx )
    {
      idx->make();
    }
}

void
IlFitnessFunction::addTransition(long xidx, long yidx)
{
  if( xidx == yidx ) return;
  if( xidx < yidx )
    {
      addTransition(yidx,xidx);
    }
  else
    {
      long idx = (xidx * xidx - xidx ) / 2  + yidx ;
      ((*this)[idx])++;
    }
}



template<class TYPE>
inline static void
localSwap(TYPE &x, TYPE &y)
{
  TYPE tmp = x;
  x=y;
  y=tmp;
}


template <class TYPE>
inline static void
localSelectBestOutThree(TYPE *objs, double *rank)
{
  int idx = 0;
  if( rank[1] > rank[2] )
    {
      idx = 1;
    }
  else
    {
      idx = 2;
    }
  if( rank[0] > rank[idx] )
    {
      return;
    }
  else
    {
      localSwap(objs[0],objs[idx]);
      localSwap(rank[0],rank[idx]);
    }
}

template <class TYPE>
inline static void
localSelectWorstOutThree(TYPE *objs, double *rank)
{
  int idx = 0;
  if( rank[1] < rank[2] )
    {
      idx = 1;
    }
  else
    {
      idx = 2;
    }
  if( rank[0] < rank[idx] )
    {
      return;
    }
  else
    {
      localSwap(objs[0],objs[idx]);
      localSwap(rank[0],rank[idx]);
    }
}


void
IlGeneticAlgorithForCoordinates(IlLTS::StateIterator statesBeg_I,
				IlLTS::StateIterator statesEnd_I,
				IlLTS::TransitionIterator start_I,
				IlLTS::TransitionIterator end_I)
{
  typedef map<state_t,long> IndexMap;
  IndexMap indexMap;

  // Let's make index from states
  long idxI;
  IlLTS::StateIterator  idxS;
  for( idxI = 0, idxS = statesBeg_I ;
       idxS != statesEnd_I;
       ++idxS, ++idxI )
    {
      indexMap.insert(make_pair(idxS->name(),idxI));
    }
  assert( static_cast<size_t>(idxI) == indexMap.size()
	  && "WHOOW !! Duplicate state names" );

  // Add transitions to fitness function
  IlFitnessFunction fitness(indexMap.size());
  long transCnt = 0;
  for( IlLTS::TransitionIterator tidx = start_I ;
       tidx != end_I ;
       ++tidx )
    {
      IndexMap::iterator uidx = indexMap.find(tidx->tail().name());
      IndexMap::iterator vidx = indexMap.find(tidx->head().name());
      assert( uidx != indexMap.end() && vidx != indexMap.end()
	      && "Names of endpoints are not in index !");
      if( uidx != vidx )
	{
	  fitness.addTransition(uidx->second,vidx->second);
	  ++transCnt;
	}
    }
  // Finalize fitness function
  fitness.make();


  // This is microbial genetic algorithm
  // Population consist of three individuals
  Chromosome *population[3];
  Chromosome *child[3];
  double     popFitness[3];
  double     childFitness[3];
  population[0] = new Chromosome(indexMap.size());
#if 0
  // Get solution from input to chromosome
  for( idxI = 0, idxS = statesBeg_I ;
       idxS != statesEnd_I;
       ++idxS, idxI+=2 )
    {
      (*(population[0]))[idxI] = idxS->coords().xCoord();
      (*(population[0]))[idxI+1] = idxS->coords().yCoord();
    }
#endif
  population[1] = new Chromosome(*(population[0]));
  population[2] = new Chromosome(*(population[0]));
  child[0]      = new Chromosome(indexMap.size());
  child[1]      = new Chromosome(indexMap.size());
  child[2]      = new Chromosome(indexMap.size());


  // Initial pupulation is created by mutating individuals several time
  // double delta = sqrt(indexMap.size()) / indexMap.size();
  for( int cnt = 0; cnt < 0 /* indexMap.size() / 2 */ ; ++cnt )
    {
      population[0]->mutate(0.1);
      population[1]->mutate(-0.5);
      population[2]->mutate(0.5);
    }
  popFitness[0] = fitness(*(population[0]));
  popFitness[1] = fitness(*(population[1]));
  popFitness[2] = fitness(*(population[2]));

  localSelectBestOutThree(population,popFitness);

  
  // counter counts non-profitable modifications. 
  for( int counter=0; counter < 3000 ; ++counter )
    {
      child[0]->crossOver(1.5*drand48()-0.249999999999,
		       *(population[0]),*(population[1]));
      child[1]->crossOver(1.5*drand48()-0.249999999999,
		       *(population[1]),*(population[2]));
      child[2]->crossOver(1.5*drand48()-0.249999999999,
		       *(population[2]),*(population[0]));
      if( drand48() < 0.8 )
        {
          child[0]->mutate(0.1);
        }
      if( drand48() < 0.8 )
        {
          child[1]->mutate(0.2);
        }
      if( drand48() < 0.8 )
        {
          child[2]->mutate(0.3);
        }
      childFitness[0]= fitness(*(child[0]));
      childFitness[1]= fitness(*(child[1]));
      childFitness[2]= fitness(*(child[2]));

      localSelectWorstOutThree(child,childFitness);

      localSwap( child[1],population[1]);
      localSwap( child[2],population[2]);
      localSwap( childFitness[1],popFitness[1]);
      localSwap( childFitness[2],popFitness[2]);

      double oldBest = popFitness[0];
      localSelectBestOutThree(population,popFitness);

      if( oldBest < popFitness[0] )
	{
	  if( popFitness[0] - oldBest > 0.001 )
	    {
	      cerr << "Parannus lˆytyi " << popFitness[0]
		   << " " << counter << "                                 \r"
		   << flush ;
	    }
	  else
	    {
	      cerr << counter << "   \r" << flush;
	    }
	  counter = 0;
	}
    }

  cerr << endl;
  // Return solution from best chromosome
  for( idxI = 0, idxS = statesBeg_I ;
       idxS != statesEnd_I;
       ++idxS, idxI+=2 )
    {
      idxS->coords() = Point((*(population[0]))[idxI],
			     (*(population[0]))[idxI+1]);
    }
  
}
@| @}



\chapter{Creating arrows for transitions}

There are few requirements how the arrows of transitions are drawn.
The most important one is that arrows are distinct from each other and
they clearly show how a state of the system is chanced. This
requirement is approximately fulfilled by selecting lines of
transition arrows in a way that
\begin{itemize}
\item lines do not cross the states
\item lines are as straight as possible
\item lines do not cross each other it they do not have to
\item crossings of lines are clear and states they connect are obvious.
\end{itemize}

@d Function for creating arrows of transitions
@{
bool
IlTransitionArrows
  (
  IlLTS::StateIterator nodesStart,
  IlLTS::StateIterator nodesEnd,
  IlLTS::TransitionIterator start_I,
  IlLTS::TransitionIterator end_I
  )@| @}
@d arrows.hh
@{
@<Function for creating arrows of transitions@>;
@| @}
@d arrows.cc
@{

@<Local functions for transitions routing@>
@<Checking node crossing@>
@<Departure angles for crossing line@>
@<Departure angles for transition line@>

@<Function for creating arrows of transitions@>
{
  @<Prepare the transtitions and datastructures for bend selection@>
  @<Assing bends to transition lines THIS IS EMPTY@>
  @<Beautify transitions arrows@>
  return( true );
}
@| @}
Local functions and classes should be enclosed into unnamed local
namespace, but namespaces are not implemented yet.



\section{Initializations for bend selection}

Algorithm for drawing arrows start from straight lines and assign
necessary bends so that lines go nicely between states. This
refinement process is divided into two phases. First long transitions
are routed between states and then connections of arrows to states are
made distinct. Unnecessary crossings of transitions are avoided by
handling transitions in length order starting from shortest one.


@d Prepare the transtitions and datastructures for bend selection
@{
  @<Create initial arrows as straight lines@>
  @<Make length ordered collection of lines of transitions@>
  @<Make set of node coordinates@>
@| @}


To make task of selecting bends easier, lines of transitions are
initialized and transitions are sorted according to they length.

@d Create initial arrows as straight lines
@{
for( IlLTS::TransitionIterator ii = start_I;
     ii != end_I;
     ++ii)
  {
    (*ii).line().push_front((*ii).tail().coords());
    (*ii).line().push_back((*ii).head().coords());
  } 
@| @}


Lines of transitions are ordered from short to long except loops which
are at very end.
@D Make length ordered collection of lines of transitions
@{
typedef vector<IlLTS::Transition *> TransitionPtrVector;
TransitionPtrVector transitions;
for( IlLTS::TransitionIterator ii = start_I;
     ii != end_I;
     ++ii )
  {
    transitions.push_back(&(*ii));
  }
sort(transitions.begin(),
     transitions.end(),
     IlLineLengthLess());
@| @}
@D Local functions for transitions routing
@{
class IlLineLengthLess
{
public:
  bool operator () (const IlLTS::Transition *a,
		    const IlLTS::Transition *b);
  bool operator () (const Line *a, const Line *b);
};


bool
IlLineLengthLess::operator () (const Line *a, const Line *b)
{
  double lenA = euclNormP2(*a);
  double lenB = euclNormP2(*b);

  if( lenA == lenB ) return( false );
  if( lenA == 0.0 ) return( false );
  if( lenB == 0.0 ) return( true );
  return( lenA < lenB );
}

bool
IlLineLengthLess::operator () (const IlLTS::Transition *a,
			       const IlLTS::Transition *b)
{
  double lenA = euclNormP2(a->line());
  double lenB = euclNormP2(b->line());

  if( lenA == lenB ) return( false );
  if( lenA == 0.0 ) return( false );
  if( lenB == 0.0 ) return( true );
  return( lenA < lenB );
}
@| @}


@d Make set of node coordinates
@{set<Point> grid;
for( IlLTS::StateIterator idx = nodesStart;
     idx != nodesEnd;
     ++idx ) {
    grid.insert(idx->coords());
    }
@| @}



\section{Adding extra bends}

For transitions of which length measured as max norm is greater than
one are only ones that might need extra bends. Bend selection is not
done but it is approximated by selecting other departure angle.
  
@d Checking node crossing
@{bool arrowCrossesNode( Line& trline, set<Point>& grid) {
    const double NODE_TO_LINE_DIST = 0.2;
    LineAlgorithm points(trline.front(),trline.back());
    LineAlgorithm endp(trline.back(),trline.back());
    for( ++points; points != endp ; ++points) {
	if( contains(grid,(*points)) &&
	    euclNorm(*points,
		     trline.front(),
		     trline.back()) < NODE_TO_LINE_DIST ) return true;
	}
    return false;
    }
@| @}

@d Departure angles for transition line
@{void anglesForTransition( Line& trline, set<Point>& grid,
			 double& beginAngle,
			 double& endAngle ) {

    endAngle = angleOfVector(trline.front()-trline.back());
    beginAngle   = angleOfVector(trline.back()-trline.front());

    if( arrowCrossesNode(trline,grid) ) {
	anglesForCrossingLine(trline, beginAngle, endAngle);
	}
}
@| @}


@d Departure angles for crossing line
@{
template <typename Number>
Number sign(Number n) {
    if( n < 0 ) return -1;
    if( n > 0 ) return 1;
    return 0;
}

#ifndef M_PI
#define M_PI 3.14159265359
#endif

void anglesForCrossingLine(Line& trline,
			   double& beginAngle,
			   double& endAngle ) {
    const double CROSS_SHIFT_ANGLE = M_PI / 10.0 ;

    Point vec = trline.back()-trline.front();
    double deltaX = vec.xCoord() ;
    double deltaY = vec.yCoord() ;
    double diff = fabs(deltaX)-fabs(deltaY);
    double signOfAngle = sign(deltaX) * sign(deltaY) * sign(diff);

    if( signOfAngle < 0 ) {
	beginAngle -= CROSS_SHIFT_ANGLE;
	endAngle -= CROSS_SHIFT_ANGLE;
	}
    else if( signOfAngle > 0 ) {
	beginAngle += CROSS_SHIFT_ANGLE;
	endAngle += CROSS_SHIFT_ANGLE;
	}
    else {
	beginAngle -= CROSS_SHIFT_ANGLE;
	endAngle += CROSS_SHIFT_ANGLE;
        }
}
@| @}

Lines of transitions
are approximated by selecting grid segments, which transition have to
cross. Each of these crossing points are potential bend points.

Grid segment is horizontal or vertical line connecting adjacent grid
points and it's length is one. So each grid point is adjacent to four
grid segments. It is trusted, that first point of grid segment is
lexicographically smaller than second.

@D Local functions for transitions routing
@{
typedef pair<Point,Point> GridSegment;
enum SegmentDirection
{
  DIR_LEFT,
  DIR_UP,
  DIR_RIGHT,
  DIR_DOWN,
  DIR_LAST
};

#include <cassert>

static GridSegment
makeSegment(Point p, int direc)
{
  switch( direc )
  {
  case DIR_LEFT:
    return( make_pair( p+Point(-1.0,0.0), p ) );
  case DIR_UP:
    return( make_pair( p+Point(0.0,-1.0), p) );
  case DIR_RIGHT:
    return( make_pair( p, p+Point(1.0,0.0) ) );
  case DIR_DOWN:
    return( make_pair( p, p+Point(0.0,1.0) ) );
  default:
    assert( false && "Illigal grid segment direction" );
    return( make_pair( p,p));
  }
}

inline bool
horizontalSegment( const GridSegment &seg)
{
  return( seg.first.yCoord() == seg.second.yCoord());
}

inline bool
verticalSegment( const GridSegment &seg)
{
  return( seg.first.xCoord() == seg.second.xCoord());
}
@}


Class \texttt{CrossPoint} is placeholder for bend point as decision
variable. This structure contains other information used in
constraints too.
@d Local functions for transitions routing
@{
class CrossPoint
{
public:
  CrossPoint
    (
    const GridSegment &seg,
    double crossingDelta
    ): segment(seg),
       naturalCrossPoint(crossingDelta),
       realCrossPoint(0.0)
  {
  };

  bool
  operator <
    (
    const CrossPoint &o
    )
  {
    return (naturalCrossPoint < o.naturalCrossPoint) ;
  };
  
public:
  GridSegment segment;
  double      naturalCrossPoint;
  double      realCrossPoint;
};
@| @}

Potential bend points are found by travelling along approximation of
transition array and they are handled with to containers. First one
contains crossings by transition.
@d Prepare the transtitions and datastructures for bend selection
@{
typedef map<Line *, vector< CrossPoint > > BendContainer;
BendContainer additionalBendsOf;
@| additionalBendsOf @}
This container also owns the crossing points.

The other one contains only references into the first container. This
container is organized by grid segment, since actual bends are seleted
for one grid segment at a time.
@d Prepare the transtitions and datastructures for bend selection
@{
typedef map<GridSegment, vector< CrossPoint *> > BendsByGridSegment;
BendsByGridSegment crossesOf;
@| crossesOf @}
@d Inserting new grossing point for \texttt{trline} and \texttt{segment}
@{{
  double delta = crossingFractionInP( segment.first,
                                      segment.second,
                                      trline.front(),
                                      trline.back());
  additionalBendsOf[&trline].push_back(CrossPoint(segment, delta));
  crossesOf[segment].push_back( &(additionalBendsOf[&trline].back()) );
}
@| @}



Only transitions which length measured as maximum norm is greater than
one can go too close to states and cross grid segments. 
@D Prepare the transtitions and datastructures for bend selection
@{
typedef map<Point, set<Line *> > CrossedMap;
typedef map<GridSegment, set<Line *> > TunnelMap;
CrossedMap crossed;
TunnelMap  tunnel;
for( IlLTS::StateIterator ii = nodesStart ;
     ii != nodesEnd ;
     ++ ii )
  {
    crossed[ii->coords()];
  }
TransitionPtrVector::iterator longStart = transitions.begin();
for( ; longStart != transitions.end() && maxNorm((*longStart)->line()) < 1.1;
     ++ longStart )
  ;
// TransitionPtrVector::iterator longBegin=longStart;
for( ; longStart != transitions.end() && maxNorm((*longStart)->line()) > 1.0;
     ++ longStart )
  {
    @<Insert long transition line into tunnel set@>
  }
// TransitionPtrVector::iterator longEnd=longStart;
@| crossed tunnel @}

To find out which grid segment each transition crosses, the line is
approximated with line drawing algorithm, which gives enough points to
find out all crossed segments.
@D Insert long transition line into tunnel set
@{
Line &trline = (*longStart)->line();
LineAlgorithm points(trline.front(), trline.back());
LineAlgorithm endp(trline.back(),trline.back());
for( ++points; points != endp ; ++points)
  {
    if( points.exact() )
      {
	@<Select crossed grid segments using right side driving rule@>
      }
    else
      {
	@<Test all four grid segments as candidate for crossing@>
      }
  }
@| trline @}

If transition crosses state exactly, the grid segments are selected
so, that transition passes state from right.
@D Select crossed grid segments using right side driving rule
@{
if( contains(crossed,*points) )
  {
    double deltaX = trline.back().xCoord() - trline.front().xCoord();
    double deltaY = trline.back().yCoord() - trline.front().yCoord();
    if( deltaX <= -0.1 )
      {
	if( deltaY <= -0.1 )
	  {
            {
              GridSegment segment = makeSegment(*points,DIR_UP);
              @<Inserting new grossing point for \texttt{trline} and \texttt{segment}@>
            }
            {
              GridSegment segment = makeSegment(*points,DIR_RIGHT);
              @<Inserting new grossing point for \texttt{trline} and \texttt{segment}@>
            }
	  }
	else if( deltaY >= 0.1 )
	  {
            {
              GridSegment segment = makeSegment(*points,DIR_UP);
              @<Inserting new grossing point for \texttt{trline} and \texttt{segment}@>
            }
            {
              GridSegment segment = makeSegment(*points,DIR_LEFT);
              @<Inserting new grossing point for \texttt{trline} and \texttt{segment}@>
            }
	  }
	else
	  {
            {
              GridSegment  segment = makeSegment(*points,DIR_UP);
              @<Inserting new grossing point for \texttt{trline} and \texttt{segment}@>
            }
	  }
      }
    else if( deltaX >= 0.1 )
      {
	if( deltaY <= -0.1 )
	  {
            {
              GridSegment segment = makeSegment(*points,DIR_DOWN);
              @<Inserting new grossing point for \texttt{trline} and \texttt{segment}@>
            }
            {
              GridSegment segment = makeSegment(*points,DIR_RIGHT);
              @<Inserting new grossing point for \texttt{trline} and \texttt{segment}@>
            }
	  }
	else if( deltaY >= 0.1 )
	  {
	    tunnel[makeSegment(*points,DIR_DOWN)].insert(&trline);
	    tunnel[makeSegment(*points,DIR_LEFT)].insert(&trline);
	  }
	else
	  {
	    tunnel[makeSegment(*points,DIR_DOWN)].insert(&trline);
	  }
      }
    else
      {
	if( deltaY <= -0.1 )
	  {
	    tunnel[makeSegment(*points,DIR_RIGHT)].insert(&trline);
	  }
	else if( deltaY >= 0.1 )
	  {
	    tunnel[makeSegment(*points,DIR_LEFT)].insert(&trline);
	  }
	else
	  {
	    assert(false && "Loops are not allowed here");
	  }
      }
  }
@| @}
@D Local functions for transitions routing
@{
template <class SetOrMap, class Key>
inline bool
contains(const SetOrMap &cont, const Key &key)
{
  return( cont.end() != cont.find(key) );
}
@| contains @}

If line of transition naturally crosses grid segmet, it is easy to
find using algorithm for checking line crossings.
@D Test all four grid segments as candidate for crossing
@{
for( int direc=DIR_LEFT; direc < DIR_LAST; ++direc )
  {
    GridSegment seg=makeSegment(*points,direc);
    if( linesCross(seg.first, seg.second,trline.front(),trline.back()) )
      {
	tunnel[seg].insert(&trline);
      }
  }
@| @}

For each line in \verb<tunnels< there is a set of transitions crossing
line. If transition crosses state, it is added to sets of those lines,
where it naturally belongs (right side driving) or where is room for
it and where it crosses as few transitions as possible.

For each transition in transition sets of lines in \verb<tunnels<,
there is point telling where exactly transitions crosses that line.
These points are selected so that available space is evenly
distributing and crossings of transitions are avoided. These points
are added to transitions as bend points if necessary.

Now information about long lines and grid segments they cross is
collected. Now this information have to be transfed into form, where
bends are easy to select.
@D Assing bends to transition lines
@{
typedef map<GridSegment, set<Point> > OccupyMap;
typedef map<Line *, set<GridSegment> > CrosspointMap;
OccupyMap occupied;
CrosspointMap crossings;
for( TunnelMap::iterator ii = tunnel.begin() ;
     ii != tunnel.end();
     ++ii)
  {
    const GridSegment &seg = (*ii).first;
    set<Line *> &lines = (*ii).second;
    for( set<Line *>::iterator jj = lines.begin();
	 jj != lines.end();
	 ++jj )
      {
	crossings[*jj].insert(seg);
        occupied[seg];
      }
  }
@| @}

Map \verb<occupied< contains set of used crossing points for each grid
segment. This map is initialized by inserting points, which prevent
lines to go too close to states.
@D Assing bends to transition lines
@{
const double MIN_LINE_TO_LINE_DIST = 0.1;
const double MIN_LINE_TO_NODE_DIST = 0.3;
for( OccupyMap::iterator ii = occupied.begin();
     ii != occupied.end() ;
     ++ii)
  {
    const GridSegment &seg = ii->first;
    set<Point> &pointset = ii->second;
    if( contains(crossed, seg.first) )
      {
	Point p = seg.first;
	for( double dist = 0.0;
	     dist < MIN_LINE_TO_NODE_DIST ;
	     dist += MIN_LINE_TO_LINE_DIST )
	  {
	    pointset.insert(p);
	    p.move(MIN_LINE_TO_LINE_DIST,seg.second);
	  }
      }
    if( contains(crossed, seg.second) )
      {
	Point p = seg.second;
	for( double dist = 0.0;
	     dist < MIN_LINE_TO_NODE_DIST ;
	     dist += MIN_LINE_TO_LINE_DIST )
	  {
	    pointset.insert(p);
	    p.move(MIN_LINE_TO_LINE_DIST,seg.first);
	  }
      }
  }
@| MIN_LINE_TO_LINE_DIST MIN_LINE_TO_NODE_DIST  @}

Actual bends are selected first for shorer transitions, which prevent
avoidable crossings.
@D Assing bends to transition lines
@{
for( TransitionPtrVector::iterator ii = longBegin;
     ii != longEnd;
     ++ii)
  {
    Line &trline = (*ii)->line();
    set<GridSegment> &segs = crossings[&trline];
    for( set<GridSegment>::iterator jj = segs.begin();
         jj != segs.end();
         ++jj )
      {
        const GridSegment &seg = *jj;
        set<Point> &reserved = occupied[seg];
        insertBendToLine(trline, seg, reserved, MIN_LINE_TO_LINE_DIST );
      }
  }
@}

In bend selection from line is selected place, where line most
naturally crosses segment. This point is moved as little as possible
to place where is room for it.
@D Local functions for transitions routing NO THIS
@{

template <class T>
inline bool
inBetween(T p0, T p1, T a)
{
  return( ( p0 < a && a < p1 ) || ( p0 > a && a > p1 ) );
}

static void
insertBendToLine(Line &line, const GridSegment &seg,
		 set<Point> &reserved, double min_line_distance)
{
  if( horizontalSegment( seg ) )
    {
      Line::iterator pos0 = line.begin();
      Line::iterator pos1 = ++line.begin();
      for(  ; pos1 != line.end() ; ++pos0, ++pos1 )
	{
	  if( inBetween( pos0->yCoord(), pos1->yCoord(), seg.first.yCoord()) )
	    break;
	}
      assert( pos1 != line.end()
	      && "Interesting, line do NOT cross grid segment" );
      double deltaX = pos1->xCoord() - pos0->xCoord();
      double deltaY = pos1->yCoord() - pos0->yCoord();
      double coff = (seg.first.yCoord() - pos0->yCoord() ) / deltaY;
      Point created(pos0->xCoord() + coff * deltaX, seg.first.yCoord());
      if( created < seg.first ) created = seg.first ;
      if( created > seg.second ) created = seg.second ;
      if( ! reserved.empty() )
	{
	  set<Point>::iterator start = reserved.lower_bound(created);
          Point lower = created;
          bool lower_valid = false;
          Point upper = created;
          bool upper_valid = false;

          set<Point>::iterator idx = start;
	  while( idx != reserved.end()
		 && idx->xCoord() - upper.xCoord() < min_line_distance )
	    {
	      upper.xCoord(idx->xCoord() + min_line_distance);
	      ++idx;
	    }
	  if( idx != reserved.end()
	      || seg.second.xCoord()-upper.xCoord() >= 0.0 )
	    {
	      upper_valid = true;
	    }
	  idx=start;
	  if( idx != reserved.end() )
	    {
	      lower.xCoord(idx->xCoord() - min_line_distance );
	    }
	  else
	    {
	      lower = seg.second;
	    }
	  while( idx != reserved.begin() )
	    {
	      --idx;
	      if( lower.xCoord() - idx->xCoord() >= min_line_distance )
		break;
	      lower.xCoord( idx->xCoord() - min_line_distance );
	    }
          if( lower.xCoord() >= seg.first.xCoord() )
            lower_valid = true;

          assert( (lower_valid || upper_valid)
                  && "No room for long transition" );
          if( ! lower_valid )
	    {
	      created = upper;
	    }
	  else if( ! upper_valid )
	    {
	      created = lower;
	    }
          else if( created.xCoord() - lower.xCoord()
		   <= upper.xCoord() - created.xCoord() )
            {
	      created = lower;
	    }
	  else
	    {
	      created = upper;
	    }
	}
      reserved.insert(created);
      line.insert(pos1,created);
    }
  else
    {
      Line::iterator pos0 = line.begin();
      Line::iterator pos1 = ++line.begin();
      for(  ; pos1 != line.end() ; ++pos0, ++pos1 )
	{
	  if( inBetween( pos0->xCoord(), pos1->xCoord(), seg.first.xCoord()) )
	    break;
	}
      assert( pos1 != line.end()
	      && "Interesting, line do NOT cross grid segment" );
      double deltaX = pos1->xCoord() - pos0->xCoord();
      double deltaY = pos1->yCoord() - pos0->yCoord();
      double coff = (seg.first.xCoord() - pos0->xCoord() ) / deltaX;
      Point created(seg.first.xCoord(), pos0->yCoord() + coff * deltaY);
      if( created < seg.first ) created = seg.first ;
      if( created > seg.second ) created = seg.second ;
      if( ! reserved.empty() )
	{
	  set<Point>::iterator start = reserved.lower_bound(created);
          Point lower = created;
          bool lower_valid = false;
          Point upper = created;
          bool upper_valid = false;

          set<Point>::iterator idx = start;
	  while( idx != reserved.end()
		 && idx->yCoord() - upper.yCoord() < min_line_distance )
	    {
	      upper.yCoord(idx->yCoord() + min_line_distance);
	      ++idx;
	    }
	  if( idx != reserved.end()
	      || seg.second.yCoord()-upper.yCoord() >= 0.0 )
	    {
	      upper_valid = true;
	    }
	  idx=start;
	  if( idx != reserved.end() )
	    {
	      lower.yCoord(idx->yCoord() - min_line_distance );
	    }
	  else
	    {
	      lower = seg.second;
	    }
	  while( idx != reserved.begin() )
	    {
	      --idx;
	      if( lower.yCoord() - idx->yCoord() >= min_line_distance )
		break;
	      lower.yCoord( idx->yCoord() - min_line_distance );
	    }
          if( lower.yCoord() >= seg.first.yCoord() )
            lower_valid = true;

          assert( (lower_valid || upper_valid)
                  && "No room for long transition" );
          if( ! lower_valid )
	    {
	      created = upper;
	    }
	  else if( ! upper_valid )
	    {
	      created = lower;
	    }
          else if( created.yCoord() - lower.yCoord()
		   <= upper.yCoord() - created.yCoord() )
            {
	      created = lower;
	    }
	  else
	    {
	      created = upper;
	    }
	}
      reserved.insert(created);
      line.insert(pos1,created);
    }
}
@}

\section{Adding normal bends}

Two bend points are added at both ends of each arrow line of transitions.
They keep transitions distinct at state.

First any other transition except loops are insert to surrounding ring
which serve as auxiliary data structure for bend point
selection. Loops are not inserted because they do not fit into used
optimization model.
@D Beautify transitions arrows
@{
typedef map<state_t, IlTransitionRing> SurrMap;
SurrMap stateSurroundings;


TransitionPtrVector::iterator curTransition;
for( curTransition = transitions.begin();
     curTransition != transitions.end() ;
     ++ curTransition)
  {
    IlLTS::Transition &tr = * * curTransition;
    IlLTS::State &head=tr.head();
    IlLTS::State &tail=tr.tail();
    if( head == tail )
      {
	//% Loops are at end of orderedTransitions -vector
	break;
      }
    IlTransitionRing &headRing = stateSurroundings[head.name()];
    IlTransitionRing &tailRing = stateSurroundings[tail.name()];

    double headAngle;
    double tailAngle;
    anglesForTransition(tr.line(), grid, tailAngle, headAngle);
    headRing.insertAfter(headAngle,tr,tail);
    tailRing.insertBefore(tailAngle,tr,head);
  }
@}

In next stage linear optimization method is used to select most
suitable bends.
@D Beautify transitions arrows
@{
for( SurrMap::iterator ii=stateSurroundings.begin();
     ii != stateSurroundings.end();
     ++ii)
  {
    IlTransitionRing &ring= (*ii).second;
    //% !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if( ring.optimize(M_PI/8.0) )
      {
	// cerr << "Optimoitu solmulle " << (*ii).first << endl;
      }
    else
      {
        // cerr << "Ei k‰yp‰ solmulle " << (*ii).first << endl;
      }
  }
@}

Loops are added there where is enough space.
@D Beautify transitions arrows
@{
//% curTransition points still to first loop
for(   ;
     curTransition != transitions.end() ;
     ++ curTransition)
  {
    IlLTS::Transition &tr = * * curTransition;
    IlLTS::State &head=tr.head();
    IlLTS::State &tail=tr.tail();
    if( !(head == tail) )
      {
	//% !!!!!!!!!!!!!!!!!!!!
        //% Uh! sombody is fooling me
	return( false );
      }
    //% To get first and last two point from line representing
    //% transition. 
    IlTransitionRing &headRing = stateSurroundings[head.name()];

    //% !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    headRing.addLoop(tr, M_PI / 12.0);
  }
@}

At the last stage bends are added to lines representing transitions.
@D Beautify transitions arrows
@{
for( map<state_t, IlTransitionRing>::iterator ii=stateSurroundings.begin();
     ii != stateSurroundings.end();
     ++ii)
  {
    IlTransitionRing &ring= (*ii).second;
    state_t           stateName = (*ii).first;
    // cerr << "solmun " << stateName << " viivat" << endl;
    //% !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    for( IlTransitionRing::Iterator jj = ring.begin();
	 jj != ring.end();
	 ++jj)
      {
	double angle=(*jj).getAngle();
        const double FIRST_BEND_RADIUS = 0.3;
	Point toAdd(constProduct(FIRST_BEND_RADIUS,
				 Point(cos(angle),sin(angle))));
        IlLTS::Transition &tr = (*jj).transition();
        if( tr.head().name() == stateName )
	  {
	    Point p = tr.head().coords() + toAdd ;
	    Line::iterator pos=tr.line().end();
	    pos--;
	    tr.line().insert(pos,p);
	  }
	else
	  {
	    Point p = tr.tail().coords() + toAdd ;
	    Line::iterator pos=tr.line().begin();
	    pos++;
	    tr.line().insert(pos,p);
	  }
      }
  }
@| FIRST_BEND_RADIUS @}



\chapter{Auxiliary datastructures for layout algorithm}

\section{Grid helps with constraints}\label{sec:grid}

Grid is used to answer questions is given coordinate pair free and if
not which state is in those coordinates. This class is auxiliary tool
which implements low level solution modification operators for local
search optimization method (can be used with simulated annealing too)

@d Class IlGrid
@{
class IlGrid
{
@<Operational interface of class IlGrid@>
@<Iplementation of grid datastructures@>
};
@| @}


States are located in grid. Basic grid oprations are
\begin{itemize}
\item Tell whether given grid point is free or not.
\item If grid point is occupied, tell by which.
\item Swap positions of two states.
\item Move state to given location (only if location is free).
\item Undo last swap or move operation.
\end{itemize}
To keep things simple, grid class updates the graphical information of state.
@d Operational interface of class IlGrid
@{
public:
  bool isFree(int x, int y) const;
  IlLTS::State &content(int x, int y);
  bool swapStates(IlLTS::State &u, IlLTS::State &v);
  bool moveState(IlLTS::State &s, int x, int y);
  bool undoLast();
@| @}
For making initial solution states have to be put into grid
@d Operational interface of class IlGrid
@{
  bool placeState(IlLTS::State &s, int x, int y);
@| @}

@d Operational interface of class IlGrid
@{
  void reInit();
@| @}


As basic level grid is two dimensional array of pointers to states.
@d Iplementation of grid datastructures
@{
private:
  typedef IlLTS::State *state_ptr;
  typedef vector<IlLTS::State *> grid_row;
  typedef vector<grid_row> grid_array;
  grid_array grid;
@| @}
In old version of illustrator grid size is fixed. This makes
implementation somehow more robust, but it might have bad influence to
result, at least then state count grow bigger. In this version square
grid of size \(N\times N\) is used and \(N\) is number of states. The
idea comes from article \cite{s-dgrg-95}, where is defined the upper
limit of needed area to draw a graph into the grid.
@d Iplementation of grid datastructures
@{
public:
  IlGrid(int N);
  IlGrid();
@| @}

In order to implement undo operation, mechanism for saving solution is
needed. In this case saving one modification is sufficient.
@d Iplementation of grid datastructures
@{
public:
  class BeforeModification;
private:
  BeforeModification *mark;
@| @}
Implementation of these internals is hiden into the implementations file
@D Implementing auxliary datastructure for undo operation
@{
class IlGrid::BeforeModification
{
public:
  virtual void restore(IlGrid &) = 0;
};

class SwapMod: public  IlGrid::BeforeModification
{
public:
  SwapMod(IlLTS::State &u, IlLTS::State &v): s1(&u),s2(&v) {};
private:
  IlLTS::State *s1;
  IlLTS::State *s2;
public:
  void restore(IlGrid &g) { g.swapStates(*s1,*s2); };
};

class MoveMod: public  IlGrid::BeforeModification
{
public:
  MoveMod(IlLTS::State &u, int org_x, int org_y): s(&u), x(org_x), y(org_y) {};
private:
  IlLTS::State *s;
  int x;
  int y;
public:
  void restore(IlGrid &g) { g.moveState(*s,x,y); }; 
};
@| @}

@D Implementation of basic operations of IlGrid
@{
bool
IlGrid::isFree(int x, int y) const
{
  if( x < 0 ) return false;
  if( y < 0 ) return false;
  if( (unsigned)x >= grid.size() ) return false ;
  if( (unsigned)y >= grid.size() ) return false ;

  return 0 == grid[x][y];
}

#include <cassert>

IlLTS::State &
IlGrid::content(int x, int y)
{
  assert(    (x>=0)
	  && (y>=0)
	  && ((unsigned)x < grid.size())
	  && ((unsigned)y < grid.size())
	  && (grid[x][y] != 0)
	  && "There is not state in given position" );
  return( *grid[x][y] );
}


bool
IlGrid::swapStates(IlLTS::State &u, IlLTS::State &v)
{
  if( 0 != mark ) delete mark;
  mark = new SwapMod(u,v);
  Point tmp= u.coords();
  u.coords()=v.coords();
  v.coords()=tmp;

  grid[u.Xcoord()][u.Ycoord()] = &u;
  grid[v.Xcoord()][v.Ycoord()] = &v;
  return( true );
}

bool
IlGrid::moveState(IlLTS::State &s, int x, int y)
{
  if( x < 0 ) return false;
  if( y < 0 ) return false;
  if( (unsigned)x >= grid.size() ) return false ;
  if( (unsigned)y >= grid.size() ) return false ;

  if( 0 != grid[x][y] )
    {
      return( false );
    }

  if( 0 != mark )
    {
      delete mark;
    }

  mark = new MoveMod(s,s.Xcoord(),s.Ycoord());
  grid[s.Xcoord()][s.Ycoord()] = 0;

  s.coords() = Point(x,y);

  grid[s.Xcoord()][s.Ycoord()] = &s;
  return( true );
}

bool
IlGrid::undoLast()
{
  if( 0 == mark )
    {
      return( false );
    }
  BeforeModification *oldmark =  mark;
  mark = 0;
  oldmark ->restore(*this);
  delete oldmark;
  return( true );
}

bool
IlGrid::placeState(IlLTS::State &s, int x, int y)
{
  if( 0 != grid[x][y] )
    {
      return( false );
    }
  s.coords()=Point(x,y);

  grid[s.Xcoord()][s.Ycoord()] = &s;
  return( true );
}

IlGrid::IlGrid(int N)
: grid(N,grid_row(N,(IlLTS::State *)0)),mark(0)
{
}

void
IlGrid::reInit()
{
  for( long idx=0; static_cast<size_t>(idx) < grid.size() ; ++idx)
    {
      for( long jdx=0; static_cast<size_t>(jdx) < grid.size() ; ++jdx )
	{
	  grid[idx][jdx] = 0;
	}
    }
}

IlGrid::IlGrid()
{
}

@| @}

@d grid.hh
@{
@<Class IlGrid@>
@| @}

@d grid.cc
@{
@<Implementing auxliary datastructure for undo operation@>
@<Implementation of basic operations of IlGrid@>
@| @}


\section{Optimize surroundings of states}\label{sec:angles}
@d surround.hh
@{
@<Definition of class EdgeRing@>
@| @}
@d surround.cc
@{@}


@D Definition of class EdgeRing
@{
class IlTransitionRingItem
{
public:
  IlTransitionRingItem(double angle,
		       IlLTS::Transition &trans,
		       IlLTS::State      &other)
    :d_transition(trans),
     d_otherEnd(other),
     d_naturalAngle(angle),
     d_realAngle(angle)
  {};

    // T‰m‰ on `feikki'-sijoitusoperaattori k‰‰nt‰j‰‰ varten!
    // ?? Mit‰ mahtaa tapahtua ilman t‰t‰?
    IlTransitionRingItem& operator=( const IlTransitionRingItem& rhs )
    {
        if ( &rhs != this )
        {
            d_naturalAngle = rhs.d_naturalAngle;
            d_realAngle = rhs.d_realAngle;
        }

        return *this;
    }

  bool operator < (const IlTransitionRingItem &o)
    { return( d_naturalAngle < o.d_naturalAngle ); };
  bool operator > (const IlTransitionRingItem &o)
    { return( d_naturalAngle > o.d_naturalAngle ); };
  
  double getAngle() const       { return( d_realAngle ); };
  double naturalAngle() const   { return( d_naturalAngle ); };
  void   setAngle(double angle) { d_realAngle = angle; };

  IlLTS::Transition &transition() {return d_transition; };
  IlLTS::State      &otherEnd()   {return d_otherEnd;   };
private:
  IlLTS::Transition &d_transition;
  IlLTS::State      &d_otherEnd;
  double             d_naturalAngle;
  double             d_realAngle;
};

class IlTransitionRing: public list<IlTransitionRingItem>
{
  typedef list<IlTransitionRingItem> List;
public:
  typedef list<IlTransitionRingItem>::iterator Iterator;
  typedef IlTransitionRingItem Item;
  typedef Item *ItemPtr;

  void insertAfter( double angle,
		    IlLTS::Transition &trans,
		    IlLTS::State &other)
  {
    Item item(angle,trans,other);
    Iterator ii;
    for( ii = begin();	 (ii != end()) && !( *ii > item) ;  ++ii)
      {;}
    insert( ii , item );
  };
  void insertBefore( double angle,
		     IlLTS::Transition &trans,
		     IlLTS::State &other )
  {
    Item item(angle,trans,other);
    Iterator ii;
    for( ii = begin();	 (ii != end()) && ( *ii < item ) ;  ++ii)
      {;}
    insert( ii , item );
  };

  bool addLoop(IlLTS::Transition &trans, double minAngle);

  bool optimize(double minAngle);
};
@| @}

\subsection{Adding loops to state surrounding ring}

@d surround.cc
@{
@<Implementation of function IlTransitionRing::addLoop@>
@| @}

@D Implementation of function IlTransitionRing::addLoop
@{

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#include <cassert>

bool
IlTransitionRing::addLoop(IlLTS::Transition &trans, double minAngle)
{
  if( begin()==end() )
    {
      //% List is empty
      Item one(0.0, trans, trans.head());
      Item other(minAngle, trans, trans.tail());
      push_back(one);
      push_back(other);
    }
  else
    {
      Iterator current = end();
      current --;
      double lastAngle = (*current).getAngle() - 2*M_PI ;
      bool found = false;
      for( current = begin() ;
	   current != end() ;
	   ++current )
	{
	  if( (*current).getAngle() - lastAngle >= 3*minAngle )
	    {
	      found = true;
	      break;
	    }
	  else
	    {
	      lastAngle = (*current).getAngle();
	    }
	}
      if( found )
	{
	  Item one(lastAngle + minAngle, trans, trans.head());
	  Item other(lastAngle + minAngle + minAngle, trans, trans.tail());
	  current = insert(current, other);
	  current = insert(current, one);
	}
      else
	{
	  assert( found && "No room for loop. Sorry" );
	}
    }
  return( true );
}
@| @}



\subsection{Optimization of departure angles}

@d surround.cc
@{
@<Implementation of function OptimizeEdgeRing@>
@| @}

@d Implementation of function OptimizeEdgeRing
@{
bool
IlTransitionRing::optimize(double minAngle)
{
@| @}
This function is guaranteed to return solution even if returns
\verb|false|. However in that case solution may be odd.

Optimimization of departure angles is linear programming problem, that
can be solved easily with \verb|lp_solve|. The objective is to
minimize sum of absolute differences of real and natural angles. So
decision variables are differences and divided to positive and
negative parts
\[
  \Delta\alpha_i = y_i - z_i
\]
The constraints tell not to change the order of edges and to keep
edges distinct. Let \(\alpha\) be the natural angle and \(\gamma\)
minimal difference of two edges
\[
  \alpha_i + \Delta\alpha_i + \gamma \leq \alpha_{i+1} + \Delta\alpha_{i+1}
\]
and between last and first edge of ring
\[
  \alpha_n + \Delta\alpha_n + \gamma \leq 2 \pi + \alpha_{1} + \Delta\alpha_{1}
\]

This is modeleted as LP
\begin{eqnarray*}
  \min & & \sum_i y_i + \sum_i z_i \\
    & & \textrm{Ehdolla:} \\
    & &
   \left[\begin{array}{ccccccccccc}
       1 & -1 & -1 & 1 & 0 & 0 & 0 & 0 & \cdots & 0 & 0 \\
       0 & 0 & 1 & -1 & -1 & 1 & 0 & 0 & \cdots & 0 & 0 \\
       0 & 0 & 0 & 0 & 1 & -1 & -1 & 1 & \cdots & 0 & 0 \\
         &   &   &   &   &    &    &   & \ddots &   &   \\
       -1 & 1 & 0 & 0 & 0 & 0 & 0 & 0 & \cdots & 1 & -1
   \end{array}\right]
   \left[
     \begin{array}{c}
       y_1 \\
       z_1 \\
       y_2 \\
       z_2 \\
       y_3 \\
       z_3 \\
       y_4 \\
       z_4 \\
       \vdots \\
       y_n \\
       z_n
     \end{array}
   \right] \leq
\left[
     \begin{array}{c}
       \alpha_2 - \alpha_1 - \gamma \\
       \alpha_3 - \alpha_2 - \gamma \\
       \alpha_4 - \alpha_3 - \gamma \\
       \vdots \\
       2 \pi + \alpha_1 - \alpha_n - \gamma \\
     \end{array}
   \right]
\end{eqnarray*}
However this model has one small problem. If there is two edges
between same pair of nodes and no other edges interfering, only one of
the edges is moved compared to it's natural
angle.\marginpar{This is not implemented. Have strange behaviour too.}
This can be solved
by introdusing one auxliary variable per decision variable. Each
auxliary variable has positive cofficient in objective function and if
decision variable become big enougth, auxliary variable must be
positive.

The standard method for decenerasy is used instend. This needs
modifications all round the code.\marginpar{Don`t work any better}

The coefficient part of EdgeRingElement is replaced by node at other
end of edge. This information is used to group edges.

First input information is collected
@D Implementation of function OptimizeEdgeRing
@{
  bool     feasible;
  int      numOfEdges = size();
  // muuttuja(numOfEdges);
  double   jako = 2.0*M_PI/numOfEdges;
  feasible = jako > minAngle;

  auto_vec<double> alpha = new double [numOfEdges];
  auto_vec<int> lkm = new int [numOfEdges];
  auto_vec<ItemPtr> alkiot= new ItemPtr [numOfEdges];
//  list_item it;
  int gidx=0;
  int varidx=0;
  for(Iterator it=begin();
      it != end();
      ++it)
    {
      alkiot[gidx]=&(*it);
      if( (varidx > 0)
	  && alkiot[gidx-1]->otherEnd() == alkiot[gidx]->otherEnd()
	  && alkiot[gidx-1]->naturalAngle() == alkiot[gidx]->naturalAngle() )
	{
	  lkm[varidx-1] += 1;
	}
      else
	{
	  alpha[varidx]=alkiot[gidx]->naturalAngle();
	  lkm[varidx]=1;
	  ++varidx;
	}
      ++gidx;
    }
@}
Remember. Items in vector \verb|alkiot| are direct pointers to ring's items
and they are in same order as decision variables.

If there is too many edges as compared to \verb|minAngle|, problem is
infeasible. To get reasonable solution, the \verb|minAngle| is made
small enough.
@d Implementation of function OptimizeEdgeRing
@{
  if( !feasible )
    {
      minAngle = jako / 2.0;
    }
@| @}

@D Implementation of function OptimizeEdgeRing
@{
  // muuttuja(varidx);
  int numOfDesVar = 2 * varidx;
  int numOfVar = numOfDesVar;
  int numOfConstr = varidx;
  auto_vec<double> row = new double [numOfVar];

@}

If there is at most one variable pair, there is no need for optimization
@D Implementation of function OptimizeEdgeRing
@{
  if( numOfDesVar > 2 ) 
    {
      @<Do actual optimization@>
    }
  else
    {
      if( numOfDesVar == 2 )
	{
	  row[0]=row[1]=0.0;
	}
      else
	{
	  return(feasible);
	}
      
    }
@| @}


The next step is to build LP-problem
@D Do actual optimization
@{

  lprec *prob = make_lp(numOfConstr, numOfVar);

  for( int idx = 0 ; idx < numOfVar ; ++idx )
    {
      row[idx]=0.0;
    }

  // Adding constraints for decision variables
  row[0]=row[3]=1.0;
  row[1]=row[2]=-1.0;
  for( int idx = 0 ; idx < numOfConstr -2 ; ++idx )
    {
      add_constraint(prob, row.get()-1, LE ,
		     alpha[idx+1]
                      -alpha[idx]
                      -(lkm[idx+1]+lkm[idx])*minAngle/2.0 );
      for( int jdx = 2*idx + 5 ;
	   jdx >= 2*idx + 2;
	   --jdx )
	{
	  row[jdx]=row[jdx-2];
	  row[jdx-2]=0.0;
	}
    }
  add_constraint(prob, row.get()-1, LE ,
		 alpha[numOfConstr-1]-alpha[numOfConstr-2]
                  -(lkm[numOfConstr-1]+lkm[numOfConstr-2])*minAngle/2.0 );
  row[numOfDesVar-4]=row[numOfDesVar-3]=0.0;
  row[numOfDesVar-2]=row[1]=1.0;
  row[numOfDesVar-1]=row[0]=-1.0;
  add_constraint(prob, row.get()-1, LE ,
		 2*M_PI + alpha[0]-alpha[numOfConstr-1]
                   -(lkm[0]+lkm[numOfConstr-1])*minAngle/2.0 );
  
  // Adding objective function
  for( int idx = 0 ; idx < numOfVar ; ++idx )
    {
      row[idx] = 1.0; // alkiot[idx/2]->coff();
      // muuttuja(alkiot[idx/2]->coff());
    }
  set_obj_fn(prob,row.get()-1);
  set_minim(prob);
  // print_lp(prob);
@}
The funny for loop while adding constraints is used to shift nonzero
elements to right. See matrix above.

The last problem is solved and solution is returned
@D Do actual optimization
@{
  if( OPTIMAL != solve(prob) )
    {
      feasible = false;
      for( int idx = 0 ; idx < numOfVar ; ++idx )
	{
	  row[idx]=0.0;
	}
    }
  else
    {
      @<Get solution from problem prob to vector row@>
    }

  delete_lp(prob);
@}

Now the solution is in vector row and it is writen back to the EdgeRing.
Note: If there is nothing to optimize, functions returns earlier.
@D Implementation of function OptimizeEdgeRing
@{
  varidx=0;
  double cang;
  for( int idx = 0 ; idx < numOfEdges ; ++varidx)
    {
      cang = alpha[varidx]+row[2*varidx]-row[2*varidx+1];
      cang -= (lkm[varidx]-1)*minAngle/2.0;
      for( int jdx = 0 ; jdx < lkm[varidx] ; ++jdx,++idx )
	{
	  alkiot[idx]->setAngle(cang);
	  cang += minAngle;
	}
    }
  
  return( feasible );
}
@| @}

The \verb|lp_solve| seems to lack the routine to get solution. Model
was taken from sources of \verb|lp_solve|, namely from function
\verb|print_solution|. Hopefully \verb|lp_solve| maintains the order
of variables ...
@d Get solution from problem prob to vector row
@{
double tarkistus=prob->best_solution[0];
// muuttuja(tarkistus);
for(int idx=0; idx<numOfVar; ++ idx)
  {
    row[idx]=prob->best_solution[prob->rows+idx+1];
    // muuttuja(row[idx]);
    tarkistus -= row[idx];
  }
// muuttuja(tarkistus);
assert((fabs(tarkistus) < 0.001 && "Muuttujista laskettu ratkaisu ei t‰sm‰‰"));
@| @}




\chapter{Tests}


\section{Tests for single nodes}

Single node with some transitions.
@O uno.lts
@{LSTS_FILE
BEGIN Header
state_cnt = 1
action_cnt = 5
transition_cnt = 6
initial_state = 1
END Header
BEGIN action_names
#0 = "tau"
1 = "rec"
2 = "send ack"
3 = "rec ack"
4 = "send"
5 = "stop"
END action_names
BEGIN transitions
1 1 0 
  1 1 
  1 2 
  1 3 
  1 4 
  1 5 ;
END transitions
END_LSTS
@| @}

Singel node without transitions
@O mini.lts
@{LSTS_FILE
BEGIN Header
state_cnt = 1
action_cnt = 5
transition_cnt = 0
initial_state = 1
END Header
BEGIN action_names
#0 = "tau"
1 = "rec"
2 = "send ack"
3 = "rec ack"
4 = "send"
5 = "stop"
END action_names
BEGIN transitions
END transitions
END_LSTS
@| @}


\section{Very simple test, star}

Following LTS file contains simple structure of one center node and
four other nodes connected to center node.\\[2mm]
\begin{minipage}{0.5\linewidth}
@O stara.lts
@{LSTS_FILE
BEGIN Header
state_cnt = 5
action_cnt = 5
transition_cnt = 12
initial_state = 1
END Header
BEGIN action_names
#0 = "tau"
1 = "rec"
2 = "send ack"
3 = "rec ack"
4 = "send"
5 = "stop"
END action_names
BEGIN transitions
1 2 0  
  5 3  
  3 2  
  4 4 ;
2 1 1  
  2 4 ;
3 1 3  
  3 5 ;
4 1 5  
  4 0 ;
5 1 1  
  5 1 ;
END transitions
END_LSTS
@| @}
\end{minipage}
\setlength{\unitlength}{1cm}
\newsavebox{\picbox}
\begin{lrbox}{\picbox}
\begin{picture}(5,5)(0,-5)
\put(2.5,-2.5){\circle*{0.3}}
\put(0.5,-2.5){\circle{0.3}}
\put(2.5,-4.5){\circle{0.3}}
\put(4.5,-2.5){\circle{0.3}}
\put(2.5,-0.5){\circle{0.3}}
%
\put(1.0, -2.25){\line(-2,-1){0.357}}
\put(1.0, -2.25){\line(1,0){1}}
\put(2.0, -2.25){\vector(2,-1){0.357}}
%
\put(1.0, -2.75){\vector(-2,1){0.357}}
\put(1.0, -2.75){\line(1,0){1}}
\put(2.0, -2.75){\line(2,1){0.357}}
%
\put(3.0, -2.25){\line(-2,-1){0.357}}
\put(3.0, -2.25){\line(1,0){1}}
\put(4.0, -2.25){\vector(2,-1){0.357}}
%
\put(3.0, -2.75){\vector(-2,1){0.357}}
\put(3.0, -2.75){\line(1,0){1}}
\put(4.0, -2.75){\line(2,1){0.357}}
%
\put(2.25, -4){\line(1,-2){0.1785}}
\put(2.25, -4){\line(0,1){1}}
\put(2.25, -3){\vector(1,2){0.1785}}
%
\put(2.75, -4){\vector(-1,-2){0.1785}}
\put(2.75, -4){\line(0,1){1}}
\put(2.75, -3){\line(-1,2){0.1785}}
%
\put(2.25, -2){\line(1,-2){0.1785}}
\put(2.25, -2){\line(0,1){1}}
\put(2.25, -1){\vector(1,2){0.1785}}
%
\put(2.75, -2){\vector(-1,-2){0.1785}}
\put(2.75, -2){\line(0,1){1}}
\put(2.75, -1){\line(-1,2){0.1785}}
\end{picture}
\end{lrbox}
\framebox{\usebox{\picbox}}


\section{More difficult test}

@O vaikea.lts
@{Lsts_file
Begin History

 ""

End History

Begin Header
 State_cnt = 11
 Transition_cnt = 23
 Action_cnt = 4
 Initial_state = 1
End Header

Begin Action_names
 2 = "errA"
 3 = "recB"
 4 = "sendA"
End Action_names

Begin Transitions
 1 7 0 1 0;
 2 8 0 2 0 7 0;
 3 9 0 3 0 2 3;
 4 9 0 4 0 7 0 2 3;
 5 9 0 5 0 10 0;
 6 11 0 6 0 1 3;
 7 3 4;
 8 1 2;
 9 6 2;
 10 4 3;
 11 5 4;
End Transitions

End_lsts
@| @}


\section{Huge test, plane net}

This test is not coded into this file, but the generator program is
provided. Program is compiled using command
\begin{center}
  \verb<g++ -o genplane genplane.cc<
\end{center}
and run
\begin{center}
  \verb|genplane <N> <M>|
\end{center}
where \verb|<N>| and \verb|<M>| are integers greater than 2. Program
writes output to \verb<stdout<.
@O genplane.cc -d
@{
#include <iostream>
#include <cstdlib>
using namespace std;

static void writeHeader(int N, int M);
static void writeTransitions(int N, int M);

int
main(int argc, char **argv)
{
  if( argc < 3 )
    {
      cerr << "Usage: " << argv[0] << " <N> <M>" << endl;
      cerr << "Usage: " << argv[0] << " N > 2, M > 2" << endl;
      return( 1 );
    }
  int N = atoi(argv[1]);
  int M = atoi(argv[2]);
  if( (N < 3) || (M < 3) )
    {
      cerr << "Usage: " << argv[0] << " <N> <M>" << endl;
      cerr << "Usage: " << argv[0] << " N > 2, M > 2" << endl;
      return( 1 );
    }
  writeHeader(N,M);
  writeTransitions(N,M);
  return( 0 );
}

static void
writeHeader(int N, int M)
{
  cout << "lts" << endl;
  cout << "semantics cffd" << endl;
  cout << "state_cnt " << N*M << endl;
  cout << "action_rng 1" << endl;
  cout << "transit_cnt " << 4*N*M -3*N -3*M +2 << endl;
  cout << "initial_state 1" << endl;
  cout << "actions" << endl;
  cout << "0 |tau|" << endl;
  cout << "1 !a!" << endl;
  cout << "end_actions" << endl;
}

static void
writeTransitions(int N, int M)
{
  cout << "transitions" << endl;
  for( int jj = 1 ; jj < M ; ++jj )
    {
      for( int ii = 1 ; ii < N ; ++ii )
	{
	  int se = jj*N+ii;
	  int sw = se + 1;
	  int ne = se - N;
	  int nw = ne + 1;
	  cout << ne << " " << nw << " 1 ;" << endl;
	  cout << ne << " " << se << " 1 ;" << endl;
	  cout << ne << " " << sw << " 1 ;" << endl;
	  cout << nw << " " << se << " 1 ;" << endl;
          if( ii == N-1 )
            {
	      cout << nw << " " << sw << " 1 ;" << endl;
	    }
	  if( jj == M-1 )
	    {
	      cout << se << " " << sw << " 1 ;" << endl;
	    }
	}
    }
  cout << "end_transitions" << endl;
}

@| @}




%%% ================== End of file ====================
%%% Local variables:
%%% mode: nuweb
%%% nuweb-source-mode: "c++"
%%% TeX-master: "illus.w"
%%% mode: outline-minor
%%% End:
