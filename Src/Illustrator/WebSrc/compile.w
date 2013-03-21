%-*-mode: nuweb; nuweb-source-mode: "c++"; TeX-master: "illus.w"; -*-
%%% $Source: Src/Illustrator/WebSrc/compile.w $
%%% $Id: compile.w 1.1 Mon, 25 Aug 2003 15:49:29 +0300 hvi $

\part{Compilation environment}



\chapter{Makefile ja illustraattorin kääntäminen}

Illustraattori on osa TVT kokonaisuutta ja käännettään siten muun
kokonaisuuden mukana.  Literate Programming tekniikka ei ole aidosti
tuettu TVT:n käännösympäristössä, mutta tästä ei pitäisi olla mitään
ongelmia, koska kääntäjän tarvitseman lähdekoodit on jo valmiiksi
generoitu.  Jos näin ei ole, tai lähdekoodiin joutuu tekemään
muutoksia, niin kääntäjän tarvitsemat tiedostot pitää generoida
komennolla
\begin{center}
  \verb<nuweb -t illus.w<
\end{center}

Yhtenäisen käännösympäristön vuoksi alihakemiston Makefile:ssä pitää
määritellä vain muutamia kohtia.
\begin{itemize}
\item Päätason lähdekoodihakemisto.
@o Makefile -t
@{TVT_BASE = ../..
@| @}
\item Käytetyt käännösyksiköt
@o Makefile -t
@{SOURCES = @<Compilation units@>
# Do not remove this comment
@| @}
\item Rakennettava ohjelma
@o Makefile -t
@{BIN = illus
@| @}
\item Käytetyt erilliset kirjastot, tässä tapauksessa vain LP\_SOLVE
@o Makefile -t
@{LOCAL_INC_DIRS = -I $(TVT_BASE)/ThirdPartySoftware
LOCAL_LIBS = -L $(TVT_BASE)/ThirdPartySoftware/lp_solve -llpk
@| @}
\end{itemize}
Muut tarvittavat osat saadaankin sitten yhteisestä Makefilestä
@o Makefile -t
@{include $(TVT_BASE)/BaseMakefile

#WARNINGS = -Wall -pedantic -ansi #$
@| @}

Vanhaa perua on, että muutamien yleisten osien toteutus on alkujaan
erotettu itse illustraattorista omaksi kokonaisuudekseen (tools.w).
Tuossa tiedostossa on määritelty käännösyksiköt
@d Compilation units
@{ graph.cc point.cc line.cc linealg.cc random.cc \
@| @}

Varsinainen graafinen käyttöliittymä on määritellyt, miten sille
annetaan tieto lasketusta kuvasta.  Tämä formaatti on koodattu
erilliseen käännösyksikköön
@d Compilation units
@{ lts_draw_write.cc \
@| @}

Myös tiedostojen nimet ovat vähän vanhaa perua ja ajalta, jolloin
joutui erikseen ottamaan kantaa, miten template"-määritykset
sisältävät modulit pitää kääntää, ettei binääriin tule samaa koodia
moneen kertaan.  Siksi tiedostot, joissa oli instantioitavia template
juttuja, on nimetty \verb<*.inst.cc< ja tavalliset modulit
\verb<*.cc<.  Sitten on vielä toteutuksia, jotka joutuu kääntämään
vasta instantaation yhteydessä ja niitä ei ole tarkoitettu omaksi
käännösyksikökseen.  Nämä on nimetty muodolla \verb<*.tmpl.cc<.

Loppuosa tästä osiosta onkin sitten käännösyksiköiden ja niiden vaatimien
esittelytiedostojen luettelointia.


\chapter{Compilation units}

\section{Model classes}


\subsection{LTS as container}
@d Compilation units
@{	lts.inst.cc \
@| @}


@O lts.hh -d
@{
#ifndef ILLUS_LTS_HH
#define ILLUS_LTS_HH

#ifndef TOOLS_POINT_HH
#include "point.hh"
#endif

#ifndef TOOLS_LINE_HH
#include "line.hh"
#endif

#ifndef TOOLS_GRAPH_HH
#include "graph.hh"
#endif

#include "StateProps.hh"

#ifndef MAKEDEPEND
#include <map>
#include <string>
#include <LSTS_File/TransitionsAP.hh>
#include <LSTS_File/ActionNamesAP.hh>
#endif

class InStream;
class Header;

@<lts.hh@>

#endif // ILLUS_LTS_HH
@| @}

