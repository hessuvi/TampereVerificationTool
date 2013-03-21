#include "BBreducer.hh"

#include "TvtCLP.hh"

#include "../TauReduction/TauReductionClass.cc"

int main(int argc, char* argv[])
{
#ifndef NO_EXCEPTIONS
  try {
#endif
    TvtCLP clp
        ("Calculates the minimal LSTS equivalent to the input LSTS according\n"
         "to branching bisimilarity formalism.");

    if(!clp.parseCommandLine(argc, argv)) return 1;

    // Try to alleviate memory requirements by allocating and deallocating
    // the data containers as soon as they are not needed anymore:

    InputLSTSContainer* ilsts =
        new InputLSTSContainer(ILC::STATEPROPS | ILC::TRANSITIONS);
    ilsts->readLSTSFile(clp.getInputStream());

    TauReduction* tauReducer = new TauReduction(*ilsts);
    tauReducer->reduce();

    ActionNamesStore actionNames = ilsts->getActionNamesStore();
    Header header = ilsts->getHeaderData();
    unsigned initialStateNumber = tauReducer->getInitialStateNumber();

    delete ilsts; ilsts = 0;

    BBreducer bbReducer(tauReducer->getStatesAmount(),
                        initialStateNumber,
                        tauReducer->getTransitions(),
                        tauReducer->getStateProps(),
                        actionNames);

    delete tauReducer; tauReducer = 0;

    bbReducer.reduce(clp.getOutputStream(), header);

#ifndef NO_EXCEPTIONS
  } catch(...) { return 1; }
#endif
    return 0;
}
