// Input language source process
// -----------------------------

#ifndef INPUT_LANGUAGE_SOURCE_HH
#define INPUT_LANGUAGE_SOURCE_HH

#include "SourceProcess.hh"

class InputLanguageSource: public SourceProcess
{
 public:
    InputLanguageSource(int col, int yCoord,
                        MainDrawingWindow& dWindow, const char* name);


//------------------------------------------------------------
 protected:
//------------------------------------------------------------
    virtual void drawMainPart(MainDrawingWindow& dWindow,
                              int col, int yCoord) const;

    virtual void edit();

    virtual TVTItem* clone(int newVerticalOffset, MainDrawingWindow& dWindow);

//------------------------------------------------------------
 private:
//------------------------------------------------------------
    InputLanguageSource(const InputLanguageSource&);
    InputLanguageSource& operator=(const InputLanguageSource&);
};

#endif
