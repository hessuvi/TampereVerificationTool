#include "ActionNameEditor.hh"
#include "StatusBar.hh"
#include "DrawingTools.hh"
#include "constants.hh"

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <fstream>


//===========================================================================
// ActionPropertiesDialog
//===========================================================================
// Dialog for editing action properties (name, color and dash)
// -----------------------------------------------------------
class ActionNameEditor::ActionPropertiesDialog
{
 public:
    ActionPropertiesDialog(const ActionNameEditor& editor);
    ~ActionPropertiesDialog();

    bool run(ActionName& action, bool nameIsEditable);



 private:
    const ActionNameEditor& owner;
    ActionName* actionBeingEdited;
    GtkWidget* sampleDrawingArea;
    GtkWidget* colorButtonDrawingArea;
    GtkWidget* dashPatternDrawingArea;
    std::vector<bool> dash;
    GdkGC* colorButtonGC;

    void drawSample();
    void drawColorButton();
    void drawDashPattern();
    void calculateDash();

    static gboolean sampleExposeEvent(GtkWidget*, GdkEventExpose*,
                                      ActionPropertiesDialog*);
    static gboolean colorButtonExposeEvent(GtkWidget*, GdkEventExpose*,
                                           ActionPropertiesDialog*);
    static void colorButtonClickedEvent(GtkButton*, ActionPropertiesDialog*);
    static void decreaseButtonClickedEvent(GtkButton*,
                                           ActionPropertiesDialog*);
    static void increaseButtonClickedEvent(GtkButton*,
                                           ActionPropertiesDialog*);
    static gboolean dashPatternExposeEvent(GtkWidget*, GdkEventExpose*,
                                           ActionPropertiesDialog*);
    static gboolean dashPatternClickEvent(GtkWidget*, GdkEventButton*,
                                          ActionPropertiesDialog*);

    ActionPropertiesDialog(const ActionPropertiesDialog&);
    ActionPropertiesDialog& operator=(const ActionPropertiesDialog&);
};

ActionNameEditor::ActionPropertiesDialog::ActionPropertiesDialog
(const ActionNameEditor& editor):
    owner(editor), actionBeingEdited(0),
    sampleDrawingArea(0), colorButtonDrawingArea(0), dashPatternDrawingArea(0),
    colorButtonGC(0)
{}

ActionNameEditor::ActionPropertiesDialog::~ActionPropertiesDialog()
{
    g_object_unref(colorButtonGC);
}

void ActionNameEditor::ActionPropertiesDialog::calculateDash()
{
    const std::vector<gint8>& dashPattern = actionBeingEdited->dashPattern;
    if(dashPattern.empty())
    {
        dash.push_back(true);
        return;
    }
    bool value = true;
    for(unsigned i = 0; i < dashPattern.size(); ++i)
    {
        for(gint8 j = 0; j < dashPattern[i]; j+=2)
        {
            dash.push_back(value);
        }
        value = !value;
    }
}

bool ActionNameEditor::ActionPropertiesDialog::run(ActionName& action,
                                                   bool nameIsEditable)
{
    actionBeingEdited = &action;

    // Action name text entry:
    // ----------------------
    GtkWidget* nameTextEntry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(nameTextEntry), action.name.getText());
    if(nameIsEditable)
        gtk_editable_select_region(GTK_EDITABLE(nameTextEntry), 0, -1);
    else
        gtk_widget_set_sensitive(nameTextEntry, FALSE);
    gtk_entry_set_activates_default(GTK_ENTRY(nameTextEntry), TRUE);

    GtkWidget* nameBox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(nameBox), gtk_label_new("Name"),FALSE,FALSE,4);
    gtk_box_pack_start(GTK_BOX(nameBox), nameTextEntry, TRUE, TRUE, 0);

    // Line sample and color selector:
    // ------------------------------
    sampleDrawingArea = gtk_drawing_area_new();
    gtk_widget_set_size_request(sampleDrawingArea, 16, 16);
    g_signal_connect(GTK_OBJECT(sampleDrawingArea), "expose-event",
                     G_CALLBACK(sampleExposeEvent), this);

    GtkWidget* sampleFrame = gtk_frame_new("Sample");
    gtk_container_add(GTK_CONTAINER(sampleFrame), sampleDrawingArea);

    colorButtonDrawingArea = gtk_drawing_area_new();
    gtk_widget_set_size_request(colorButtonDrawingArea, 16, 16);
    g_signal_connect(GTK_OBJECT(colorButtonDrawingArea), "expose-event",
                     G_CALLBACK(colorButtonExposeEvent), this);

    colorButtonGC =
        gdk_gc_new(gtk_widget_get_root_window(GTK_WIDGET(owner.parent)));

    GtkWidget* colorButton = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(colorButton), colorButtonDrawingArea);
    g_signal_connect(GTK_OBJECT(colorButton), "clicked",
                     G_CALLBACK(colorButtonClickedEvent), this);

    GtkWidget* sampleBox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sampleBox), sampleFrame, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(sampleBox), colorButton, FALSE, FALSE, 2);

    // Dash pattern editor:
    // -------------------
    GtkWidget* decreaseButton = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(decreaseButton),
                      gtk_arrow_new(GTK_ARROW_LEFT, GTK_SHADOW_IN));
    g_signal_connect(GTK_OBJECT(decreaseButton), "clicked",
                     G_CALLBACK(decreaseButtonClickedEvent), this);

    GtkWidget* increaseButton = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(increaseButton),
                      gtk_arrow_new(GTK_ARROW_RIGHT, GTK_SHADOW_IN));
    g_signal_connect(GTK_OBJECT(increaseButton), "clicked",
                     G_CALLBACK(increaseButtonClickedEvent), this);

    calculateDash();
    dashPatternDrawingArea = gtk_drawing_area_new();
    gtk_widget_set_size_request(dashPatternDrawingArea, dash.size()*16+1, 17);
    gtk_widget_set_events(dashPatternDrawingArea,
                          GDK_EXPOSURE_MASK |
                          GDK_BUTTON_PRESS_MASK);
    g_signal_connect(GTK_OBJECT(dashPatternDrawingArea), "expose-event",
                     G_CALLBACK(dashPatternExposeEvent), this);
    g_signal_connect(GTK_OBJECT(dashPatternDrawingArea), "button-press-event",
                     G_CALLBACK(dashPatternClickEvent), this);

    GtkWidget* dashPatternBox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(dashPatternBox), decreaseButton,
                       FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(dashPatternBox), increaseButton,
                       FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(dashPatternBox), dashPatternDrawingArea,
                       FALSE, FALSE, 2);

    GtkWidget* dashPatternFrame = gtk_frame_new("Dash pattern");
    gtk_container_add(GTK_CONTAINER(dashPatternFrame), dashPatternBox);


    // Dialog window:
    // -------------
    GtkWidget* dialog =
        gtk_dialog_new_with_buttons("Action properties editor", owner.parent,
                                    GtkDialogFlags
                                    (GTK_DIALOG_MODAL |
                                     GTK_DIALOG_DESTROY_WITH_PARENT),
                                    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                    GTK_STOCK_OK, GTK_RESPONSE_OK,
                                    NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);

    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), nameBox);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), sampleBox,
                       TRUE, TRUE, 4);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), dashPatternFrame,
                       TRUE, TRUE, 4);

    // Run:
    // ---
    gtk_widget_show_all(dialog);
    bool retval = (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK);
    if(retval)
    {
        action.name.setText(gtk_entry_get_text(GTK_ENTRY(nameTextEntry)));
    }
    gtk_widget_destroy(dialog);
    return retval;
}

