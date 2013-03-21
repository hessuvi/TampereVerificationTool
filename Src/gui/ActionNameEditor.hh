// Action name editor
// ------------------
// Subwindow which can show and optionally edit action names and state
// proposition names.

#ifndef ACTION_NAME_EDITOR_HH
#define ACTION_NAME_EDITOR_HH

#include "TextDraw.hh"
#include "PopupMenu.hh"

#include "LSTS_File/ActionNamesAP.hh"

#ifndef MAKEDEPEND
#include <gtk/gtk.h>
#include <vector>
#endif

class ActionNameEditor: public iActionNamesAP, public oActionNamesAP
{
 public:
    ActionNameEditor();
    ~ActionNameEditor();

    GtkWidget* initialize(GtkWindow* parentWindow,
                          class StatusBar* parentWindowStatusBar,
                          bool editable);

    void setGutterPosition(int percentage);
    int getGutterPosition() const;


    class AttributeChangeCallBack
    {
     public:
        virtual void actionNameSelect(unsigned actionNumber) = 0;
        virtual void actionNameAttributesChanged(unsigned actionNumber) = 0;
        virtual void actionNameRemoved(unsigned actionNumber) = 0;
        virtual void statePropNameSelect(unsigned spNumber) = 0;
        virtual void statePropNameChanged(unsigned spNumber) = 0;
        virtual void statePropNameAttributesChanged(unsigned spNumber) = 0;
        virtual void statePropNameRemoved(unsigned spNumber) = 0;

        virtual ~AttributeChangeCallBack() {}
    };

    void registerAttributeChangeCallBack(AttributeChangeCallBack*);

    inline GdkGC* getActionGC(unsigned actionNumber);
    inline GdkGC* getStatePropGC(unsigned spNumber);

    void selectActionName(unsigned actionNumber);
    void selectStatePropName(unsigned spNumber);

    void addStatePropName(const std::string& name, bool refreshWindow);
    inline unsigned statePropNamesAmount();
    inline const char* getActionName(unsigned actionNumber);
    inline const char* getStatePropName(unsigned spNumber);

    void refreshAll();
    void deleteAll();

    inline bool hasBeenModified() const;
    inline void setModifiedToFalse();

    void loadStyleFile(const std::string& filename);
    void saveStyleFile(const std::string& filename) const;


//----------------------------------------------------------------------
 private:
//----------------------------------------------------------------------
    class ActionPropertiesDialog;

    struct ActionName
    {
        TextDraw name;
        GdkColor color;
        std::vector<gint8> dashPattern;

        ActionName(GtkWindow* parentWindow, const char* n):
            name(parentWindow, n, -1, false)
        {
            color.red = color.green = color.blue = 0;
        }
    };

    struct StatePropName
    {
        TextDraw name;
        GdkColor color;

        StatePropName(GtkWindow* parentWindow, const char* n):
            name(parentWindow, n, -1, false)
        {
            color.red = color.green = color.blue = 0x8000;
        }
    };

    std::vector<ActionName> actionNames;
    std::vector<StatePropName> statePropNames;
    unsigned selectedActionName, selectedStatePropName;

    GtkWindow* parent;
    class StatusBar* statusBar;
    GtkWidget* panedWindow;
    GdkPixmap* actionsPixmap;
    GtkWidget* actionsDrawingArea;
    GdkPixmap* statePropsPixmap;
    GtkWidget* statePropsDrawingArea;
    GdkGC* lineGC;
    GdkGC* circleGC;
    GdkGC* selectionGC;
    GdkColormap* colorMap;
    int actionsPixmapWidth, actionsPixmapHeight;
    int statePropsPixmapWidth, statePropsPixmapHeight;
    mutable bool dataHasBeenModified;

    AttributeChangeCallBack* callbacks;

    PopupMenu actionsPopupMenu, statePropsPopupMenu;
    void updateActionsPopupMenuSensitivity();
    void updateStatePropsPopupMenuSensitivity();

    bool editAction(ActionName& action, bool nameIsEditable) const;

