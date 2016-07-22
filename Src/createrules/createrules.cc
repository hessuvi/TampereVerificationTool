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

Contributor(s): Juha Nieminen.
*/

#include "cr_clp.hh"
#include "RegExp.hh"

#include "LSTS_File/iLSTS_File.hh"
#include "LSTS_File/ActionNamesAP.hh"
#include "LSTS_File/StatePropsAP.hh"
#include "FileFormat/Lexical/ITokenStream.hh"

#include "ParRulesFile/ParRulesFile.hh"
#include "ParRulesFile/ParComponentsAP.hh"
#include "ParRulesFile/RulesAP.hh"
#include "ParRulesFile/StatePropRulesAP.hh"
#include "error_handling.hh"

#ifndef MAKEDEPEND
#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>
using namespace std;
#endif



// Action name to be renamed
// ------------------------------------------------------------------------
struct ActionRename
{
    inline ActionRename(const string& aName): action(aName) {}

    RegExp action;
    vector<string> newName;
};



//========================================================================
// LSTS-reading (reads action names from LSTS-file to the given data
// structure)
//========================================================================
class ActionNamesReading: public iActionNamesAP
{
public:
    inline ActionNamesReading(map<string, lsts_index_t>& an):
        actionNames(an)
    { }

private:
    map<string, lsts_index_t>& actionNames;

    void lsts_StartActionNames(Header&) {}

    void lsts_ActionName(lsts_index_t action_number, const string& name)
    {
        if(action_number > 0)
        {
            check_claim(actionNames.find(name) == actionNames.end(),
                        string("Action name '")+name+"' found twice.");

            actionNames[name] = action_number;
        }
    }

    void lsts_EndActionNames() {}
};


//========================================================================
// LSTS-reading (reads state proposition names from LSTS-file to the
// given data structure)
// ========================================================================

class StatePropositionNamesReading: public iStatePropsAP, public oStatePropRulesAP
{
public:
  typedef map<string, lsts_index_t> CompanionContainer;

public:
  inline StatePropositionNamesReading():
    current_lsts(0)
  { }

private:
  CompanionContainer statePropositionMap;
  lsts_index_t current_lsts;

public:

  void start_PropositionsFromLSTS( lsts_index_t idx )
  {
    /*check_claim( 0 == current_lsts,
      string("Proposition names reading: internal error: recursion"));*/
    current_lsts = idx ;
  }
  void end_PropositionsFromLSTS( lsts_index_t idx )
  {
    check_claim( idx == current_lsts,
		 string("Proposition names reading: internal error: mismatch"));
    current_lsts = 0;
  }

  void lsts_StartStateProps( class Header& )
  { }

  void lsts_StartPropStates( const std::string& prop_name )
  {
    CompanionContainer::iterator old( statePropositionMap.find(prop_name));
    check_claim( statePropositionMap.end() == old || old->second == current_lsts,
		 string("Proposition names reading: Multiple LSTSs define '")
		 + prop_name + "'");
    if ( statePropositionMap.end() == old )
      {
	statePropositionMap[prop_name] = current_lsts;
      }
  }

  void lsts_PropState( lsts_index_t )
  { }

  void lsts_EndPropStates( const std::string& )
  { }

  void lsts_EndStateProps()
  { }

public:
  bool lsts_doWeWriteStatePropRules()
  {
    return (statePropositionMap.size() > 0);
  }
  
  void lsts_WriteStatePropRules( iStatePropRulesAP& writer )
  {
    for ( CompanionContainer::iterator ptr=statePropositionMap.begin() ;
	  statePropositionMap.end() != ptr ;
	  ++ptr )
      {
	lsts_index_t component(ptr->second);
	string proposition_name(ptr->first);
	ostringstream result;
	result << component << ".\""<< proposition_name
	       << "\"->\"" << proposition_name << "\"";
	writer.lsts_StatePropRule(result.str());
      }
  }
};

//========================================================================
// Pattern searching function
// Looks if any item in 'regExprs' matches 'name' and returns iterator to
// the matching element or to end() if nothing matches
//========================================================================
vector<RegExp>::const_iterator
searchMatch(const string& name, const vector<RegExp>& regExprs)
{
    for(vector<RegExp>::const_iterator iter = regExprs.begin();
        iter != regExprs.end(); ++iter)
    {
        if(iter->match(name)) return iter;
    }
    return regExprs.end();
}

vector<ActionRename>::const_iterator
searchNextMatch(const string& name, const vector<ActionRename>& regExprs,
                vector<ActionRename>::const_iterator iter)
{
    for(;iter != regExprs.end(); ++iter)
    {
        if(iter->action.match(name)) return iter;
    }
    return regExprs.end();
}

inline vector<ActionRename>::const_iterator
searchMatch(const string& name, const vector<ActionRename>& regExprs)
{
    return searchNextMatch(name, regExprs, regExprs.begin());
}