void ActionNameEditor::ActionPropertiesDialog::drawSample()
{
    gdk_draw_rectangle(sampleDrawingArea->window,
                       sampleDrawingArea->style->white_gc,
                       TRUE, 0, 0,
                       sampleDrawingArea->allocation.width,
                       sampleDrawingArea->allocation.height);

    owner.setColorAndDash(*actionBeingEdited);
    gdk_draw_line(sampleDrawingArea->window, owner.lineGC,
                  2, sampleDrawingArea->allocation.height/2,
                  sampleDrawingArea->allocation.width-2,
                  sampleDrawingArea->allocation.height/2);
}

void ActionNameEditor::ActionPropertiesDialog::drawColorButton()
{
    gdk_colormap_alloc_color(owner.colorMap, &actionBeingEdited->color,
                             FALSE, TRUE);
    gdk_gc_set_foreground(colorButtonGC, &actionBeingEdited->color);
    gdk_draw_rectangle(colorButtonDrawingArea->window, colorButtonGC,
                       TRUE, 0, 0,
                       colorButtonDrawingArea->allocation.width,
                       colorButtonDrawingArea->allocation.height);
}

void ActionNameEditor::ActionPropertiesDialog::drawDashPattern()
{
    const int y = dashPatternDrawingArea->allocation.height/2-8;
    gdk_draw_rectangle(dashPatternDrawingArea->window,
                       dashPatternDrawingArea->style->black_gc,
                       TRUE, 0, y-1, dash.size()*16+1, 17);

    GdkGC* whiteGC = dashPatternDrawingArea->style->white_gc;
    for(unsigned i = 0; i < dash.size(); ++i)
    {
        if(!dash[i])
            gdk_draw_rectangle(dashPatternDrawingArea->window, whiteGC,
                               TRUE, i*16+1, y, 15, 15);
    }
}

gboolean ActionNameEditor::ActionPropertiesDialog::sampleExposeEvent
(GtkWidget*, GdkEventExpose*, ActionPropertiesDialog* instance)
{
    instance->drawSample();
    return TRUE;
}

gboolean ActionNameEditor::ActionPropertiesDialog::colorButtonExposeEvent
(GtkWidget*, GdkEventExpose*, ActionPropertiesDialog* instance)
{
    instance->drawColorButton();
    return TRUE;
}

void ActionNameEditor::ActionPropertiesDialog::colorButtonClickedEvent
(GtkButton*, ActionPropertiesDialog* instance)
{
    GtkWidget* dialog = gtk_color_selection_dialog_new("Select action color");
    gtk_window_set_transient_for(GTK_WINDOW(dialog), instance->owner.parent);
    gtk_color_selection_set_current_color
        (GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog)->colorsel),
         &instance->actionBeingEdited->color);

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
    {
        gtk_color_selection_get_current_color
            (GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog)->colorsel),
             &instance->actionBeingEdited->color);
        instance->drawSample();
        instance->drawColorButton();
    }
    gtk_widget_destroy(dialog);
}

void ActionNameEditor::ActionPropertiesDialog::decreaseButtonClickedEvent
(GtkButton*, ActionPropertiesDialog* instance)
{
    std::vector<bool>& dash = instance->dash;
    if(dash.size() > 1)
    {
        std::vector<gint8>& dashPattern =
            instance->actionBeingEdited->dashPattern;

        if(dash.size() == 2)
        {
            dash.pop_back();
            dashPattern.clear();
        }
        else if(dashPattern.back() > 2)
        {
            dashPattern.back() -= 2;
            dash.pop_back();
        }
        else
        {
            if(dashPattern[dashPattern.size()-2] > 2)
            {
                dashPattern[dashPattern.size()-2] -= 2;
            }
            else
            {
                dashPattern.pop_back();
                dashPattern.pop_back();
                dashPattern.back() += 2;
            }
            dash.pop_back();
            dash.back() = false;
        }

        gtk_widget_set_size_request(instance->dashPatternDrawingArea,
                                    instance->dash.size()*16+1, 17);
        instance->drawDashPattern();
        instance->drawSample();
    }
}

void ActionNameEditor::ActionPropertiesDialog::increaseButtonClickedEvent
(GtkButton*, ActionPropertiesDialog* instance)
{
    std::vector<gint8>& dashPattern = instance->actionBeingEdited->dashPattern;
    std::vector<bool>& dash = instance->dash;

    if(dashPattern.empty())
    {
        dashPattern.push_back(2);
        dashPattern.push_back(2);
    }
    else
        dashPattern.back() += 2;

    dash.push_back(false);
    gtk_widget_set_size_request(instance->dashPatternDrawingArea,
                                dash.size()*16+1, 17);
    instance->drawDashPattern();
    instance->drawSample();
}