@o lts.inst.cc -d
@{
#include "lts.hh"

#ifndef MAKEDEPEND
#include <fstream>
#include <LSTS_File/iLSTS_File.hh>
#endif

#include "lts_draw_write.hh"
#include "error_handling.hh"
@<lts.cc@>
@| @}



\subsection{Objective function}

@o objfunc.hh -d
@{
#ifndef ILLUS_OBJFUNC_HH
#define ILLUS_OBJFUNC_HH

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif


@<objfunc.hh@>

#endif // ILLUS_OBJFUNC_HH
@| @}


\section{Controllers and their auxiliary classes}


\subsection{Layput algorithm}
@d Compilation units
@{	layout.inst.cc \
@| @}

@o layout.hh -d
@{
#ifndef ILLUS_LAYOUT_HH
#define ILLUS_LAYOUT_HH

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif

@<layout.hh@>

#endif
@| @}

@O layout.inst.cc -d
@{
#include "layout.hh"

#ifndef ILLUS_GRID_HH
#include "grid.hh"
#endif

#ifndef ILLUS_INITIAL_HH
#include "initial.hh"
#endif

#ifndef ILLUS_LEVEL_HH
#include "level.hh"
#endif

#ifndef ILLUS_NONSYM_HH
#include "nonsym.hh"
#endif

#ifndef ILLUS_LOCALSEA_HH
#include "localsea.hh"
#endif

#ifndef ILLUS_SA_HH
#include "sa.hh"
#endif

#ifndef ILLUS_ARROWS_HH
#include "arrows.hh"
#endif

#ifndef ILLUS_OBJFUNC_HH
#include "objfunc.hh"
#endif

#ifndef ILLUS_GENETIC_HH
// Ei käytössä: Heikki Virtanen 7.6.2003
// #include "genetic.hh"
#endif

@<layout.cc@>
@| @}


\subsection{Initial solution}
@d Compilation units
@{	initial.cc \
@| @}

@o initial.hh -d
@{
#ifndef ILLUS_INITIAL_HH
#define ILLUS_INITIAL_HH

#ifndef ILLUS_GRID_HH
#include "grid.hh"
#endif

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif

@<initial.hh@>

#endif
@| @}

@o initial.cc -d
@{
#include "initial.hh"

@<initial.cc@>
@| @}


@d Compilation units
@{	level.inst.cc \
@| @}

@o level.hh -d
@{
#ifndef ILLUS_LEVEL_HH
#define ILLUS_LEVEL_HH

#ifndef ILLUS_GRID_HH
#include "grid.hh"
#endif

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif

@<level.hh@>

#endif
@| @}

@o level.inst.cc -d
@{
#include "level.hh"

@<level.cc@>
@| @}


@d Compilation units
@{	nonsym.cc \
@| @}

@o nonsym.hh -d
@{
#ifndef ILLUS_NONSYM_HH
#define ILLUS_NONSYM_HH

#ifndef ILLUS_GRID_HH
#include "grid.hh"
#endif

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif

@<nonsym.hh@>

#endif
@| @}

@o nonsym.cc -d
@{
#include "nonsym.hh"

@<nonsym.cc@>
@| @}


\subsection{Local search method}
@d Compilation units
@{	localsea.inst.cc \
@| @}

@o localsea.hh -d
@{
#ifndef ILLUS_LOCALSEA_HH
#define ILLUS_LOCALSEA_HH

#ifndef ILLUS_GRID_HH
#include "grid.hh"
#endif

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif

@<localsea.hh@>
@<shrink.hh@>

#endif
@| @}

@o localsea.inst.cc -d
@{
#include "localsea.hh"

#ifndef ILLUS_OBJFUNC_HH
#include "objfunc.hh"
#endif

#ifndef MAKEDEPEND
#include <algorithm>
#include <set>
#include <iostream>
#endif

@<localsea.cc@>
@<shrink.cc@>
@| @}