//========================================================================
// Rulesfile writer class. Calculates parallel composition rules and
// writes them.
//========================================================================
class WriteRules: public oParComponentsAP,
                  public oRulesAP,
                  public oActionNamesAP
{
public:
    inline WriteRules(CreateRulesCLP& clparser,
                      const vector<map<string, lsts_index_t> >& aNames,
                      const vector<RegExp>& hidActs,
                      const vector<ActionRename>& renActs,
		      StatePropositionNamesReading &spn):
        clp(clparser),
        inputActionNames(aNames),
        hideActions(hidActs),
        renameActions(renActs),
        actionCount(0), rulesCount(0)
    {
        CalculateRules();
        oParrulesFile rules;
        rules.AddParComponentsWriter(*this);
        rules.AddRulesWriter(*this);
        rules.AddActionNamesWriter(*this);
	if( clp.propositions() )
	  {
	    // check_claim(false, "Not yet");
	    rules.AddStatePropRulesWriter(spn);
	  }
        rules.WriteFile(clp.getOutputStream(0));
    }

//---------------------------------------------------------------------------
private:
    typedef map<string, lsts_index_t> NameToNumb_t;
    typedef map<string, vector<lsts_index_t> > NameToNumbs_t;

    // Input:
    // ------
    CreateRulesCLP& clp;
    const vector<NameToNumb_t>& inputActionNames;
    const vector<RegExp>& hideActions;
    const vector<ActionRename>& renameActions;

    // Output:
    // -------
    // Action names of result. First element is action name, second element
    // is action number:
    NameToNumb_t destActionNames;

    // Map from input action name to output action number(s). First element
    // is input action name, second element contains resulting action
    // numbers:
    NameToNumbs_t destNumbers;

    unsigned actionCount, rulesCount;

    void CalculateRules()
    {
        // For each input-LSTS:
        for(unsigned i=0; i<inputActionNames.size(); i++)
        {
            // and for each of its action name:
            for(NameToNumb_t::const_iterator ian_iter =
                    inputActionNames[i].begin();
                ian_iter != inputActionNames[i].end();
                ++ian_iter)
            {
                // look if the same action name has already been handled:
                if(destNumbers.find(ian_iter->first) == destNumbers.end())
                {
                    // if not, add it to 'destNumbers'
                    NameToNumbs_t::iterator newDestAN_iter =
                        destNumbers.insert(destNumbers.begin(),
                                           NameToNumbs_t::value_type
                                           (ian_iter->first,
                                            vector<lsts_index_t>()));

                    // look if we have to rename the action name:
                    // ------------------------------------------
                    vector<ActionRename>::const_iterator renIter =
                        searchMatch(ian_iter->first, renameActions);
                    if(renIter != renameActions.end())
                    {
                        do
                        {
                            for(unsigned i=0; i<renIter->newName.size(); ++i)
                            {
                                string newName =
                                    renIter->action.replace
                                    (ian_iter->first, renIter->newName[i]);
                                // Look if same dest. name already exists:
                                NameToNumb_t::iterator dan_iter =
                                    destActionNames.find(newName);
                                // If it doesn't, add the new name:
                                if(dan_iter == destActionNames.end())
                                {
                                    destActionNames[newName] = ++actionCount;
                                    newDestAN_iter->second.push_back
                                        (actionCount);
                                }
                                else // use the number of the used name:
                                {
                                    newDestAN_iter->second.push_back
                                        (dan_iter->second);
                                }
                                ++rulesCount;
                            }

                            renIter = searchNextMatch(ian_iter->first,
                                                      renameActions,
                                                      ++renIter);
                        } while(renIter != renameActions.end());
                    } // end renaming

                    // if not, look if the action name has to be hidden:
                    // -------------------------------------------------
                    else if(searchMatch(ian_iter->first, hideActions) !=
                            hideActions.end())
                    {
                        newDestAN_iter->second.push_back(0);
                        ++rulesCount;
                    }
                    // else just add it as is to the destination names with
                    // a consecutive action number:
                    // ----------------------------------------------------
                    else
                    {
                        destActionNames[ian_iter->first] = ++actionCount;
                        newDestAN_iter->second.push_back(actionCount);
                        ++rulesCount;
                    }
                }// end if same action name already handled
            }// end for each action name of input
        }// end for each input-LSTS
    }

    // ParComponents section:
    // ----------------------
    virtual bool lsts_doWeWriteParComponents() { return true; }
    virtual void lsts_WriteParComponents(iParComponentsAP& pipe)
    {
        for(unsigned i=1; i<clp.getFilenamesCnt(); i++)
            pipe.lsts_Component(i, clp.getInputFilename(i), "" );
    }

    // Rules section:
    // --------------
    virtual lsts_index_t lsts_numberOfRulesToWrite()
    { return rulesCount; }

    virtual void lsts_WriteRules(iRulesAP& pipe)
    {
        // For each input action name create a rule or rules:
        for(NameToNumbs_t::const_iterator ian_iter = destNumbers.begin();
            ian_iter != destNumbers.end();
            ++ian_iter)
        {
            // Create as many rules as destination action numbers
            for(unsigned j=0; j<ian_iter->second.size(); ++j)
            {
                // Search for participating inputs and add them to the rule:
                for(unsigned i=0; i<inputActionNames.size(); i++)
                {
                    // Look if this input is participating:
                    NameToNumb_t::const_iterator srcAN_iter =
                        inputActionNames[i].find(ian_iter->first);
                    // If so, output which action number it is using for this
                    // action:
                    if(srcAN_iter != inputActionNames[i].end())
                        pipe.lsts_Rule(i+1, srcAN_iter->second);
                }
                // Set the resulting action number:
                pipe.lsts_RuleDestinationTransitionNumber(ian_iter->second[j]);
            }
        }
    }

    // Action names section:
    // ---------------------
    lsts_index_t lsts_numberOfActionNamesToWrite()
    { return actionCount; }
    void lsts_WriteActionNames(iActionNamesAP& pipe)
    {
        // Output all resulting action names:
        for(NameToNumb_t::iterator iter=destActionNames.begin();
            iter!=destActionNames.end(); ++iter)
        {
            pipe.lsts_ActionName(iter->second, iter->first);
        }
    }
};

