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
  piirros.TulostaTilaselite ( "Cut state", NORMAL_STATE_COLORS,
                              LTS_Draw_Write::CUTFLAG);
  piirros.TulostaTilaselite ( "Has sticky props", NORMAL_STATE_COLORS,
                              LTS_Draw_Write::STICKYFLAG);

  const StatePropsContainer& spcont = lts.getStatePropsContainer();

  for( IlLTS::StateIterator ss = lts.beginStates() ;
       ss != lts.endStates() ;
       ++ss )
    {
        list<string> statePropNames;
        unsigned stateNumber = lts.getStateNumber(ss);
        string stateName = valueToMessage("",stateNumber,"");

        if(spcont.isInitialized())
        {
            StatePropsContainer::StatePropsPtr props =
                spcont.getStateProps(stateNumber);
            for(unsigned prop = spcont.firstProp(props); prop;
                prop=spcont.nextProp(props))
            {
                statePropNames.push_back(string("\"")+
                                         spcont.getStatePropName(prop)+'"');
            }
        }

      int cut = (ss->flags() & IlStateBase::CUTSTATE_FLAG) ?
          LTS_Draw_Write::CUTFLAG : 0;
      int sticky = (ss->flags() & IlStateBase::STICKYPROP_FLAG) ?
          LTS_Draw_Write::STICKYFLAG : 0;
      int flags = cut | sticky;

      if( ss->flags() & IlStateBase::INITIAL_STATE )
        {
          piirros.TulostaTila( ss->coords(), stateName,
                               INITIAL_STATE_COLORS, flags,
                               statePropNames);
        }
      else if( !(ss->flags() & IlStateBase::HASTRANSITIONS_FLAG) )
        {
          piirros.TulostaTila( ss->coords(), stateName,
                               DEADLOCK_STATE_COLORS, flags,
                               statePropNames );
        }
      else
        {
          piirros.TulostaTila( ss->coords(), stateName,
                               NORMAL_STATE_COLORS, flags,
                               statePropNames );
        }
    }


  for( IlLTS::TransitionIterator tt = lts.beginTransitions() ;
       tt != lts.endTransitions() ;
       ++tt )
    {
      piirros.TulostaTapahtuma( tt->line(),
                                lts.nameOfAction(tt->action()),
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
        // printStdinStdoutNotice();
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
#ifndef NO_EXCEPTIONS
    try {
#endif
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
#ifndef NO_EXCEPTIONS
    } catch(...) { return 1; }
#endif

    return 0;
}



#include "styles.tmpl.cc"