@d Compilation units
@{	sa.inst.cc \
@| @}

@o sa.hh -d
@{
#ifndef ILLUS_SA_HH
#define ILLUS_SA_HH

#ifndef ILLUS_GRID_HH
#include "grid.hh"
#endif

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif

@<sa.hh@>
#endif
@| @}

@o sa.inst.cc -d
@{
#include "sa.hh"

#ifndef ILLUS_OBJFUNC_HH
#include "objfunc.hh"
#endif

#ifndef TOOLS_RANDOM_HH
#include "random.hh"
#endif

#ifndef MAKEDEPEND
#include <cmath>
#include <iostream>
#endif

@<sa.cc@>
@| @}




\subsection{Genetic algorithm}
@d EI Compilation units
@{	genetic.inst.cc \
@| @}
@D poistettu genetic.hh -d
@{
#ifndef ILLUS_GENETIC_HH
#define ILLUS_GENETIC_HH

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif

@<genetic.hh@>

#endif // ILLUS_GENETIC_HH
@| @}
@D poistettu genetic.inst.cc -d
@{
#include "genetic.hh"

#ifndef MAKEDEPEND
#include <iostream>
#include <cstdlib>
#include <cmath>
#endif

@<genetic.cc@>
@| @}




\subsection{Finding arrows}
@d Compilation units
@{	arrows.inst.cc \
@| @}

@o arrows.hh -d
@{
#ifndef ILLUS_ARROWS_HH
#define ILLUS_ARROWS_HH

#ifndef ILLUS_GRID_HH
#include "grid.hh"
#endif

#ifndef TOOLS_LINEALG_HH
#include "linealg.hh"
#endif

@<arrows.hh@>

#endif // ILLUS_ARROWS_HH
@| @}

@o arrows.inst.cc -d
@{
#include "arrows.hh"

#ifndef ILLUS_SURROUND_HH
#include "surround.hh"
#endif

#ifndef MAKEDEPEND
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>
#endif

@<arrows.cc@>


@| @}



\section{Auxiliary datastructures}


\subsection{State placement grid}
@d Compilation units
@{	grid.inst.cc \
@| @}

@o grid.hh -d
@{
#ifndef ILLUS_GRID_HH
#define ILLUS_GRID_HH

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif

@<grid.hh@>

#endif // ILLUS_GRID_HH
@| @}

@o grid.inst.cc -d
@{
#include "grid.hh"
@<grid.cc@>
@| @}



\subsection{State surrounding optimization}
@d Compilation units
@{	surround.cc \
@| @}

@o surround.hh -d
@{
#ifndef ILLUS_SURROUND_HH
#define ILLUS_SURROUND_HH

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif

@<surround.hh@>

#endif // ILLUS_SURROUND_HH
@| @}

@o surround.cc -d
@{
#include "surround.hh"

#ifndef TOOLS_AUTO_VEC_HH
#include "auto_vec.hh"
#endif

#ifndef MAKEDEPEND
extern "C" {
#include <lp_solve/lpkit.h>
}
#include <cmath>
#endif

@<surround.cc@>
@| @}


\section{Style file}
@d Compilation units
@{	styles.inst.cc \
@| @}

@O styles.hh -d
@{
#ifndef ILLUS_STYLES_HH
#define ILLUS_STYLES_HH
#ifndef MAKEDEPEND
#include <utility>
#include <string>
#include <map>
#endif

@<styles.hh@>

#endif // ILLUS_STYLES_HH
@| @}

@O styles.inst.cc -d
@{
#include "styles.hh"

#ifndef MAKEDEPEND
#include <vector>
#include <fstream>
#endif

@<styles.cc@>
@| @}

@o styles.tmpl.cc
@{
@<styles.tmpl.cc@>
@| @}



\section{Plain algorithm}

@d Compilation units
@{	plain_algo.inst.cc \
@| @}

