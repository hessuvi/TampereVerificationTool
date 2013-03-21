// The main drawing/editing window of the graphical LSTS editor
// ------------------------------------------------------------

#ifndef LSTS_EDITOR_MAIN_DRAWING_WINDOW_HH
#define LSTS_EDITOR_MAIN_DRAWING_WINDOW_HH

#include "ActionNameEditor.hh"
#include "PopupMenu.hh"

#include "LSTS_File/LayoutAP.hh"
#include "LSTS_File/TransitionsAP.hh"
#include "LSTS_File/StatePropsAP.hh"
#include "LSTS_File/Header.hh"

#ifndef MAKEDEPEND
#include <gtk/gtk.h>
#include <vector>
#include <list>
#include <set>
#include <string>
#endif

//======================================================================
class LSTSEditorMainDrawingWindow:
//======================================================================
    public ActionNameEditor::AttributeChangeCallBack,
    public iLayoutAP, public oLayoutAP,
    public iTransitionsAP, public oTransitionsAP,
    public iStatePropsAP, public oStatePropsAP
{
 public:
    LSTSEditorMainDrawingWindow(bool iLangEditor = false);
    ~LSTSEditorMainDrawingWindow();

    GtkWidget* initialize(GtkWindow* parentWindow,
                          ActionNameEditor* editor,
                          class StatusBar* mainStatusBar);


    void deleteEverything();
    void calculateLayout();
    bool readLSTS(const std::string& filename, bool overrideCheck,
                  bool setModifiedFlag, bool refreshWindow = false);
    void saveLSTS(const std::string& filename);

    inline bool hasBeenModified() const;


    class EventNotify
    {
     public:
        virtual void stateCreated(unsigned stateNumber,
                                  bool isInitialState) = 0;
        virtual void stateSelected(unsigned stateNumber) = 0;
        virtual void stateDeleted(unsigned stateNumber) = 0;
        virtual void stateWasMadeInitial(unsigned stateNumber) = 0;

        virtual void statePropAddedToState(unsigned stateNumber,
                                           unsigned statePropNumber,
                                           const char* statePropName) = 0;
        virtual void statePropRemovedFromState(unsigned stateNumber,
                                               unsigned statePropNumber) = 0;
        virtual void statePropRenamed(unsigned statePropNumber,
                                      const char* newName) = 0;
        virtual void statePropRemoved(unsigned statePropNumber) = 0;

        virtual void transitionCreated(unsigned id,
                                       unsigned startStateNumber,
                                       unsigned endStateNumber,
                                       unsigned action,
                                       const char* actionName) = 0;
        virtual void transitionSelected(unsigned id) = 0;
        virtual void transitionUnselected(unsigned id) = 0;
        virtual void transitionDeleted(unsigned id) = 0;
        virtual void transitionActionChanged(unsigned id,
                                             unsigned newAction) = 0;
        virtual void editTransitionProperties(unsigned id) = 0;

        virtual void actionNameChanged(unsigned actionNumber,
                                       const char* newActionName) = 0;
        virtual void actionNameDeleted(unsigned actionNumber) = 0;

        virtual void deleteEverything() = 0;
    };

    inline void registerEventListener(EventNotify* listener);




//--------------------------------------------------------------------------
 private:
//--------------------------------------------------------------------------

    // Window handling related variables:
    // ---------------------------------
    bool thisIsAnILangEditor;
    GtkWindow* parent;
    ActionNameEditor* actionNameEditor;
    class StatusBar* statusBar;
    GtkWidget* drawingArea;
    GdkPixmap* pixmap;
    GdkGC* drawingGC;
    GdkGC* highlightedTransitionGC;
    GdkColormap* colorMap;
    int pixmapWidth, pixmapHeight, origoX, origoY;

    int clickedX, clickedY;

    PopupMenu backgroundPopupMenu, statePopupMenu;

    EventNotify* eventListener;

    // Initialization and window handling related methods:
    // --------------------------------------------------
    void initializeLSTSData();

    void setDrawingColor(unsigned red, unsigned green, unsigned blue);
    void recalculatePixmapSize();
    void redrawPixmap();
    void clearArea(int x, int y, int width, int height);
    void refreshArea(int x=0, int y=0, int width=0, int height=0);
    void refreshAll();

    static gboolean configureEvent(GtkWidget*, GdkEventConfigure*,
                                   LSTSEditorMainDrawingWindow*);
    static gboolean exposeEvent(GtkWidget*, GdkEventExpose*,
                                LSTSEditorMainDrawingWindow*);
    static gboolean mouseButtonPressEvent(GtkWidget*, GdkEventButton*,
                                          LSTSEditorMainDrawingWindow*);
    static gboolean mouseButtonReleaseEvent(GtkWidget*, GdkEventButton*,
                                            LSTSEditorMainDrawingWindow*);
    static gboolean motionEvent(GtkWidget*, GdkEventMotion*,
                                LSTSEditorMainDrawingWindow*);
    static gboolean leaveEvent(GtkWidget*, GdkEventCrossing*,
                               LSTSEditorMainDrawingWindow*);


    static const GtkItemFactoryEntry backgroundPopupMenuItems[];
    static const GtkItemFactoryEntry backgroundPopupMenuILangItems[];
    static const GtkItemFactoryEntry statePopupMenuItems[];
    void updateBackgroundMenuSensitivity();
    void updateStateMenuSensitivity();
    static void newStateCallBack(gpointer, guint, GtkWidget*);
    static void changeActionOfTransitionCallBack(gpointer, guint, GtkWidget*);
    static void removeTransitionCallBack(gpointer, guint, GtkWidget*);
    static void unselectTransitionCallBack(gpointer, guint, GtkWidget*);
    static void removeStateCallBack(gpointer, guint, GtkWidget*);
    static void addStatePropCallBack(gpointer, guint, GtkWidget*);
    static void makeInitialStateCallBack(gpointer, guint, GtkWidget*);
    static void createNewTransitionCallBack(gpointer, guint, GtkWidget*);
    static void rearrangeTransitionsToSelfCallBack(gpointer,guint,GtkWidget*);
    static void editTransitionPropertiesCallBack(gpointer,guint,GtkWidget*);


    class TransitionsHolder;


    // State
    // -----
    class State
    {
        struct TransitionToSelf
        {
            unsigned ID; // > 0
            unsigned actionNumber;
            // Coordinates relative to the center of the state:
            int centerX, centerY, coord1X, coord1Y, coord2X, coord2Y;

            TransitionToSelf(unsigned id, unsigned an):
                ID(id), actionNumber(an) {}
        };

        static unsigned stateNumberCounter;
        unsigned stateNumber;
        std::vector<const TransitionsHolder*> transitionsHolders;
        std::vector<TransitionToSelf> transitionsToSelf;
        std::vector<unsigned> stateProps;
        int xCoord, yCoord;
        unsigned selectedTransition;
        bool isBeingRemoved;

        void calculateCoordinates(TransitionToSelf& tr, double angle);
        unsigned getTransitionIndex(int coordX, int coordY) const;

     public:
        State(int ix=0, int iy=0);
        static void resetStateNumberCounter(unsigned value = 0);

        // id > 0
        // angle = 0-360; -1 = unset
        bool addTransitionToSelf(unsigned id, unsigned actionNumber,
                                 bool calculateOrientation = true,
                                 int angle = -1);

        void rearrangeTransitionsToSelf(bool
                                        onlyThoseWithoutOrientation = false);
        void addTransitionsHolder(const TransitionsHolder&);
        void removeTransitionsHolder(const TransitionsHolder&);
        void addStateProp(unsigned stateProp);
        void removeStateProp(unsigned stateProp, bool renumber);
        void draw(GdkPixmap*, GdkGC*, ActionNameEditor*,
                  int origoX, int origoY, bool drawTransitions=true) const;
        void drawSelectedTransition(GdkPixmap*, ActionNameEditor*,
                                    GdkGC* highlightGC,
                                    int origoX, int origoY) const;
        void erase(GdkPixmap*, GdkGC*, int origoX, int origoY) const;

        // Retval 0 = no selection, else the id of the selected transition:
        unsigned selectTransition(int clickX, int clickY);

        bool canChangeSelectedTransitionActionNumber(unsigned
                                                     actionNumber) const;
        void changeSelectedTransitionActionNumber(unsigned actionNumber);
        void removeSelectedTransition();
        void reorientSelectedTransition(int dirx, int diry);
        void getSelectedTransitionBoundaries(int& minx, int& miny,
                                             int& maxx, int& maxy) const;
        void getBoundaries(int& minx, int& miny,
                           int& maxx, int& maxy) const;
        void removeTransition(unsigned actionNumber,
                              EventNotify* eventListener);
        void selectStateProp(int clickX, int clickY, ActionNameEditor*) const;

        inline bool coordinatesInsideState(int coordX, int coordY) const;
        bool coordinatesInsideTransitionHandle(int coordX,
                                               int coordY) const;

        int x() const { return xCoord; }
        int y() const { return yCoord; }
        void x(int newX) { xCoord = newX; }
        void y(int newY) { yCoord = newY; }
        unsigned number() const { return stateNumber; }
        unsigned getSelectedTransitionActionNumber() const
        { return transitionsToSelf[selectedTransition].actionNumber; }
        unsigned getSelectedTransitionID() const
        { return transitionsToSelf[selectedTransition].ID; }
        bool hasTransitions() const { return !transitionsToSelf.empty(); }
        unsigned transitionsToSelfAmount() const
        { return transitionsToSelf.size(); }
        const std::vector<unsigned>& getStateProps() const
        { return stateProps; }
        inline bool hasStateProp(unsigned stateProp) const;

        template<typename TransitionsHoldersContainer>
        void removeAllTransitionsHolders(TransitionsHoldersContainer& holders,
                                         EventNotify* eventListener);
        void removeAllTransitionsToSelf(EventNotify* eventListener);
        void writeTransitions(class iTransitionsAP& pipe,
                              const std::vector<unsigned>& stateNumberLookup);
    };

    // Transition (in TransitionsHolder)
    // ----------
    struct Transition
    {
        unsigned ID; // > 0
        unsigned actionNumber;
        unsigned startStateIndex; // 0 or 1
        int midx, midy;

        Transition(unsigned id=0, unsigned an=0, unsigned ssi=0):
            ID(id), actionNumber(an), startStateIndex(ssi) {}

        bool operator==(const Transition& rhs) const
        { return actionNumber == rhs.actionNumber &&
              startStateIndex == rhs.startStateIndex; }
    };

    // TransitionsHolder
    // -----------------
    class TransitionsHolder
    {
     public:
        TransitionsHolder();
        TransitionsHolder(State* s1, State* s2);

        const State* getState(unsigned which) const { return state[which]; }
        int x() const { return (state[0]->x()+state[1]->x())/2; };
        int y() const { return (state[0]->y()+state[1]->y())/2; };

        bool operator<(const TransitionsHolder& rhs) const
        {
            return state[0]->number() == rhs.state[0]->number() ?
                state[1]->number() < rhs.state[1]->number() :
                state[0]->number() < rhs.state[0]->number();
        }

        bool addTransition(const Transition& t) const;
        void draw(GdkPixmap*, ActionNameEditor*, int origoX, int origoY) const;
        bool coordinatesInsideTransitionHandle(int coordX,
                                               int coordY) const;
        // Retval 0 = no selection, else id of the selected transition:
        unsigned selectTransition(int clickX, int clickY) const;
        void drawSelectedTransition(GdkPixmap*, ActionNameEditor*,
                                    GdkGC* highlightGC,
                                    int origoX, int origoY) const;

        const Transition& getSelectedTransition() const
        { return transitions[selectedTransition]; }

        bool canChangeSelectedTransitionActionNumber(unsigned
                                                     actionNumber) const;
        void changeSelectedTransitionActionNumber(unsigned actionNumber) const;
        void removeSelectedTransition() const;
        void removeTransition(unsigned actionNumber,
                              EventNotify* eventListener) const;

        void disconnect(EventNotify* eventListener) const;

        unsigned transitionsAmount() const { return transitions.size(); }
        const std::vector<Transition>& getTransitions() const
        { return transitions; }

     private:
        State* state[2];
        mutable std::vector<Transition> transitions;
        // "Bounding box" of the handlers (for quick discard of hover):
        mutable int minHandlerX, minHandlerY, maxHandlerX, maxHandlerY;
        mutable unsigned selectedTransition;

        unsigned getTransitionIndex(int coordX, int coordY) const;
    };

    // LSTS data:
    // ---------
    static unsigned transitionIdCounter;
    std::list<State> states;
    typedef std::list<State>::iterator StatesIterator;
    std::set<TransitionsHolder> transitionsHolders;
    typedef std::set<TransitionsHolder>::iterator TransitionsHoldersIter;
    unsigned initialStateNumber;
    StatesIterator clickedStateIter;
    unsigned selectedActionNumber, selectedStatePropNumber;

    State* stateBeingDragged;
    int draggedStateOriginalX, draggedStateOriginalY;

    State* transitionCreationStartState;
    class LineDrag* transitionCreationLine;

    const TransitionsHolder* selectedTransitionsHolder;
    State* stateWithSelectedTransition;
    bool selectedTransitionIsBeingDragged, dataHasBeenModified;

    unsigned getSelectedTransitionID() const
    {
        if(selectedTransitionsHolder)
            return selectedTransitionsHolder->getSelectedTransition().ID;
        else if(stateWithSelectedTransition)
            return stateWithSelectedTransition->getSelectedTransitionID();
        return 0;
    }

    // LSTS handling methods:
    // ---------------------
    void removeState(StatesIterator);
    void startCreatingNewTransition(StatesIterator);
    void cancelNewTransitionCreation();
    void createNewTransition(State* startState, State* endState,
                             bool refreshWindow = true);

    // Callback functions for ActionNameEditor:
    // ---------------------------------------
    virtual void actionNameSelect(unsigned actionNumber);
    virtual void actionNameAttributesChanged(unsigned actionNumber);
    virtual void actionNameRemoved(unsigned actionNumber);
    virtual void statePropNameSelect(unsigned spNumber);
    virtual void statePropNameChanged(unsigned spNumber);
    virtual void statePropNameAttributesChanged(unsigned spNumber);
    virtual void statePropNameRemoved(unsigned spNumber);


    // LSTS reading and writing methods
    // --------------------------------
    Header lstsHeader;
    std::vector<State*> statePointers;
    unsigned stateNamesCounter;
    virtual void lsts_StartLayout(Header&);
    virtual void lsts_StateLayout(lsts_index_t state_number,
                                  int x, int y);
    virtual void lsts_EndLayout();

    virtual void lsts_StartTransitions(class Header&);
    virtual void lsts_StartTransitionsFromState(lsts_index_t start_state);
    virtual void lsts_Transition(lsts_index_t start_state,
                                 lsts_index_t dest_state,
                                 lsts_index_t action);
    virtual void lsts_TransitionWithLayoutInfo(lsts_index_t start_state,
                                               lsts_index_t dest_state,
                                               lsts_index_t action,
                                               const std::string& layout_info);
    virtual void lsts_EndTransitionsFromState(lsts_index_t start_state);
    virtual void lsts_EndTransitions();

    virtual void lsts_StartStateProps(class Header&);
    virtual void lsts_StartPropStates(const std::string& prop_name);
    virtual void lsts_PropState(lsts_index_t state);
    virtual void lsts_EndPropStates(const std::string& prop_name);
    virtual void lsts_EndStateProps();


    std::vector<unsigned> stateNumberToLSTSFileStateNumberLookup;
    virtual bool lsts_doWeWriteLayout();
    virtual void lsts_WriteLayout(iLayoutAP& writer);

    virtual lsts_index_t lsts_numberOfTransitionsToWrite();
    virtual void lsts_WriteTransitions(iTransitionsAP& pipe);

    virtual lsts_index_t lsts_numberOfStatePropsToWrite();
    virtual void lsts_WriteStateProps(iStatePropsAP& writer);


    // Forbid copying:
    LSTSEditorMainDrawingWindow(const LSTSEditorMainDrawingWindow&);
    LSTSEditorMainDrawingWindow& operator=(const LSTSEditorMainDrawingWindow&);
};