gboolean ActionNameEditor::ActionPropertiesDialog::dashPatternExposeEvent
(GtkWidget*, GdkEventExpose*, ActionPropertiesDialog* instance)
{
    instance->drawDashPattern();
    return TRUE;
}

gboolean ActionNameEditor::ActionPropertiesDialog::dashPatternClickEvent
(GtkWidget*, GdkEventButton* event, ActionPropertiesDialog* instance)
{
    const unsigned index = unsigned(event->x)/16;
    std::vector<gint8>& dashPattern = instance->actionBeingEdited->dashPattern;
    std::vector<bool>& dash = instance->dash;

    if(index == 0 || index == dash.size()-1) return TRUE;

    dash[index] = !dash[index];

    dashPattern.clear();
    gint8 value = 2;
    for(unsigned i = 1; i < dash.size(); ++i)
    {
        if(dash[i] != dash[i-1])
        {
            dashPattern.push_back(value);
            value = 2;
        }
        else
            value += 2;
    }
    dashPattern.push_back(value);

    instance->drawDashPattern();
    instance->drawSample();
    return TRUE;
}




//===========================================================================
// ActionNameEditor
//===========================================================================

// ActionNameEditor popup menu data
// --------------------------------
const GtkItemFactoryEntry
ActionNameEditor::editableActionNameEditorPopupMenuItems[] =
{
    { "/Action properties...", NULL,
      GtkItemFactoryCallback(editActionCallback), 1,
      NULL, NULL },
    { "/Remove action", NULL, GtkItemFactoryCallback(removeActionCallback), 0,
      NULL, NULL },
    { "/sep1", NULL, NULL, 0, "<Separator>", NULL },
    { "/New action...", NULL, GtkItemFactoryCallback(newActionCallback), 0,
      NULL, NULL }
};

const GtkItemFactoryEntry
ActionNameEditor::noneditableActionNameEditorPopupMenuItems[] =
{
    { "/Action properties...", NULL,
      GtkItemFactoryCallback(editActionCallback), 0,
      NULL, NULL }
};

const GtkItemFactoryEntry
ActionNameEditor::editableStatePropNameEditorPopupMenuItems[] =
{
    { "/Rename...", NULL, GtkItemFactoryCallback(renameStatePropCallback), 0,
      NULL, NULL },
    { "/Change color...", NULL,
      GtkItemFactoryCallback(changeStatePropColorCallback), 0,
      NULL, NULL },
    { "/Remove", NULL, GtkItemFactoryCallback(removeStatePropCallback), 0,
      NULL, NULL },
    { "/sep1", NULL, NULL, 0, "<Separator>", NULL },
    { "/New state proposition...", NULL,
      GtkItemFactoryCallback(newStatePropCallback), 0,
      NULL, NULL }
};

const GtkItemFactoryEntry
ActionNameEditor::noneditableStatePropNameEditorPopupMenuItems[] =
{
    { "/Change color...", NULL,
      GtkItemFactoryCallback(changeStatePropColorCallback), 0,
      NULL, NULL }
};

// Constructor and destructor
// --------------------------
ActionNameEditor::ActionNameEditor():
    selectedActionName(~0U), selectedStatePropName(~0U),
    parent(0), statusBar(0), panedWindow(0),
    actionsPixmap(0), actionsDrawingArea(0),
    statePropsPixmap(0), statePropsDrawingArea(0),
    lineGC(0), colorMap(0),
    actionsPixmapWidth(0), actionsPixmapHeight(0),
    statePropsPixmapWidth(0), statePropsPixmapHeight(0),
    dataHasBeenModified(false),
    callbacks(0)
{
}

ActionNameEditor::~ActionNameEditor()
{
    if(actionsPixmap) g_object_unref(actionsPixmap);
    if(statePropsPixmap) g_object_unref(statePropsPixmap);
    if(lineGC) g_object_unref(lineGC);
    if(circleGC) g_object_unref(circleGC);
    if(selectionGC) g_object_unref(selectionGC);
}


