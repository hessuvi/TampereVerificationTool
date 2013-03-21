#include "LSTSEditorMainDrawingWindow.hh"
#include "SystemTools.hh"

#include "LSTS_File/iLSTS_File.hh"
#include "LSTS_File/oLSTS_File.hh"

#include <cstdlib>
#include <cerrno>
#include <sstream>

// ========================================================================
// Read LSTS from file
// ========================================================================
bool LSTSEditorMainDrawingWindow::readLSTS(const std::string& filename,
                                           bool overrideCheck,
                                           bool setModifiedFlag,
                                           bool refreshWindow)
{
    InStream is(filename);
    iLSTS_File ifile(is);

    unsigned statesAmount = ifile.GiveHeader().GiveStateCnt();
    if(!overrideCheck && statesAmount > 50)
        return false;

    lstsHeader = ifile.GiveHeader();

    ifile.AddLayoutReader(*this);
    ifile.AddTransitionsReader(*this);
    ifile.AddActionNamesReader(*actionNameEditor);
    ifile.AddStatePropsReader(*this);
    IgnoreExtraData();

    deleteEverything();
    states.clear();
    statePointers.resize(statesAmount+1);
    State::resetStateNumberCounter(1);
    for(unsigned i = 0; i < statesAmount; ++i)
    {
        states.push_back(State());
        statePointers[i+1] = &states.back();
    }
    initialStateNumber = lstsHeader.GiveInitialState();

    stateNamesCounter = 0;

    ifile.ReadFile();

    std::vector<State*> tmp;
    statePointers.swap(tmp);

    if(stateNamesCounter != states.size())
        calculateLayout();
    else
        for(StatesIterator iter = states.begin(); iter != states.end(); ++iter)
        {
            iter->rearrangeTransitionsToSelf(true);
        }

    std::string styleFile = filename;
    styleFile += ".style";
    if(tools::fileExists(styleFile.c_str()))
        actionNameEditor->loadStyleFile(styleFile);

    if(refreshWindow)
        refreshAll();
    actionNameEditor->refreshAll();
    dataHasBeenModified = setModifiedFlag;
    actionNameEditor->setModifiedToFalse();

    return true;
}

// State names (state coordinates):
// -------------------------------
void LSTSEditorMainDrawingWindow::lsts_StartLayout(class Header&)
{}

void LSTSEditorMainDrawingWindow::lsts_StateLayout(lsts_index_t state_number,
                                                   int xCoord, int yCoord)
{
    statePointers[state_number]->x(xCoord);
    statePointers[state_number]->y(yCoord);
    ++stateNamesCounter;
}

void LSTSEditorMainDrawingWindow::lsts_EndLayout()
{}


// Transitions
// -----------
void LSTSEditorMainDrawingWindow::lsts_StartTransitions(class Header&)
{}

void LSTSEditorMainDrawingWindow::lsts_StartTransitionsFromState
(lsts_index_t /*start_state*/)
{}

void LSTSEditorMainDrawingWindow::lsts_Transition(lsts_index_t start_state,
                                                  lsts_index_t dest_state,
                                                  lsts_index_t action)
{
    if(start_state == dest_state)
      { // HACK: original line:
        // statePointers[start_state]->addTransitionToSelf(action, false);
	// now added "transition id" 42 to make the action visible:
	// (the same hack is applied to other addTransitionToSelfs too)
        statePointers[start_state]->addTransitionToSelf(42,action, false);
    }
    else
    {
        selectedActionNumber = action;
        createNewTransition(statePointers[start_state],
                            statePointers[dest_state], false);
    }
}

void LSTSEditorMainDrawingWindow::lsts_TransitionWithLayoutInfo
(lsts_index_t start_state, lsts_index_t dest_state,
 lsts_index_t action, const std::string& layout_info)
{
    if(start_state == dest_state)
    {
        int angle = std::atoi(layout_info.c_str());
        if(angle > 0)
            statePointers[start_state]->addTransitionToSelf(42,action, false,
                                                            angle-1);
        else
            statePointers[start_state]->addTransitionToSelf(42,action, false);
    }
    else
        lsts_Transition(start_state, dest_state, action);
}

void LSTSEditorMainDrawingWindow::lsts_EndTransitionsFromState(lsts_index_t
                                                               /*start_state*/)
{}

void LSTSEditorMainDrawingWindow::lsts_EndTransitions()
{
    selectedActionNumber = ~0U;
}


// State propositions
// ------------------
void LSTSEditorMainDrawingWindow::lsts_StartStateProps(class Header&)
{}

