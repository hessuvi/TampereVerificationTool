// LSTS source process editable with the graphical LSTS editor
// -----------------------------------------------------------

#ifndef LSTS_EDITOR_SOURCE_HH
#define LSTS_EDITOR__SOURCE_HH

#include "SourceProcess.hh"
#include "LSTSEditor.hh"

class LSTSEditorSource: public SourceProcess
{
 public:
    LSTSEditorSource(int col, int yCoord,
                     MainDrawingWindow& dWindow, const char* name);


//------------------------------------------------------------
 protected:
//------------------------------------------------------------
    virtual void drawMainPart(MainDrawingWindow& dWindow,
                              int col, int yCoord) const;

    virtual void edit();
    virtual void hasBeenRenamed(const char* newName);

    virtual TVTItem* clone(int newVerticalOffset, MainDrawingWindow& dWindow);

//------------------------------------------------------------
 private:
//------------------------------------------------------------
    LSTSEditor editor;

    LSTSEditorSource(const LSTSEditorSource&);
    LSTSEditorSource& operator=(const LSTSEditorSource&);
};

#endif