// Read action names to be renamed
//========================================================================
void ReadRenameActions(InStream& renamefile,
                       vector<RegExp>& hideActions,
                       vector<ActionRename>& renameActions)
{
    ITokenStream is(renamefile);
    is.SetPunctuation(";");

    try {
        while(!is.Peek().isEndOfFile())
        {
            string actionName = is.Get(TT::STRING).stringData();

            if(is.Peek().isString())
            {
                renameActions.push_back(ActionRename(actionName));
                do
                {
                    renameActions.back().newName.push_back
                        (is.Get(TT::STRING).stringData());
                } while(is.Peek().isString());
            }
            else
            {
                hideActions.push_back(actionName);
            }

            is.Get(TT::PUNCTUATION, ";");
        }
    }
    catch(int n)
    {
        cerr << "  In file: " << renamefile.GiveFilename() << endl;
        throw(n);
    }
}

void InvertHideNames( vector<RegExp> &hideActions,
		      const vector<map<string, lsts_index_t> > &actionNames,
		      const vector<ActionRename> &renameActions )
{
  vector<RegExp> visibleActions;
  visibleActions.swap(hideActions);
  set<string> found_names;
  typedef set<string> actNames_t;
  typedef map<string, lsts_index_t> NameToNumb_t;

  unsigned num_of_comps( actionNames.size() );
  for( unsigned idx=0; idx < num_of_comps ; ++idx )
    {
      for( NameToNumb_t::const_iterator act_iter =
	     actionNames[idx].begin();
	   act_iter != actionNames[idx].end();
	   ++act_iter )
	{
	  if( found_names.find(act_iter->first) == found_names.end() )
	    {
	      found_names.insert(actNames_t::value_type(act_iter->first));
	    }
	}
    }
  for( actNames_t::iterator next=found_names.begin();
       next != found_names.end();
       ++next )
    {
      if( searchMatch( *next, renameActions ) != renameActions.end() )
	continue;
      if( searchMatch( *next, visibleActions ) != visibleActions.end() )
	continue;
      // This action is not mentioned neither in rename or visible action set
      hideActions.push_back( *next );
    }
}

//========================================================================
// Rules file creation
//========================================================================
bool CreateRulesFile(CreateRulesCLP& clp)
{
    vector<map<string, lsts_index_t> > actionNames(clp.getFilenamesCnt()-1);
    vector<RegExp> hideActions;
    vector<ActionRename> renameActions;
    StatePropositionNamesReading propos;

    if(clp.rename())
        ReadRenameActions(clp.getRenameFile(), hideActions, renameActions);

    for(unsigned i=1; i<clp.getFilenamesCnt(); i++)
    {
        iLSTS_File is(clp.getInputStream(i));
        ActionNamesReading an(actionNames[i-1]);
        is.AddActionNamesReader(an);
	if ( clp.propositions() )
	  {
	    propos.start_PropositionsFromLSTS(i);
	    is.AddStatePropsReader(propos);
	  }
        is.SetNoReaderAction(iLSTS_File::IGNORE);
        is.ReadFile();
    }

    if( clp.visible() )
      InvertHideNames(hideActions, actionNames, renameActions);

    WriteRules(clp, actionNames, hideActions, renameActions, propos);

    return true;
}


//========================================================================
// Command line parsing
//========================================================================
int main(int argc, char* argv[])
{
    try
    {
        CreateRulesCLP clp;
        clp.setRequiredFilenamesCnt(2);
        if(!clp.parseCommandLine(argc, argv)) return 1;

        if(!CreateRulesFile(clp))
            return 1;

    } catch(...) { return 1; }

    return 0;
}