// Initialization
// --------------
GtkWidget* ActionNameEditor::initialize(GtkWindow* parentWindow,
                                        StatusBar* parentWindowStatusBar,
                                        bool editable)
{
    if(parent)
    {
        std::cerr << "Fatal error: ActionNameEditor::initialize() called "
                  << "more than once.\n";
        std::abort();
    }

    parent = parentWindow;
    statusBar = parentWindowStatusBar;

    lineGC = gdk_gc_new(gtk_widget_get_root_window(GTK_WIDGET(parent)));
    circleGC = gdk_gc_new(gtk_widget_get_root_window(GTK_WIDGET(parent)));
    selectionGC = gdk_gc_new(gtk_widget_get_root_window(GTK_WIDGET(parent)));
    colorMap = gdk_colormap_get_system();

    GdkColor selColor = { 0, ACTION_NAME_EDITOR_SELECTION_COLOR };
    gdk_colormap_alloc_color(colorMap, &selColor, FALSE, TRUE);
    gdk_gc_set_foreground(selectionGC, &selColor);

    // Create action names window:
    actionsDrawingArea = gtk_drawing_area_new();
    GtkWidget* actionNamesWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(actionNamesWindow),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_add_with_viewport
        (GTK_SCROLLED_WINDOW(actionNamesWindow), actionsDrawingArea);

    gtk_widget_set_events(actionsDrawingArea,
                          GDK_EXPOSURE_MASK |
                          GDK_BUTTON_PRESS_MASK |
                          GDK_ENTER_NOTIFY_MASK |
                          GDK_LEAVE_NOTIFY_MASK);

    g_signal_connect(GTK_OBJECT(actionsDrawingArea), "configure-event",
                     G_CALLBACK(actionsConfigureEvent), this);
    g_signal_connect(GTK_OBJECT(actionsDrawingArea), "expose-event",
                     G_CALLBACK(actionsExposeEvent), this);
    g_signal_connect(GTK_OBJECT(actionsDrawingArea), "button-press-event",
                     G_CALLBACK(actionsMouseButtonPressEvent), this);
    g_signal_connect(GTK_OBJECT(actionsDrawingArea), "enter-notify-event",
                     G_CALLBACK(enterNotifyEvent), this);
    g_signal_connect(GTK_OBJECT(actionsDrawingArea), "leave-notify-event",
                     G_CALLBACK(leaveNotifyEvent), this);


    GtkWidget* editorWindow;
    if(editable)
    {
        PopupMenu::MenuItems
            items(TableRange(editableActionNameEditorPopupMenuItems));
        actionsPopupMenu.createMenu("<ActionNamesMenu>", items, this);

        // Create stateprop names window:
        statePropsDrawingArea = gtk_drawing_area_new();
        GtkWidget* statePropNamesWindow = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_policy
            (GTK_SCROLLED_WINDOW(statePropNamesWindow),
             GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        gtk_scrolled_window_add_with_viewport
            (GTK_SCROLLED_WINDOW(statePropNamesWindow),
             statePropsDrawingArea);

        gtk_widget_set_events(statePropsDrawingArea,
                              GDK_EXPOSURE_MASK |
                              GDK_BUTTON_PRESS_MASK |
                              GDK_ENTER_NOTIFY_MASK |
                              GDK_LEAVE_NOTIFY_MASK);
        g_signal_connect(GTK_OBJECT(statePropsDrawingArea), "configure-event",
                         G_CALLBACK(statePropsConfigureEvent), this);
        g_signal_connect(GTK_OBJECT(statePropsDrawingArea), "expose-event",
                         G_CALLBACK(statePropsExposeEvent), this);
        g_signal_connect(GTK_OBJECT(statePropsDrawingArea),
                         "button-press-event",
                         G_CALLBACK(statePropsMouseButtonPressEvent), this);
        g_signal_connect(GTK_OBJECT(statePropsDrawingArea),
                         "enter-notify-event",
                         G_CALLBACK(enterNotifyEvent), this);
        g_signal_connect(GTK_OBJECT(statePropsDrawingArea),
                         "leave-notify-event",
                         G_CALLBACK(leaveNotifyEvent), this);

        PopupMenu::MenuItems
            items2(TableRange(editableStatePropNameEditorPopupMenuItems));
        statePropsPopupMenu.createMenu("<StatePropNamesMenu>", items2, this);

        editorWindow = panedWindow = gtk_vpaned_new();
        gtk_paned_add1(GTK_PANED(editorWindow), actionNamesWindow);
        gtk_paned_add2(GTK_PANED(editorWindow), statePropNamesWindow);
    }
    else
    {
        PopupMenu::MenuItems
            items(TableRange(noneditableActionNameEditorPopupMenuItems));
        actionsPopupMenu.createMenu("<ActionNamesMenu>", items, this);

        PopupMenu::MenuItems
            items2(TableRange(noneditableStatePropNameEditorPopupMenuItems));
        statePropsPopupMenu.createMenu("<StatePropNamesMenu>", items2, this);

        editorWindow = actionNamesWindow;
    }

    return editorWindow;
}

void ActionNameEditor::refreshAll()
{
    recalculateActionsPixmapSize();
    recalculateStatePropsPixmapSize();
    //redrawActionsPixmap();
    //redrawStatePropsPixmap();
    refreshActionsWindow();
    refreshStatePropsWindow();
}

void ActionNameEditor::deleteAll()
{
    actionNames.clear();
    ActionName tau(parent, "tau");
    tau.color.red = tau.color.green = tau.color.blue =
        ACTION_NAME_EDITOR_DEFAULT_TAU_GRAY_SHADE;
    actionNames.push_back(tau);
    statePropNames.clear();
    selectedActionName = ~0U;
    selectedStatePropName = ~0U;

    recalculateActionsPixmapSize();
    redrawActionsPixmap();
    refreshActionsWindow();
    recalculateStatePropsPixmapSize();
    redrawStatePropsPixmap();
    refreshStatePropsWindow();

    dataHasBeenModified = true;
}

void ActionNameEditor::registerAttributeChangeCallBack
(AttributeChangeCallBack* cb)
{
    callbacks = cb;
}

void ActionNameEditor::updateActionsPopupMenuSensitivity()
{
    bool actionActive = (selectedActionName < actionNames.size());

    actionsPopupMenu.setSensitivity("/Action properties...", actionActive);
    actionsPopupMenu.setSensitivity("/Remove action",
                                    selectedActionName > 0 && actionActive);
}

void ActionNameEditor::updateStatePropsPopupMenuSensitivity()
{
    bool statePropActive = (selectedStatePropName < statePropNames.size());
    statePropsPopupMenu.setSensitivity("/Rename...", statePropActive);
    statePropsPopupMenu.setSensitivity("/Change color...", statePropActive);
    statePropsPopupMenu.setSensitivity("/Remove", statePropActive);
}

void ActionNameEditor::setGutterPosition(int percentage)
{
    if(panedWindow)
        gtk_paned_set_position(GTK_PANED(panedWindow),
                               panedWindow->allocation.height*percentage/100);
}

int ActionNameEditor::getGutterPosition() const
{
    if(panedWindow)
        return gtk_paned_get_position(GTK_PANED(panedWindow))*100/
            panedWindow->allocation.height;
    return 0;
}

void ActionNameEditor::selectActionName(unsigned actionNumber)
{
    selectedActionName = actionNumber;
    redrawActionsPixmap();
    refreshActionsWindow();
    if(callbacks)
        callbacks->actionNameSelect(actionNumber);
}

void ActionNameEditor::selectStatePropName(unsigned spNumber)
{
    selectedStatePropName = spNumber;
    redrawStatePropsPixmap();
    refreshStatePropsWindow();
    if(callbacks)
        callbacks->statePropNameSelect(spNumber);
}

void ActionNameEditor::addStatePropName(const std::string& name,
                                        bool refreshWindow)
{
    StatePropName newStateProp(parent, name.c_str());
    tools::generateRandomColor(newStateProp.color,
                               32768/2, 0xFFFF, 0, 0xFFFF);
    statePropNames.push_back(newStateProp);
    selectedStatePropName = statePropNames.size()-1;
    if(refreshWindow)
    {
        recalculateStatePropsPixmapSize();
        refreshStatePropsWindow();
    }
    if(callbacks)
        callbacks->statePropNameSelect(selectedStatePropName);
    dataHasBeenModified = true;
}


// Action properties editing
// -------------------------
bool ActionNameEditor::editAction(ActionName& action,
                                  bool nameIsEditable) const
{
    ActionPropertiesDialog dialog(*this);
    bool res = dialog.run(action, nameIsEditable);
    if(res) dataHasBeenModified = true;
    return res;
}


// Window redrawing functions
// --------------------------
void ActionNameEditor::setColorAndDash(ActionName& data) const
{
    gdk_colormap_alloc_color(colorMap, &data.color, FALSE, TRUE);
    gdk_gc_set_foreground(lineGC, &data.color);
    if(data.dashPattern.empty())
    {
        gdk_gc_set_line_attributes(lineGC, 2, GDK_LINE_SOLID,
                                   GDK_CAP_NOT_LAST, GDK_JOIN_MITER);
    }
    else
    {
        gdk_gc_set_line_attributes(lineGC, 2, GDK_LINE_ON_OFF_DASH,
                                   GDK_CAP_NOT_LAST, GDK_JOIN_MITER);
        gdk_gc_set_dashes(lineGC, 0,
                          &data.dashPattern[0], data.dashPattern.size());
    }
}


void ActionNameEditor::setColor(StatePropName& data) const
{
    gdk_colormap_alloc_color(colorMap, &data.color, FALSE, TRUE);
    gdk_gc_set_foreground(circleGC, &data.color);
}

void ActionNameEditor::redrawActionsPixmap()
{
    gdk_draw_rectangle(actionsPixmap, actionsDrawingArea->style->white_gc,
                       TRUE, 0, 0, actionsPixmapWidth, actionsPixmapHeight);

    if(selectedActionName < actionNames.size())
        gdk_draw_rectangle
            (actionsPixmap, selectionGC,
             TRUE,
             0, selectedActionName*ACTION_NAME_EDITOR_NAMES_SPACING,
             actionsPixmapWidth, ACTION_NAME_EDITOR_NAMES_SPACING);

    for(unsigned i = 0; i < actionNames.size(); ++i)
    {
        ActionName& actionName = actionNames[i];
        actionName.name.draw(actionsPixmap,
                             actionsDrawingArea->style->black_gc,
                             1, i*ACTION_NAME_EDITOR_NAMES_SPACING+1);
        const int lineY =
            i*ACTION_NAME_EDITOR_NAMES_SPACING +
            actionName.name.getTextHeight() + 2;
        setColorAndDash(actionName);
        gdk_draw_line(actionsPixmap, lineGC,
                      1, lineY, actionsPixmapWidth, lineY);
    }
}

void ActionNameEditor::redrawStatePropsPixmap()
{
    if(!statePropsPixmap) return;

    gdk_draw_rectangle(statePropsPixmap,
                       statePropsDrawingArea->style->white_gc,
                       TRUE, 0, 0,
                       statePropsPixmapWidth, statePropsPixmapHeight);

    if(selectedStatePropName < statePropNames.size())
        gdk_draw_rectangle
            (statePropsPixmap, selectionGC,
             TRUE,
             0, selectedStatePropName*ACTION_NAME_EDITOR_NAMES_SPACING,
             statePropsPixmapWidth, ACTION_NAME_EDITOR_NAMES_SPACING);

    for(unsigned i = 0; i < statePropNames.size(); ++i)
    {
        StatePropName& statePropName = statePropNames[i];
        statePropName.name.draw(statePropsPixmap,
                                statePropsDrawingArea->style->black_gc,
                                ACTION_NAME_EDITOR_NAMES_SPACING+2,
                                i*ACTION_NAME_EDITOR_NAMES_SPACING+
                                ACTION_NAME_EDITOR_NAMES_SPACING/2-
                                statePropName.name.getTextHeight()/2);
        setColor(statePropName);
        gdk_draw_arc(statePropsPixmap, circleGC, TRUE,
                     0, i*ACTION_NAME_EDITOR_NAMES_SPACING,
                     ACTION_NAME_EDITOR_NAMES_SPACING,
                     ACTION_NAME_EDITOR_NAMES_SPACING,
                     0, 64*360);
    }
}

namespace
{
    template<typename Data>
    int getMaxWidth(const std::vector<Data>& data, int minWidth)
    {
        for(unsigned i = 0; i < data.size(); ++i)
        {
            if(data[i].name.getTextWidth()+2 > minWidth)
                minWidth = data[i].name.getTextWidth()+2;
        }
        return minWidth;
    }

    static const gint8 generatedDashPatterns[][5] =
    {
        { 4, 2, 0 }, { 6, 2, 2, 2, 0 }, { 0 }, { 2, 2, 0 }, { 0 }
    };

    void generateDashPattern(std::vector<gint8>& dashPattern)
    {
        static unsigned patternIndex = 0;
        dashPattern.clear();

        for(unsigned i = 0; generatedDashPatterns[patternIndex][i]; ++i)
            dashPattern.push_back(generatedDashPatterns[patternIndex][i]);

        if(++patternIndex ==
           sizeof(generatedDashPatterns)/sizeof(generatedDashPatterns[0]))
            patternIndex = 0;
    }
}

void ActionNameEditor::recalculateActionsPixmapSize()
{
    const int newWidth = getMaxWidth(actionNames,
                                     actionsDrawingArea->allocation.width);
    const int newHeight = actionNames.size()*ACTION_NAME_EDITOR_NAMES_SPACING;

    if(newWidth != actionsPixmapWidth || newHeight != actionsPixmapHeight)
    {
        if(actionsPixmap)
            g_object_unref(actionsPixmap);

        actionsPixmap = gdk_pixmap_new(actionsDrawingArea->window,
                                       newWidth, newHeight, -1);
        gtk_widget_set_size_request(actionsDrawingArea, newWidth, newHeight);
        actionsPixmapWidth = newWidth;
        actionsPixmapHeight = newHeight;
    }
    redrawActionsPixmap();
}

void ActionNameEditor::recalculateStatePropsPixmapSize()
{
    if(statePropNames.empty())
    {
        if(statePropsPixmap)
            g_object_unref(statePropsPixmap);
        statePropsPixmap = 0;
        statePropsPixmapWidth = statePropsPixmapHeight = 0;
        return;
    }

    const int newWidth = getMaxWidth(statePropNames,
                                     statePropsDrawingArea->allocation.width);
    const int newHeight =
        statePropNames.size()*ACTION_NAME_EDITOR_NAMES_SPACING;

    if(newWidth != statePropsPixmapWidth ||
       newHeight != statePropsPixmapHeight)
    {
        if(statePropsPixmap)
            g_object_unref(statePropsPixmap);

        statePropsPixmap = gdk_pixmap_new(statePropsDrawingArea->window,
                                          newWidth, newHeight, -1);
        gtk_widget_set_size_request(statePropsDrawingArea,newWidth,newHeight);
        statePropsPixmapWidth = newWidth;
        statePropsPixmapHeight = newHeight;
    }
    redrawStatePropsPixmap();
}

void ActionNameEditor::refreshActionsWindow(int x, int y,
                                            int width, int height)
{
    if(width == 0 && height == 0)
    {
        width = actionsPixmapWidth;
        height = actionsPixmapHeight;
    }
    gdk_draw_pixmap(actionsDrawingArea->window,
                    actionsDrawingArea->style->
                    fg_gc[GTK_WIDGET_STATE(actionsDrawingArea)],
                    actionsPixmap, x, y, x, y, width, height);
}

void ActionNameEditor::refreshStatePropsWindow(int x, int y,
                                               int width, int height)
{
    if(!statePropsPixmap)
    {
        gdk_window_clear(statePropsDrawingArea->window);
        return;
    }

    if(width == 0 && height == 0)
    {
        width = statePropsPixmapWidth;
        height = statePropsPixmapHeight;
    }
    gdk_draw_pixmap(statePropsDrawingArea->window,
                    statePropsDrawingArea->style->
                    fg_gc[GTK_WIDGET_STATE(statePropsDrawingArea)],
                    statePropsPixmap, x, y, x, y, width, height);
}


// Event handlers
// --------------
gboolean ActionNameEditor::actionsConfigureEvent(GtkWidget*,
                                                 GdkEventConfigure*,
                                                 ActionNameEditor* instance)
{
    if(instance->actionNames.empty())
    {
        ActionName tau(instance->parent, "tau");
        tau.color.red = tau.color.green = tau.color.blue =
            ACTION_NAME_EDITOR_DEFAULT_TAU_GRAY_SHADE;
        instance->actionNames.push_back(tau);
    }

    instance->recalculateActionsPixmapSize();
    return TRUE;
}

gboolean ActionNameEditor::actionsExposeEvent(GtkWidget*,
                                              GdkEventExpose* event,
                                              ActionNameEditor* instance)
{
    instance->refreshActionsWindow(event->area.x, event->area.y,
                                   event->area.width, event->area.height);
    return TRUE;
}

gboolean ActionNameEditor::statePropsConfigureEvent(GtkWidget*,
                                                    GdkEventConfigure*,
                                                    ActionNameEditor* instance)
{
    instance->recalculateStatePropsPixmapSize();
    return TRUE;
}

gboolean ActionNameEditor::statePropsExposeEvent(GtkWidget*,
                                                 GdkEventExpose* event,
                                                 ActionNameEditor* instance)
{
    instance->refreshStatePropsWindow(event->area.x, event->area.y,
                                      event->area.width, event->area.height);
    return TRUE;
}


gboolean ActionNameEditor::actionsMouseButtonPressEvent
(GtkWidget*,
 GdkEventButton* press,
 ActionNameEditor* instance)
{
    unsigned selection = unsigned(press->y)/ACTION_NAME_EDITOR_NAMES_SPACING;
    if(selection >= instance->actionNames.size())
        selection = ~0U;

    if(selection != instance->selectedActionName)
    {
        instance->selectedActionName = selection;
        instance->redrawActionsPixmap();
        instance->refreshActionsWindow();
        if(instance->callbacks)
            instance->callbacks->actionNameSelect(selection);
    }

    if(press->button == 3)
    {
        instance->updateActionsPopupMenuSensitivity();
        instance->actionsPopupMenu.popup(int(press->x_root),
                                         int(press->y_root),
                                         press->button, press->time);
    }

    return TRUE;
}

gboolean ActionNameEditor::statePropsMouseButtonPressEvent
(GtkWidget*,
 GdkEventButton* press,
 ActionNameEditor* instance)
{
    unsigned selection = unsigned(press->y)/ACTION_NAME_EDITOR_NAMES_SPACING;
    if(selection >= instance->statePropNames.size())
        selection = ~0U;

    if(selection != instance->selectedStatePropName)
    {
        instance->selectedStatePropName = selection;
        instance->redrawStatePropsPixmap();
        instance->refreshStatePropsWindow();
        if(instance->callbacks)
            instance->callbacks->statePropNameSelect(selection);
    }

    if(press->button == 3)
    {
        instance->updateStatePropsPopupMenuSensitivity();
        instance->statePropsPopupMenu.popup(int(press->x_root),
                                            int(press->y_root),
                                            press->button, press->time);
    }

    return TRUE;
}

gboolean ActionNameEditor::enterNotifyEvent(GtkWidget*, GdkEventCrossing*,
                                            ActionNameEditor* instance)
{
    instance->statusBar->setText(ACTION_NAME_EDITOR_HOVER_TEXT);
    return TRUE;
}

gboolean ActionNameEditor::leaveNotifyEvent(GtkWidget*, GdkEventCrossing*,
                                            ActionNameEditor*instance)
{
    instance->statusBar->clear();
    return TRUE;
}


// Popup menu callbacks
// --------------------
void ActionNameEditor::newActionCallback(gpointer ptr, guint, GtkWidget*)
{
    ActionNameEditor* instance = reinterpret_cast<ActionNameEditor*>(ptr);

    ActionName newAction(instance->parent, "New action");
    tools::generateRandomColor(newAction.color, 0, 0xFFFF, 0, 0x8000);
    generateDashPattern(newAction.dashPattern);
    if(instance->editAction(newAction, true))
    {
        instance->actionNames.push_back(newAction);
        instance->selectedActionName = instance->actionNames.size()-1;
        instance->recalculateActionsPixmapSize();
        instance->refreshActionsWindow();
        if(instance->callbacks)
            instance->callbacks->actionNameSelect
                (instance->selectedActionName);
    }
}

void ActionNameEditor::editActionCallback(gpointer ptr, guint editable,
                                          GtkWidget*)
{
    ActionNameEditor* instance = reinterpret_cast<ActionNameEditor*>(ptr);

    if(instance->editAction
       (instance->actionNames[instance->selectedActionName],
        editable && instance->selectedActionName > 0))
    {
        instance->recalculateActionsPixmapSize();
        instance->refreshActionsWindow();
        if(instance->callbacks)
            instance->callbacks->actionNameAttributesChanged
                (instance->selectedActionName);
    }
}

void ActionNameEditor::removeActionCallback(gpointer ptr, guint, GtkWidget*)
{
    ActionNameEditor* instance = reinterpret_cast<ActionNameEditor*>(ptr);

    GtkWidget* dialog = gtk_message_dialog_new
        (instance->parent, GTK_DIALOG_DESTROY_WITH_PARENT,
         GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
         "Are you sure you want to remove the\naction \"%s\"?",
         instance->actionNames[instance->selectedActionName].name.getText());
    gtk_window_set_title(GTK_WINDOW(dialog), "Confirm removal");
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if(result == GTK_RESPONSE_YES)
    {
        instance->actionNames.erase(instance->actionNames.begin()+
                                    instance->selectedActionName);
        unsigned removedActionName = instance->selectedActionName;
        instance->selectedActionName = ~0U;
        instance->recalculateActionsPixmapSize();
        instance->refreshActionsWindow();
        if(instance->callbacks)
        {
            instance->callbacks->actionNameSelect(~0U);
            instance->callbacks->actionNameRemoved(removedActionName);
        }
        instance->dataHasBeenModified = true;
    }
}


void ActionNameEditor::renameStatePropCallback(gpointer ptr, guint,
                                               GtkWidget*)
{
    ActionNameEditor* instance = reinterpret_cast<ActionNameEditor*>(ptr);

    if(instance->statePropNames[instance->selectedStatePropName].name.askString
       ("State proposition name"))
    {
        instance->recalculateStatePropsPixmapSize();
        instance->refreshStatePropsWindow();
        if(instance->callbacks)
            instance->callbacks->statePropNameChanged
                (instance->selectedStatePropName);
        instance->dataHasBeenModified = true;
    }
}

void ActionNameEditor::changeStatePropColorCallback(gpointer ptr, guint,
                                                    GtkWidget*)
{
    ActionNameEditor* instance = reinterpret_cast<ActionNameEditor*>(ptr);

    GtkWidget* dialog =
        gtk_color_selection_dialog_new("Select state proposition color");
    gtk_window_set_transient_for(GTK_WINDOW(dialog), instance->parent);
    gtk_color_selection_set_current_color
        (GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog)->colorsel),
         &instance->statePropNames[instance->selectedStatePropName].color);

    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
    {
        gtk_color_selection_get_current_color
            (GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(dialog)->colorsel),
             &instance->statePropNames[instance->selectedStatePropName].color);
        instance->redrawStatePropsPixmap();
        instance->refreshStatePropsWindow();
        if(instance->callbacks)
            instance->callbacks->statePropNameAttributesChanged
                (instance->selectedStatePropName);
        instance->dataHasBeenModified = true;
    }
    gtk_widget_destroy(dialog);
}

void ActionNameEditor::removeStatePropCallback(gpointer ptr, guint, GtkWidget*)
{
    ActionNameEditor* instance = reinterpret_cast<ActionNameEditor*>(ptr);

    GtkWidget* dialog = gtk_message_dialog_new
        (instance->parent, GTK_DIALOG_DESTROY_WITH_PARENT,
         GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
         "Are you sure you want to remove the state\nproposition \"%s\"?",
         instance->statePropNames[instance->selectedStatePropName].
         name.getText());
    gtk_window_set_title(GTK_WINDOW(dialog), "Confirm removal");
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if(result == GTK_RESPONSE_YES)
    {
        instance->statePropNames.erase(instance->statePropNames.begin()+
                                       instance->selectedStatePropName);
        unsigned removedStatePropName = instance->selectedStatePropName;
        instance->selectedStatePropName = ~0U;
        instance->recalculateStatePropsPixmapSize();
        instance->refreshStatePropsWindow();
        if(instance->callbacks)
        {
            instance->callbacks->statePropNameSelect(~0U);
            instance->callbacks->statePropNameRemoved(removedStatePropName);
        }
        instance->dataHasBeenModified = true;
    }
}

void ActionNameEditor::newStatePropCallback(gpointer ptr, guint, GtkWidget*)
{
    ActionNameEditor* instance = reinterpret_cast<ActionNameEditor*>(ptr);

    StatePropName newStateProp(instance->parent, "New stateprop");
    if(newStateProp.name.askString("State proposition name"))
    {
        tools::generateRandomColor(newStateProp.color,
                                   32768/2, 0xFFFF, 0, 0xFFFF);
        instance->statePropNames.push_back(newStateProp);
        instance->selectedStatePropName = instance->statePropNames.size()-1;
        instance->recalculateStatePropsPixmapSize();
        instance->refreshStatePropsWindow();
        if(instance->callbacks)
            instance->callbacks->statePropNameSelect
                (instance->selectedStatePropName);
        instance->dataHasBeenModified = true;
    }
}


// Action names reading and writing:
// --------------------------------
#include "LSTS_File/Header.hh"
void ActionNameEditor::lsts_StartActionNames(Header& h)
{
    actionNames.resize(h.GiveActionCnt()+1, ActionName(parent, ""));
}

void ActionNameEditor::lsts_ActionName(lsts_index_t action_number,
                                       const std::string& action_name)
{
    actionNames[action_number].name.setText(action_name.c_str());
    tools::generateRandomColor(actionNames[action_number].color,
                               0, 0xFFFF, 0, 0x8000);
}

void ActionNameEditor::lsts_EndActionNames()
{
    for(unsigned i = 1; i < actionNames.size(); ++i)
        generateDashPattern(actionNames[i].dashPattern);
}

lsts_index_t ActionNameEditor::lsts_numberOfActionNamesToWrite()
{
    return actionNames.size()-1;
}

void ActionNameEditor::lsts_WriteActionNames(iActionNamesAP& writer)
{
    for(unsigned i = 1; i < actionNames.size(); ++i)
    {
        writer.lsts_ActionName(i, actionNames[i].name.getText());
    }
}


// Style file reading and writing:
// ------------------------------
namespace
{
    unsigned hexCharToInt(char c)
    {
        const unsigned val = c - '0';
        if(val < 10) return val;
        return std::toupper(c)-'A'+10;
    }

