// The text editor of the graphical ilang editor
// ---------------------------------------------

#ifndef ILANG_EDITOR_TEXT_WINDOW_HH
#define ILANG_EDITOR_TEXT_WINDOW_HH

#include "LSTSEditorMainDrawingWindow.hh"

#ifndef MAKEDEPEND
#include <gtk/gtk.h>
#include <list>
#endif

class ILangEditorTextWindow: public LSTSEditorMainDrawingWindow::EventNotify
{
 public:
    ILangEditorTextWindow();
    virtual ~ILangEditorTextWindow();

    GtkWidget* initialize(GtkWindow* parentWindow);


//--------------------------------------------------------------------------
 private:
//--------------------------------------------------------------------------
    class TransitionPropertiesDialog;

    GtkWindow* parent;
    GtkWidget* textView;
    GtkTextBuffer* textBuffer;

    GtkTextTag* highlightTag;

    class Transition
    {
        unsigned id;
        GtkTextBuffer* textBuffer;
        GtkTextMark* textBeginMark;
        GtkTextMark* precondBeginMark;
        GtkTextMark* precondEndMark;
        GtkTextMark* nameBeginMark;
        GtkTextMark* nameEndMark;
        GtkTextMark* postcondBeginMark;
        GtkTextMark* postcondEndMark;
        GtkTextMark* textEndMark;

        const gchar* getText(GtkTextMark* beginMark, GtkTextMark* endMark);
        void setText(GtkTextMark* beginMark, GtkTextMark* endMark,
                     const gchar*);

     public:
        Transition(unsigned ID, GtkTextBuffer* textBuf,
                   GtkTextMark* where, const char* actionName,
                   const char* destStateName);

        unsigned getID() const { return id; }
        GtkTextMark* getPrecondBeginMark() const { return precondBeginMark; }

        const gchar* getPrecondText();
        const gchar* getActionName();
        const gchar* getActionParameters();
        const gchar* getPostcondText();
        void setPrecondText(const gchar*);
        void setActionName(const gchar*);
        void setActionParameters(const gchar*);
        void setPostcondText(const gchar*);

        void highlight(GtkTextTag*);
        void unhighlight(GtkTextTag*);
    };

    class State
    {
        unsigned stateNumber;
        std::string name;
        GtkTextMark* textBeginMark;
        GtkTextMark* stateNameEndMark;
        GtkTextMark* textEndMark;
        GtkTextMark* transitionsTextEndMark;

        std::list<Transition> transitions;

     public:
        State(unsigned n = 0, GtkTextBuffer* textBuffer = 0);
        unsigned number() { return stateNumber; }
        void remove(GtkTextBuffer* textBuffer);

        void addTransition(unsigned id, State* endState,
                           const char* actionName,
                           GtkTextBuffer* textBuffer);

        void highlight(GtkTextBuffer* textBuffer, GtkTextTag*);
        void unhighlight(GtkTextBuffer* textBuffer, GtkTextTag*);
        void scrollToScreen(GtkWidget* textView, GtkTextBuffer* textBuffer,
                            unsigned transitionID = 0);

        Transition* getTransition(unsigned id);
    };

    std::list<State> states;
    std::list<State>::iterator highlightedState;

    Transition* selectedTransition;

    std::vector<std::string> actionNames;
    std::vector<std::string> statePropNames;

    void selectTransition(unsigned id);

    // LSTSEditorMainDrawingWindow::EventNotify callback functions:
    virtual void stateCreated(unsigned stateNumber, bool isInitialState);
    virtual void stateSelected(unsigned stateNumber);
    virtual void stateDeleted(unsigned stateNumber);
    virtual void stateWasMadeInitial(unsigned stateNumber);

    virtual void statePropAddedToState(unsigned stateNumber,
                                       unsigned statePropNumber,
                                       const char* statePropName);
    virtual void statePropRemovedFromState(unsigned stateNumber,
                                           unsigned statePropNumber);
    virtual void statePropRenamed(unsigned statePropNumber,
                                  const char* newName);
    virtual void statePropRemoved(unsigned statePropNumber);

    virtual void transitionCreated(unsigned id,
                                   unsigned startStateNumber,
                                   unsigned endStateNumber,
                                   unsigned action,
                                   const char* actionName);
    virtual void transitionSelected(unsigned id);
    virtual void transitionUnselected(unsigned id);
    virtual void transitionDeleted(unsigned id);
    virtual void transitionActionChanged(unsigned id,
                                         unsigned newAction);
    virtual void editTransitionProperties(unsigned id);

    virtual void actionNameChanged(unsigned actionNumber,
                                   const char* newActionName);
    virtual void actionNameDeleted(unsigned actionNumber);

    virtual void deleteEverything();
};

#endif