void LSTSEditorMainDrawingWindow::lsts_StartPropStates(const std::string&
                                                       prop_name)
{
    actionNameEditor->addStatePropName(prop_name, false);
}

void LSTSEditorMainDrawingWindow::lsts_PropState(lsts_index_t state)
{
    statePointers[state]->addStateProp(selectedStatePropNumber);
}

void LSTSEditorMainDrawingWindow::lsts_EndPropStates(const std::string&)
{}

void LSTSEditorMainDrawingWindow::lsts_EndStateProps()
{}


// ========================================================================
// Write lsts to file
// ========================================================================
void LSTSEditorMainDrawingWindow::saveLSTS(const std::string& filename)
{
    unsigned stateCounter = 1;
    for(StatesIterator iter = states.begin(); iter != states.end(); ++iter)
    {
        const unsigned stateNumber = iter->number();
        if(stateNumberToLSTSFileStateNumberLookup.size() <= stateNumber)
            stateNumberToLSTSFileStateNumberLookup.resize(stateNumber+1);
        stateNumberToLSTSFileStateNumberLookup[stateNumber] = stateCounter;
        ++stateCounter;
    }

    OutStream os(filename, true);
    oLSTS_File ofile;

    ofile.AddLayoutWriter(*this);
    ofile.AddTransitionsWriter(*this);
    ofile.AddActionNamesWriter(*actionNameEditor);
    ofile.AddStatePropsWriter(*this);

    lstsHeader.SetStateCnt(states.size());
    lstsHeader.SetInitialState
        (stateNumberToLSTSFileStateNumberLookup[initialStateNumber]);
    ofile.GiveHeader() = lstsHeader;

    ofile.WriteFile(os);

    actionNameEditor->saveStyleFile(filename+".style");

    dataHasBeenModified = false;
    actionNameEditor->setModifiedToFalse();
}


// State names (state coordinates):
// -------------------------------
bool LSTSEditorMainDrawingWindow::lsts_doWeWriteLayout()
{
    return true;
}

void LSTSEditorMainDrawingWindow::lsts_WriteLayout(iLayoutAP& writer)
{
    for(StatesIterator iter = states.begin(); iter != states.end(); ++iter)
    {
        writer.lsts_StateLayout
            (stateNumberToLSTSFileStateNumberLookup[iter->number()],
             iter->x(), iter->y());
    }
}


// Transitions
// -----------
lsts_index_t LSTSEditorMainDrawingWindow::lsts_numberOfTransitionsToWrite()
{
    unsigned amount = 0;

    for(StatesIterator iter = states.begin(); iter != states.end(); ++iter)
        amount += iter->transitionsToSelfAmount();

    for(TransitionsHoldersIter iter = transitionsHolders.begin();
        iter != transitionsHolders.end(); ++iter)
        amount += iter->transitionsAmount();

    return amount;
}

void LSTSEditorMainDrawingWindow::lsts_WriteTransitions(iTransitionsAP& writer)
{
    for(StatesIterator iter = states.begin(); iter != states.end(); ++iter)
        iter->writeTransitions(writer, stateNumberToLSTSFileStateNumberLookup);
}


// State propositions
// ------------------
lsts_index_t LSTSEditorMainDrawingWindow::lsts_numberOfStatePropsToWrite()
{
    return actionNameEditor->statePropNamesAmount();
}

void LSTSEditorMainDrawingWindow::lsts_WriteStateProps(iStatePropsAP& writer)
{
    const unsigned amount = actionNameEditor->statePropNamesAmount();

    std::vector<std::vector<StatesIterator> > spNumberToStateLookup(amount);

    for(StatesIterator iter = states.begin(); iter != states.end(); ++iter)
    {
        const std::vector<unsigned>& stateProps = iter->getStateProps();
        for(unsigned i = 0; i < stateProps.size(); ++i)
            spNumberToStateLookup[stateProps[i]].push_back(iter);
    }

    for(unsigned i = 0; i < amount; ++i)
    {
        writer.lsts_StartPropStates(actionNameEditor->getStatePropName(i));

        const std::vector<StatesIterator>& stateIters =
            spNumberToStateLookup[i];
        for(unsigned j = 0; j < stateIters.size(); ++j)
        {
            writer.lsts_PropState
                (stateNumberToLSTSFileStateNumberLookup
                 [stateIters[j]->number()]);
        }

        writer.lsts_EndPropStates(actionNameEditor->getStatePropName(i));
    }
}