    unsigned getHex(const std::string& line, unsigned index)
    {
        return hexCharToInt(line[index])*16 + hexCharToInt(line[index+1]);
    }
}

void ActionNameEditor::setActionNameStyle(const std::string& name,
                                          unsigned r,
                                          unsigned g,
                                          unsigned b,
                                          const std::string& dashPattern)
{
    for(unsigned i = 0; i < actionNames.size(); ++i)
    {
        if(name == actionNames[i].name.getText())
        {
            actionNames[i].color.red = r;
            actionNames[i].color.green = g;
            actionNames[i].color.blue = b;

            std::vector<gint8>& pattern = actionNames[i].dashPattern;
            pattern.clear();
            if(dashPattern.empty()) return;

            char c = dashPattern[0];
            gint8 counter = 0;
            for(unsigned j = 0; j < dashPattern.length(); ++j)
            {
                if(dashPattern[j] == c) counter += 2;
                else
                {
                    c = dashPattern[j];
                    pattern.push_back(counter);
                    counter = 2;
                }
            }
            pattern.push_back(counter);
            if(pattern.size() == 1)
                pattern.clear();
            else if(pattern.size() % 2 == 1)
                pattern.push_back(2);

            return;
        }
    }
}

void ActionNameEditor::setStatePropNameStyle(const std::string& name,
                                             unsigned r,
                                             unsigned g,
                                             unsigned b)
{
    for(unsigned i = 0; i < statePropNames.size(); ++i)
    {
        if(name == statePropNames[i].name.getText())
        {
            statePropNames[i].color.red = r;
            statePropNames[i].color.green = g;
            statePropNames[i].color.blue = b;
            return;
        }
    }
}

void ActionNameEditor::loadStyleFile(const std::string& filename)
{
    std::ifstream infile(filename.c_str());
    if(!infile.good()) return;

    std::string line;

    while(true)
    {
        std::getline(infile, line);
        if(!infile.good()) break;

        bool readingStateProp = false;
        unsigned si = 0;
        while(si < line.length() && std::isspace(line[si])) ++si;
        if(si == line.length()) continue;

        if(si < line.length()-3 && line.compare(si, 3, "sp:") == 0)
        {
            readingStateProp = true;
            si += 3;
            while(si < line.length() && std::isspace(line[si])) ++si;
        }

        char delim = line[si];
        unsigned ei = si+1;
        while(ei < line.length() &&
              (line[ei] != delim ||
               (line[ei] == delim && line[ei-1] == '\\')))
            ++ei;
        if(ei == line.length() || ei == si+1) continue;

        std::string name(line, si+1, ei-si-1);
        for(unsigned i = 0; i < name.length(); ++i)
            if(name[i] == '\\') name.erase(i, 1);

        si = ei+1;
        while(si < line.length() && std::isspace(line[si])) ++si;
        if(si == line.length() || line[si] != '#') continue;

        unsigned r = 256*getHex(line, si+1);
        unsigned g = 256*getHex(line, si+3);
        unsigned b = 256*getHex(line, si+5);

        std::string dashPattern;

        if(!readingStateProp)
        {
            si += 7;
            while(si < line.length() && std::isspace(line[si])) ++si;
            if(si < line.length())
            {
                delim = line[si];
                ei = si+1;
                while(ei < line.length() && line[ei] != delim) ++ei;
                if(ei < line.length())
                    dashPattern = line.substr(si+1, ei-si-1);
            }
        }

        if(readingStateProp)
            setStatePropNameStyle(name, r, g, b);
        else
            setActionNameStyle(name, r, g, b, dashPattern);
    }
}

void ActionNameEditor::saveStyleFile(const std::string& filename) const
{
    std::FILE* ofile = std::fopen(filename.c_str(), "w");
    if(!ofile) return;

    for(unsigned i = 0; i < actionNames.size(); ++i)
    {
        std::string actionName = actionNames[i].name.getText();
        for(unsigned j = 0; j < actionName.length(); ++j)
            if(actionName[j] == '"')
                actionName.insert(j++, 1, '\\');

        const GdkColor& color = actionNames[i].color;
        std::fprintf(ofile, "\"%s\" #%02X%02X%02X", actionName.c_str(),
                     color.red/256, color.green/256, color.blue/256);

        const std::vector<gint8>& pattern = actionNames[i].dashPattern;
        if(!pattern.empty())
        {
            char c = 'x';
            std::fprintf(ofile, " \"");
            for(unsigned j = 0; j < pattern.size(); ++j)
            {
                for(gint8 k = 0; k < pattern[j]/2; ++k)
                    std::fprintf(ofile, "%c", c);
                if(c == 'x') c = ' ';
                else c = 'x';
            }
            std::fprintf(ofile, "\"");
        }

        std::fprintf(ofile, "\n");
    }

    for(unsigned i = 0; i < statePropNames.size(); ++i)
    {
        std::string statePropName = statePropNames[i].name.getText();
        for(unsigned j = 0; j < statePropName.length(); ++j)
            if(statePropName[j] == '"')
                statePropName.insert(j++, 1, '\\');

        const GdkColor& color = statePropNames[i].color;
        std::fprintf(ofile, "sp: \"%s\" #%02X%02X%02X\n",
                     statePropName.c_str(),
                     color.red/256, color.green/256, color.blue/256);
    }

    std::fclose(ofile);
}