@o plain_algo.inst.cc -d
@{

#ifndef ILLUS_LAYOUT_HH
#include "layout.hh"
#endif

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif

#ifndef ILLUS_STYLES_HH
#include "styles.hh"
#endif

#include "lts_draw_write.hh"
#include "error_handling.hh"
#include "TvtCLP.hh"
#include "LogWrite.hh"

@<Program without GUI@>

#include "styles.tmpl.cc"

@}


\section{Tcl files}


\subsection{Tcl main}
@d EI Compilation units
@{	tcl_main.cc \
@| @}

@d poistettu tcl\_main.hh -d
@{
#ifndef ILLUS_TCL_MAIN_HH
#define ILLUS_TCL_MAIN_HH

#ifndef MAKEDEPEND
#include <tk.h>
#endif

@<tclmain.hh@>
#endif;
@| @}
@d poistettu tcl\_main.cc -d
@{
#include "tcl_main.hh"
#include "tcl_interp.hh"

#include "tcl_layout.hh"

#include "auto_ptr.hh"

@<tclmain.cc@>

int
main(int argc, char **argv)
{
  Tk_Main(argc, argv, TclMainCtrl::init);
  return( 0 );
}
@| @}



\subsection{Application class}
@d EI Compilation units
@{	tcl_layout.inst.cc \
@| @}

@d poistettu tcl\_layout.hh -d
@{
#ifndef ILLUS_TCL_LAYOUT_HH
#define ILLUS_TCL_LAYOUT_HH

#ifndef MAKEDEPEND
#include <string>
#endif

@<tcllayout.hh@>

#endif // ILLUS_TCL_LAYOUT_HH
@| @}

@D poistettu tcl\_layout.inst.cc -d
@{
#include "tcl_layout.hh"

#ifndef ILLUS_LAYOUT_HH
#include "layout.hh"
#endif

#ifndef ILLUS_LTS_HH
#include "lts.hh"
#endif

#ifndef TOOLS_TK_TOPLEVEL_HH
#include <Tools/tk_toplevel.hh>
#endif

#ifndef TOOLS_TK_SCROLLED_HH
#include <Tools/tk_scrolled.hh>
#endif

#ifndef ILLUS_STYLES_HH
#include "styles.hh"
#endif

@<tcllayout.cc@>

#include <Tools/tk_window.tmpl.cc>
#include "styles.tmpl.cc"
@| @}



\chapter{Template instantions}

@d Compilation units
@{	graph_tmpl.inst.cc \
@| @}

@o graph_tmpl.inst.cc -d
@{
#include "graph.cc"
#include "lts.hh"

template Graph<IlStateBase, IlTransitionBase>;
template GraphEdge<IlStateBase, IlTransitionBase>;
template GraphNode<IlStateBase, IlTransitionBase>;
@| @}

@d Compilation units
@{	stl.inst.cc \
@| @}
@o stl.inst.cc -d
@{
#ifndef MAKEDEPEND
#include <list>
#endif

// These instantiations is needed for class Line 
#include "point.hh"
template list<Point>;
@| @}


\chapter{Generic code fragments}

These are generic code fragments that apeare at many plases all over
the source code. Other common factor is, that this kinds of things are
often implicit assumptions in good programmin style but has very
little meaning in understanding what program does.

Some of these scraps are parameterized using C preprocessing
facilities. To use those scraps, make construct. This method has some
limitations.
\begin{itemize}
\item Recursion of parameterized scrap is not allowed
\item Can not be used to parameterize preprocessor macros
\end{itemize}
@d Model of using parameterized scraps
@{
#define PARAM1 Replacement A
#define PARAM2 Replacement B
@<Scrap to be used@>
#undef PARAM1
#undef PARAM2
@| @}
For example with M4 macro package there is many many tricks to do, but
need additional preprocessing of sources.

\section{Inter class scraps}


\subsection{Default methods}

This declares default set of constructors, destructor and assignment
operators. Parameter is name of class referred as \verb<THISCLASS<

@d Generic class parts with user implemented constructor and destructor
@{
public:
  THISCLASS();
  ~THISCLASS();
private:
  THISCLASS(const THISCLASS &);
  THISCLASS & operator = (const THISCLASS &) ;
@| THISCLASS @}


%%% Local variables:
%%% mode: outline-minor
%%% End:
