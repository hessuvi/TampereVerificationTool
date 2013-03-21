%-*-mode: nuweb; nuweb-source-mode: "c++"; aTeX-master: "illus.w"; -*-
%%% $Source: Src/Illustrator/WebSrc/zoom.w $
%%% $Id: zoom.w 1.1 Mon, 25 Aug 2003 15:49:29 +0300 hvi $

\part{Zooming}

\chapter{Käyttöohje}

Tämä on alkeellinen tuki zoomausta varten.  Zoomaus tarkoittaa, että
tapahtuma on näkyvissä vain joissain osissa käyttäytymistä.  Menetelmän
luonteesta johtuen, vain jälkeä voidaan käyttää näkyvyyden kriteerinä.
Lisäksi toteutus asettaa omat rajoituksensa.  Zoomauksen teoria on esitelty
paperissa \cite{valmari2002:zooming} ja menetelmä näkyy hyvin samanlaisena
myös ohhelmassa.

Ohjelma käynnistetään komennolla:
\begin{center}
  \verb|zoom <system rules> <lsts file> <switch file> <zooming rules> [<style file>] 
\end{center}
missä parametrit ovat:
\begin{description}
\item[system rules] on rinnankytkennän sääntötiedosto ja kuvaus
  järjestelmän kokoonpanon kuvaus viimeisessä vaiheessa.
\item[lsts file] on näytettävä LSTS-tiedosto, joka on edellisen
  sääntötiedoston kuvaaman järjestelmän käyttäytyminen.
\item[switch file] tiedostoon kirjoitetaan rinnankytkentään liitettävä apu
  LSTS, joka kertoo, milloin tapahtuma on näkyvä ja milloin näkymätön.
\item[zooming rules] tiedostoon puolestaan kirjoitetaan
  rinnankytkentäsäännöt, jotka ottavat myös kytkimen (switch) mukaan
  järjestelmän rinnalle.
\item[style file] on visualisoijan käyttöohjeessa kuvattu tyylitiedosto,
  joka kertoo, miten tapahtumien nimet yms tiedot havainnollistetaan.
\end{description}

Ohjelma piirtää ruudulle LSTS:n aivan samoin kuin visualisoijakin. Sen
lisäksi tuotetaan myös listaus rinnankytkentäsäännöistä, joiden
lopputuloksena on nimetön ($\tau$) tapahtuma.  Käyttäjän täytyy valita
\begin{enumerate}
\item osittain näkyvälle tapahtumalla nimi.
\item rinnankytkentäsäännöt, jotka tuottavat osittain näkyväksi halutun
  tapahtuman.
\item tilat, joissa tapahtuma on näkyvä.
\end{enumerate}
Esimerkiksi, jos tilaksi, jossa tapahtuma halutaan näkyväksi, valitaan
tila, jossa on $\tau$-silmukka, nähdään, josko ääerettömän suorituksen
aikana suoritetaan valittuja tapahtumia vai ei.

{\textsf{\small\slshape Tutki mahdollisuutta asettaa useita tapahtumia
    ehdollisesti näkyviksi kerralla.}}


\chapter{Ohjelman rakenne}

Ohjelma koostuu kahdesta binääristä eli algoritmeistä ja käyttöliittymästä
kuten visualisoijakin.  Jako tekee ohjelmasta vähän kömpelön, mutta
helpottaa siirrettävyyttä, kun eniten ympäristöriippuvainen osuus saadaan
pidettyä mahdollisimman pienenä.



@d Program without GUI
@{
static void
writeDrawingIntoFile
  (
   ostream &out,
   IlLTS& lts,
   IlStyles& styles
  )
{
  LTS_Draw_Write  piirros( out );

#define INITIAL_STATE_COLORS "black", "cyan"
#define NORMAL_STATE_COLORS "black", "#F0F0F0"
#define DEADLOCK_STATE_COLORS "black", "#F06060"

  piirros.TulostaTilaselite ( "Initial state", INITIAL_STATE_COLORS );
  piirros.TulostaTilaselite ( "Normal state", NORMAL_STATE_COLORS );
  piirros.TulostaTilaselite ( "Deadlock state", DEADLOCK_STATE_COLORS );
  piirros.TulostaTilaselite ( "Cut state", NORMAL_STATE_COLORS, true );

  const StatePropsContainer& spcont = lts.getStatePropsContainer();

  for( IlLTS::StateIterator ss = lts.beginStates() ;
       ss != lts.endStates() ;
       ++ss )
    {
        list<string> statePropNames;
        if(spcont.isInitialized())
        {
            unsigned stateNumber = lts.getStateNumber(ss);
            StatePropsContainer::StatePropsPtr props =
                spcont.getStateProps(stateNumber);
            for(unsigned prop = spcont.firstProp(props); prop;
                prop=spcont.nextProp(props))
            {
                statePropNames.push_back(string("\"")+
                                         spcont.getStatePropName(prop)+'"');
            }
        }

      bool cut = ((ss->flags() & IlStateBase::CUTSTATE_FLAG) != 0);
      if( ss->flags() & IlStateBase::INITIAL_STATE )
	{
	  piirros.TulostaTila( ss->coords(), "", INITIAL_STATE_COLORS, cut,
                               statePropNames );
	}
      else if( !(ss->flags() & IlStateBase::HASTRANSITIONS_FLAG) )
	{
	  piirros.TulostaTila( ss->coords(), "", DEADLOCK_STATE_COLORS, cut,
                               statePropNames );
	}
      else
	{
	  piirros.TulostaTila( ss->coords(), "", NORMAL_STATE_COLORS, cut,
                               statePropNames );
	}
    }


  for( IlLTS::TransitionIterator tt = lts.beginTransitions() ;
       tt != lts.endTransitions() ;
       ++tt )
    {
      piirros.TulostaTapahtuma( tt->line(),
				"",
				styles.color(tt->action()),
				styles.dash(tt->action())
				);
    }


  for( IlLTS::ActionIterator aa = lts.beginAlphabet() ;
       aa != lts.endAlphabet() ;
       ++aa )
    {
      piirros.TulostaTapahtumaselite
	(
	aa->second,
	styles.color( aa->first ),
	styles.dash( aa->first )
	);
    }
    
}


class IllusCLP: public TvtCLP
{
    static const char* const description;

 public:
    IllusCLP(): TvtCLP(description) {}

    void help(const string& programName)
    {
        printProgramDescription();
        cerr << endl << "Usage:" << endl << " " << programName
             << " [<options>] <LSTS-file> [<style file>]" << endl << endl;
        printOptionsHelp();
        cerr << endl << LogWrite::
            neatString( "The output of the program is a textual "
                        "description of an LSTS image and it is "
                        "always written to stdout. The image can be "
                        "viewed with the separate GUI-program." )
             << endl << endl;
        printCopyrightNotice();
    }
};

const char * const IllusCLP::description = "Visualizes a given LSTS.";


int main(int argc, char **argv)
{
  IllusCLP clp;
  
  if( ! clp.parseCommandLine(argc, argv) ) {
    return 1;
  }
  
  IlLTS lts;
  
  lts.initializeFromFile( clp.getInputStream(), clp.getCommentString() );

  IlLayoutAlgorithm(lts);

  IlStyles styles;
  if( clp.getFilenamesCnt() >= 2 ) {
    check_claim( styles.readFromFile(clp.getInputFilename(1).c_str() ),
		 string("Can't read style file ")+
		 clp.getInputFilename(1));
  }

  styles.buildMap( lts.beginAlphabet(), lts.endAlphabet() );

  writeDrawingIntoFile(cout, lts, styles);

  return 0;
}
@| @}



\chapter{Consept and hierarchy}

This user interface follows model-view-control pattern. It means that
there is model classes (in this case LTS) and views (dialogs) at low
level of implementation. The information is transfered from model to
view and vise versa by controllers. There may not be any compile time
or link time dependency between model and views.

There is three different kind of information to be visualized. First
and most inportant is automaton self. But it cannot be understood
without explanation of colors and symbols with which different kind of
states and actions of transitions are represented. Third part of
information is not so important but handy. It contains statistic of
LTS, it's size, history, sematics, promises and other staff from
beginning of LTS-file.

For each piece of information there is own dialog and corresponding
controller. All these cotroller-view pairs are controlled by main
controller and layout application class, which allso owns the model.
All these controllers and views are explained detaild in following
sections.