    void setActionNameStyle(const std::string& name,
                            unsigned r, unsigned g, unsigned b,
                            const std::string& dashPattern);
    void setStatePropNameStyle(const std::string& name,
                               unsigned r, unsigned g, unsigned b);

    void setColorAndDash(ActionName& data) const;
    void setColorAndDash(int r, int g, int b) const;
    void setColor(StatePropName& data) const;
    void redrawActionsPixmap();
    void redrawStatePropsPixmap();
    void recalculateActionsPixmapSize();
    void recalculateStatePropsPixmapSize();
    void refreshActionsWindow(int x=0, int y=0, int width=0, int height=0);
    void refreshStatePropsWindow(int x=0, int y=0, int width=0, int height=0);

    static gboolean actionsConfigureEvent(GtkWidget*, GdkEventConfigure*,
                                          ActionNameEditor*);
    static gboolean actionsExposeEvent(GtkWidget*, GdkEventExpose*,
                                       ActionNameEditor*);
    static gboolean actionsMouseButtonPressEvent(GtkWidget*,
                                                 GdkEventButton* press,
                                                 ActionNameEditor*);
    static gboolean statePropsConfigureEvent(GtkWidget*, GdkEventConfigure*,
                                             ActionNameEditor*);
    static gboolean statePropsExposeEvent(GtkWidget*, GdkEventExpose*,
                                          ActionNameEditor*);
    static gboolean statePropsMouseButtonPressEvent(GtkWidget*,
                                                    GdkEventButton* press,
                                                    ActionNameEditor*);
    static gboolean enterNotifyEvent(GtkWidget*, GdkEventCrossing*,
                                     ActionNameEditor*);
    static gboolean leaveNotifyEvent(GtkWidget*, GdkEventCrossing*,
                                     ActionNameEditor*);


    static const GtkItemFactoryEntry editableActionNameEditorPopupMenuItems[];
    static
    const GtkItemFactoryEntry noneditableActionNameEditorPopupMenuItems[];
    static
    const GtkItemFactoryEntry editableStatePropNameEditorPopupMenuItems[];
    static
    const GtkItemFactoryEntry noneditableStatePropNameEditorPopupMenuItems[];

    static void newActionCallback(gpointer, guint, GtkWidget*);
    static void editActionCallback(gpointer, guint, GtkWidget*);
    static void removeActionCallback(gpointer, guint, GtkWidget*);
    static void renameStatePropCallback(gpointer, guint, GtkWidget*);
    static void changeStatePropColorCallback(gpointer, guint, GtkWidget*);
    static void removeStatePropCallback(gpointer, guint, GtkWidget*);
    static void newStatePropCallback(gpointer, guint, GtkWidget*);


    virtual void lsts_StartActionNames(Header&);
    virtual void lsts_ActionName(lsts_index_t action_number,
                                 const std::string& action_name);
    virtual void lsts_EndActionNames();
    virtual lsts_index_t lsts_numberOfActionNamesToWrite();
    virtual void lsts_WriteActionNames(iActionNamesAP& writer);


    ActionNameEditor(const ActionNameEditor&);
    ActionNameEditor& operator=(const ActionNameEditor&);
};


inline GdkGC* ActionNameEditor::getActionGC(unsigned actionNumber)
{
    setColorAndDash(actionNames[actionNumber]);
    return lineGC;
}

inline GdkGC* ActionNameEditor::getStatePropGC(unsigned spNumber)
{
    setColor(statePropNames[spNumber]);
    return circleGC;
}

inline unsigned ActionNameEditor::statePropNamesAmount()
{
    return statePropNames.size();
}

inline const char* ActionNameEditor::getActionName(unsigned actionNumber)
{
    return actionNames[actionNumber].name.getText();
}

inline const char* ActionNameEditor::getStatePropName(unsigned spNumber)
{
    return statePropNames[spNumber].name.getText();
}

inline bool ActionNameEditor::hasBeenModified() const
{
    return dataHasBeenModified;
}

inline void ActionNameEditor::setModifiedToFalse()
{
    dataHasBeenModified = false;
}

#endif
