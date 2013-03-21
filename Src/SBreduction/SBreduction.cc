#include "SBreducer.hh"

#include "TvtCLP.hh"
#include "InputLSTSContainer.hh"

int main(int argc, char* argv[])
{
#ifndef NO_EXCEPTIONS
  try {
#endif
    TvtCLP clp
        ("Calculates the minimal LSTS equivalent to the input LSTS according\n"
         "to strong bisimilarity formalism.");

    if(!clp.parseCommandLine(argc, argv)) return 1;

    // Try to alleviate memory requirements by allocating and deallocating
    // the data containers as soon as they are not needed anymore:

    InputLSTSContainer* ilsts =
        new InputLSTSContainer(ILC::STATEPROPS | ILC::TRANSITIONS |
                               ILC::ACCSETS | ILC::DIVBITS);
    ilsts->readLSTSFile(clp.getInputStream());

    SBreducer sbReducer(*ilsts);

    delete ilsts; ilsts = 0;

    sbReducer.reduce(clp.getOutputStream());

#ifndef NO_EXCEPTIONS
  } catch(...) { return 1; }
#endif
    return 0;
}