\begin{figure}[htbp]
  \begin{center}
    \includegraphics[width=0.8\linewidth]{gui_classes.eps}
    \caption{Classes dealing with gui.}
    \label{fig:gui.classes}
  \end{center}
\end{figure}


\section{Main controller}

@d tclmain.hh
@{
class TclMainCtrl
{
@<TclMainCtrl interface@>
};
@| @}

@d tclmain.cc
@{
@<TclMainCtrl implementation@>
@| @}



\subsection{Creation}

@d TclMainCtrl interface
@{
public:
  TclMainCtrl();
@| @}
@d TclMainCtrl implementation
@{
TclMainCtrl::TclMainCtrl()
{
}
@| @}


\subsection{Initialization}

@d TclMainCtrl interface
@{
public:
  static int init(Tcl_Interp *interp);
@| @}
@d TclMainCtrl implementation
@{
int
TclMainCtrl::init(Tcl_Interp *interp)
{
  if (Tcl_Init(interp) == TCL_ERROR)
    {
      return TCL_ERROR;
    }
  if (Tk_Init(interp) == TCL_ERROR)
    {
      return TCL_ERROR;
    }
  Tcl_StaticPackage(interp, "Tk", Tk_Init, Tk_Init);
  
  @<Creating program specific commands@>

  Tcl_SetVar(interp, "tcl_rcFileName", "~/.illusrc", TCL_GLOBAL_ONLY);
  return TCL_OK;

}
@| @}


\subsection{Local commands}

Only local command is \verb<view_lts<, which visualize given labelled
transitions system.
@d TclMainCtrl interface
@{
public:
static int s_parser( ClientData data, Tcl_Interp *interp,
		   int argc, char **argv);

virtual void viewLts( const char *ltsFileName,
		      const char *styleFileName);
@| @}
@d TclMainCtrl implementation
@{
int
TclMainCtrl::s_parser( ClientData data, Tcl_Interp *interp,
		       int argc, char **argv)
{
  TclInterp::setInterp(interp);
  TclMainCtrl *ctrl = (TclMainCtrl *)data;
  if( argc < 3 )
    {
      TclInterp::setResult("Layouter: too few parameters");
      return( TclInterp::ERROR );
    }
  const char *ltsFileName = argv[1];
  const char *styleFileName = argv[2];
  
  ctrl->viewLts(ltsFileName,styleFileName);
  return TclInterp::retCode();
}
@| @}

@d Creating program specific commands
@{
static auto_ptr<TclMainCtrl> ctrl = new TclMainCtrl;
if ( NULL == Tcl_CreateCommand(interp,
			       "view_lts",
			       s_parser,
			       ctrl.get(),
			       NULL) )
{
  sprintf( interp->result ,
	   "Can not create command %s" ,
	   "view_lts" );
  return( TCL_ERROR );
}
else
{
  return( TCL_OK );
}

@| @}

@d TclMainCtrl interface
@{
@| @}


@d TclMainCtrl implementation
@{
void
TclMainCtrl::viewLts( const char *ltsFileName,
		      const char *styleFileName)
{
  TclLayoutApp *app = new TclLayoutApp(string(""),
				       string("app1"),
				       string(ltsFileName),
				       string(styleFileName));
  app=app;
}
@| @}





\section{Layout application class}

@d tcllayout.hh
@{
class TclLayoutApp
{
public:
  TclLayoutApp(const string &root, const string &name,
	       const string &ltsFileName,
               const string &styleFileName);
};
@| @}