// Inline method implementations
// -----------------------------
template<typename TransitionsHoldersContainer>
void LSTSEditorMainDrawingWindow::State::removeAllTransitionsHolders
(TransitionsHoldersContainer& holders, EventNotify* eventListener)
{
    isBeingRemoved = true;
    for(unsigned i = 0; i < transitionsHolders.size(); ++i)
    {
        transitionsHolders[i]->disconnect(eventListener);
        holders.erase(*transitionsHolders[i]);
    }
    transitionsHolders.clear();
    isBeingRemoved = false;
}

inline bool LSTSEditorMainDrawingWindow::State::coordinatesInsideState
(int coordX, int coordY) const
{
    const int dx = coordX - x();
    const int dy = coordY - y();
    return (dx*dx + dy*dy) <=
        (LSTS_EDITOR_STATE_RADIUS*LSTS_EDITOR_STATE_RADIUS);
}

inline bool
LSTSEditorMainDrawingWindow::State::hasStateProp(unsigned stateProp) const
{
    for(unsigned i = 0; i < stateProps.size(); ++i)
        if(stateProps[i] == stateProp)
            return true;
    return false;
}

inline bool LSTSEditorMainDrawingWindow::hasBeenModified() const
{
    return dataHasBeenModified || actionNameEditor->hasBeenModified();
}

inline void LSTSEditorMainDrawingWindow::registerEventListener
(EventNotify* listener)
{
    eventListener = listener;
}

#endif