@D tcllayout.cc
@{
static void writeLts(ostream &, IlLTS &);
static void drawLts(TkScrolledWin&, IlLTS &, IlStyles &);

TclLayoutApp::TclLayoutApp(const string &root, const string &w_name,
			   const string &ltsFileName,
			   const string &styleFileName)
{
  TkToplevelWin *root_win = new TkToplevelWin(root,w_name);
  root_win->open( (char **)0, (char **)0 );
  IlLTS *lts_ptr = new IlLTS;
  IlStyles *style_ptr = new IlStyles;
  // IlStyles *style_ptr = 0;
  TkScrolledWin *view_ptr = new TkScrolledWin(root_win->name(),string("lts"));
  if( lts_ptr
      && style_ptr
      && view_ptr 
      && TclInterp::retCode() == TclInterp::OK )
    {
      IlLTS &lts = *lts_ptr;
      IlStyles &styles = *style_ptr;
      TkScrolledWin &view = *view_ptr;
      char tcl_cmd[128];
      *tcl_cmd = '\0';
      sprintf(tcl_cmd,"pack %s -expand 1 -fill both",view.name().c_str());
      TclInterp::eval(tcl_cmd);
      cerr << "Reading file " << ltsFileName << endl;
      if( ! lts.initializeFromFile( ltsFileName.c_str()) )
	{
	  TclInterp::setResult( string("Can't read LTS file "));
	  TclInterp::appendResult(ltsFileName);
	  TclInterp::error();
	  return;
	}
      if( ! styles.readFromFile( styleFileName ) )
	{
	  TclInterp::setResult( string("Can't read style file "));
	  TclInterp::appendResult(styleFileName);
	  TclInterp::error();
	  return;
	}
      styles.buildMap(lts.beginAlphabet(),lts.endAlphabet());
      IlLayoutAlgorithm(lts);
//      writeLts(cout,lts);
      drawLts(view,lts,styles);
      
    }
}

static void
drawLts(TkScrolledWin &view, IlLTS &lts, IlStyles &styles)
{
  // os << "<= Aktiot: ====================>" << endl;
  int X = 20;
  int Y = 30;
  int Yinc = 30;
  const double NODE_RADIUS = 0.11;
  for( IlLTS::ActionIterator aa = lts.beginAlphabet() ;
       aa != lts.endAlphabet() ;
       ++aa )
    {
      string cmd("create text");
      char opt[128];
      sprintf( opt, "%d %d -text \"%s\" -anchor sw",
	       X, Y, (*aa).second.c_str() );
      view.apply(cmd, &opt , (&opt)+1 );
      cmd = "create line";
      sprintf( opt, "%d %d %d %d -fill %s",
	       X, Y+5, X+70, Y+5, styles.color((*aa).first).c_str());
      view.apply(cmd, &opt , (&opt)+1 );
      Y += Yinc;
    }
  // os << "<= Tilat: =====================>" << endl;
  double X_OFFSET = 1.0;
  // double Y_OFFSET = 0.0;
  for( IlLTS::StateIterator ss = lts.beginStates() ;
       ss != lts.endStates() ;
       ++ss )
    {
      string cmd("create oval");
      char opt[128];
      char *format;
      if( (*ss).flags() & IlStateBase::INITIAL_STATE )
	{
	  format = "%fi %fi %fi %fi -fill red -outline black";
	}
      else
	{
	  format = "%fi %fi %fi %fi -fill gray -outline #CFCFCF";
	}
      sprintf( opt , format,
	       (*ss).coords().xCoord()-NODE_RADIUS+X_OFFSET,
               (*ss).coords().yCoord()-NODE_RADIUS,
	       (*ss).coords().xCoord()+NODE_RADIUS+X_OFFSET,
               (*ss).coords().yCoord()+NODE_RADIUS );
      view.apply(cmd, &opt , (&opt)+1 );
    }
  // os << "<= Siirtymät: =================>" << endl;
  for( IlLTS::TransitionIterator tt = lts.beginTransitions() ;
       tt != lts.endTransitions() ;
       ++tt )
    {
      string cmd("create line");
      char *flags_fmt = "-fill %s -width 1 -arrow last -smooth 0";
      char flags[128];
      sprintf(flags,flags_fmt, styles.color((*tt).action()).c_str());
      string coord("");
      (*tt).line().trimBegin(NODE_RADIUS);
      (*tt).line().trimEnd(NODE_RADIUS);
      for( Line::iterator ll = (*tt).line().begin() ;
	   ll != (*tt).line().end() ;
	   ++ll)
	{
          char pari[80];
          sprintf(pari, " %fi %fi",
		  (*ll).xCoord()+X_OFFSET,
		  (*ll).yCoord());
	  coord += pari;
	}
      const char *opt[4];
      string tags("-tags \"");
      tags += lts.nameOfAction((*tt).action());
      tags += "\"";
      opt[0] = coord.c_str();
      opt[1] = flags;
      opt[2] = tags.c_str();
      view.apply(cmd, opt , opt+3 );
    }
}


static void
writeLts(ostream &os, IlLTS &lts)
{
  os << "<= Aktiot: ====================>" << endl;
  for( IlLTS::ActionIterator aa = lts.beginAlphabet() ;
       aa != lts.endAlphabet() ;
       ++aa )
    {
      os << (*aa).first << " <---> " << (*aa).second << endl;
    }
  os << "<= Tilat: =====================>" << endl;
  for( IlLTS::StateIterator ss = lts.beginStates() ;
       ss != lts.endStates() ;
       ++ss )
    {
      os << (*ss).name()
	 << " (" << (*ss).coords().xCoord() << ","
	 << (*ss).coords().yCoord() << " )" << endl;
    }
  os << "<= Siirtymät: =================>" << endl;
  for( IlLTS::TransitionIterator tt = lts.beginTransitions() ;
       tt != lts.endTransitions() ;
       ++tt )
    {
      os << (*tt).tail().name() << " -- "
	 << (*tt).action() << " ---> "
	 << (*tt).head().name() << " :";
      for( Line::iterator ll = (*tt).line().begin() ;
	   ll != (*tt).line().end() ;
	   ++ll)
	{
	  os << " ( " << (*ll).xCoord() << "," 
	     << (*ll).yCoord() << " )";
	}
      os << endl;
    }
}
@| @}



\section{Graph dialog controller}


\section{Graph dialog}

Graph dialog displays graph as circles corresponding states and bended
arrows corresponding transitions.
@d Definition of class IlTclGraphDialog
@{
class IlTclGraphDialog
{
@<Definition of class IlTclGraphDialog continued@>
};
@| @}

To operate properly, this umbrella class need name of drawing canvas,
coordinate system origin and scaling of coordinates.
@d Definition of class IlTclGraphDialog continued
@{
public:
  IlTclGraphDialog( const char *myName,
		    int width = 800,
                    int height = 700,
		    int full_width = 2400,
		    int full_height = 2100 );

@| @}
The \verb<myName< argument is name of Tk drawing canvas in same format
it is given in Tcl language. Parameters \verb<width< and \verb<hight<
discribes window size. Parameters \verb<full_width< and
\verb<full_height< is absolute maximum of picture size. All four is
measured in pixels.

@d Definition of class IlTclGraphDialog continued
@{
  ~IlTclGraphDialog();
@| @}

@d Definition of class IlTclGraphDialog continued
@{
  void drawState(Point place, 
@| @}



koordinaatisto


\section{Graph dialog controller}



\section{Explanation dialog}


\section{Explanation dialog controller}



\section{Information dialog}


\section{Information dialog controller}



\chapter{Style file}

\section{Style file content and format}

%\marginpar{This should go to model part}
Physical appearance of graph is not fully controlled by combination of
desition variables and information found in LTS file. Addition of that
at least following information is needed
\begin{itemize}
\item color and dash pattern coding of action of transition.
\item shape, label and fill and outline color coding of flags of states
\item background color
\item text color
\item state size
\item grid radius
\item width of transition arrow line
\end{itemize}
These things seem very minor, but they have remarkable impact of
quality of picture of LTS. Most of this information is verification
project wide, at least what comes to action and state flags coding. So
right place for it is not LTS files nor illustrator sources. They can
not be bind into any windowing system specific resources (X11 resource
database). And can not be selected randomly. Only suitable solution is
stylefile, but it is not good without style editor. However building
style editor is not trivial task, so it is done later if ever.

There is reasonable default values for other information except colors
and dash patterns of actions, since names of actions are picked by
user. Other information is more static.

In stylefile there is only colors and dash patterns of actions. Format
of stylefile is described in user documentation.

@d styles.hh
@{
#include <cassert>
using namespace std;

class IlStyleItem: public pair<string,string>
{
public:
  IlStyleItem() {};
  IlStyleItem(const pair<string,string> &p): pair<string,string>(p) {};
  IlStyleItem(const string &col, const string &dashPattern)
    : pair<string,string>(col, dashPattern ) {};
  const string &color() { return first ; };
  const string &dash()  { return second ; };
};

class IlStyles: public map<string, IlStyleItem>
{
public:
  bool readFromFile(const string &styleFileName);
  
  const string &color(const string &action);
  const string &dash(const string &action) const;

  template <class ITER>
  void buildMap(ITER start, ITER end);
  const string &color( int action ) const;
  const string &dash( int action ) const;
private:
  typedef map<int,IlStyleItem> AlternateMap;
  AlternateMap d_map;
};
@| @}
As class self contains stylefile content. Method \verb<buildMap< makes
alternate map accordinig to action number action name pairs it gets as
parameter. Possible iterator type is \verb<IlLTS::ActionIterator<.


@D styles.cc
@{
const int BUFFER_SIZE = 1024 ;
static char s_buffer[BUFFER_SIZE+1];

bool
IlStyles::readFromFile(const string &styleFileName)
{
  ifstream file(styleFileName.c_str());
  if( ! file )
    {
      return( false );
    }
  char delim;
  for( file >> delim ; file ; file >> delim )
    {
      file.getline(s_buffer, BUFFER_SIZE, delim);
      string actionName(s_buffer);
      string color;
      file >> color;
      if( !file )
	{
	  return( false );
	}
      file.getline(s_buffer, BUFFER_SIZE);
      char *start;
      for( start = s_buffer ; *start && *start != '"' ; ++start )
	;
      char *tail;
      for( tail = start+1; *start && *tail && *tail != '"' ; ++tail )
	;
      if( *start == '"' && *tail == '"' )
	{
	  *(tail)='\0';
          ++start;
	  string dashpat(start);
	  insert(make_pair(actionName,make_pair(color,dashpat)));
	}
      else
	{
	  insert(make_pair(actionName,make_pair(color,string(""))));
	}
    }
  return( true );
}

const string &
IlStyles::color(const string &action)
{
  static const char *vareja[]
    = { "#D0D0D0",
        "#FF0000", "#0000FF", "#00E000",
        "#8010FF", "#E0E000", "#008000",
        "#FFA040", "#00A0FF", "#E060E0",
        "#FFA0FF", "#A0FF00", "#000080",
        "#800000", "#80FF80", "#A030A0"};
  static vector<string> varit(vareja, vareja+16);
  static unsigned current_color=0;
  static string default_dash[]={"", "xx ", "xxxx x ", "", "x "};
  static unsigned whichdash = 0;

  const_iterator pos = find(action);
  if( pos == end() )
    {
      unsigned color_idx = current_color % varit.size();
      ++ current_color;
      pos=insert(make_pair(action,
                           make_pair(varit[color_idx],
                                     default_dash[(whichdash++)%5]))).first;
    }
  assert( pos != end() && "action not in stylefile");
  return( (*pos).second.first );
}

const string &
IlStyles::dash(const string &action) const
{
  static const string default_dash("");

  const_iterator pos = find(action);
  //assert( pos != end() && "action not in stylefile");
  if( pos == end() )
    {
      return default_dash;
    }
  return( (*pos).second.second );
}
@}

@D styles.tmpl.cc
@{
template <class ITER>
void
IlStyles::buildMap(ITER start, ITER tail)
{
  for( ITER ii = start ; ii != tail ; ++ii)
    {
      d_map.insert(make_pair((*ii).first,
			     make_pair(color((*ii).second),
				       dash((*ii).second))));
    }
}
@}
@D styles.cc
@{
const string &
IlStyles::color( int action ) const
{
  AlternateMap::const_iterator pos = d_map.find(action);
  assert( pos != d_map.end() && "action not in stylefile");
  return( (*pos).second.first );
}

const string &
IlStyles::dash( int action ) const
{
  AlternateMap::const_iterator pos = d_map.find(action);
  assert( pos != d_map.end() && "action not in stylefile");
  return( (*pos).second.second );
}

@| @}




\chapter{Illux}

@O tvt.illux
@{#!/bin/sh
TEMPFILE=/tmp/illuxtmp$$
tvt.illus $* > $TEMPFILE
if [ $? -eq 0 ]
then
    tvt.illux_view < $TEMPFILE
fi
rm $TEMPFILE
@}


%%% Local variables:
%%% mode: outline-minor
%%% End:
