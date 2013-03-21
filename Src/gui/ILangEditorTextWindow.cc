#include "ILangEditorTextWindow.hh"

#include <sstream>
#include <string>

// ============================
// Transition properties dialog
// ============================
class ILangEditorTextWindow::TransitionPropertiesDialog
{
 public:
    TransitionPropertiesDialog(GtkWindow* parentWindow);
    ~TransitionPropertiesDialog();

    bool run(const gchar* precondText,
             const gchar* actionName,
             const gchar* actionParameters,
             const gchar* postcondText);

    const gchar* getNewPrecondText();
    const gchar* getNewActionParameters();
    const gchar* getNewPostcondText();

 private:
    GtkWindow* parent;
    GtkWidget* dialog;
    GtkWidget* precondView;
    GtkWidget* actionParametersEntry;
    GtkWidget* postcondView;
};

ILangEditorTextWindow::TransitionPropertiesDialog::TransitionPropertiesDialog
(GtkWindow* parentWindow):
    parent(parentWindow),
    dialog(0), precondView(0), actionParametersEntry(0), postcondView(0)
{}

ILangEditorTextWindow::TransitionPropertiesDialog::
~TransitionPropertiesDialog()
{
    if(dialog)
        gtk_widget_destroy(dialog);
}

bool ILangEditorTextWindow::TransitionPropertiesDialog::run
(const gchar* precondText, const gchar* actionName,
 const gchar* actionParameters, const gchar* postcondText)
{
    precondView = gtk_text_view_new();
    gtk_text_buffer_set_text(gtk_text_view_get_buffer
                             (GTK_TEXT_VIEW(precondView)), precondText, -1);
    GtkWidget* precondFrame = gtk_frame_new("Precondition");
    gtk_container_add(GTK_CONTAINER(precondFrame), precondView);

    postcondView = gtk_text_view_new();
    gtk_text_buffer_set_text(gtk_text_view_get_buffer
                             (GTK_TEXT_VIEW(postcondView)), postcondText, -1);
    GtkWidget* postcondFrame = gtk_frame_new("Postcondition");
    gtk_container_add(GTK_CONTAINER(postcondFrame), postcondView);

    actionParametersEntry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(actionParametersEntry), actionParameters);

    GtkWidget* actionNameBox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(actionNameBox), gtk_label_new(actionName),
                       TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(actionNameBox), actionParametersEntry,
                       TRUE, TRUE, 2);

    GtkWidget* actionNameFrame = gtk_frame_new("Action");
    gtk_container_add(GTK_CONTAINER(actionNameFrame), actionNameBox);

    dialog =
        gtk_dialog_new_with_buttons("Transition properties editor", parent,
                                    GtkDialogFlags
                                    (GTK_DIALOG_MODAL |
                                     GTK_DIALOG_DESTROY_WITH_PARENT),
                                    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                    GTK_STOCK_OK, GTK_RESPONSE_OK,
                                    NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), precondFrame,
                       TRUE, TRUE, 4);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), actionNameFrame,
                       TRUE, TRUE, 4);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), postcondFrame,
                       TRUE, TRUE, 4);

    gtk_widget_show_all(dialog);
    return gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK;
}

const gchar*
ILangEditorTextWindow::TransitionPropertiesDialog::getNewPrecondText()
{
    GtkTextBuffer* buffer =
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(precondView));
    GtkTextIter begin, end;
    gtk_text_buffer_get_bounds(buffer, &begin, &end);
    return gtk_text_buffer_get_text(buffer, &begin, &end, TRUE);
}

const gchar*
ILangEditorTextWindow::TransitionPropertiesDialog::getNewActionParameters()
{
    return gtk_entry_get_text(GTK_ENTRY(actionParametersEntry));
}

const gchar*
ILangEditorTextWindow::TransitionPropertiesDialog::getNewPostcondText()
{
    GtkTextBuffer* buffer =
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(postcondView));
    GtkTextIter begin, end;
    gtk_text_buffer_get_bounds(buffer, &begin, &end);
    return gtk_text_buffer_get_text(buffer, &begin, &end, TRUE);
}




// =====================
// ILangEditorTextWindow
// =====================

namespace
{
    const double ScrollingMargin = .25;
}

// --------------------------
// Constructor and destructor
// --------------------------
ILangEditorTextWindow::ILangEditorTextWindow():
    parent(0),
    textView(0), textBuffer(0),
    highlightTag(0),
    highlightedState(states.end()),
    selectedTransition(0)
{
}

ILangEditorTextWindow::~ILangEditorTextWindow()
{
}


// -------------------
// Main initialization
// -------------------
GtkWidget* ILangEditorTextWindow::initialize(GtkWindow* parentWindow)
{
    parent = parentWindow;

    textView = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView), GTK_WRAP_WORD);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textView), FALSE);

    //pango_font_description_set_family(textView->style->font_desc, "Courier");
    //gtk_widget_modify_font(textView, textView->style->font_desc);
    PangoFontDescription* font = pango_font_description_from_string("Courier");
    gtk_widget_modify_font(textView, font);
    pango_font_description_free(font);

    GtkWidget* scrolledTextWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledTextWindow),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type
        (GTK_SCROLLED_WINDOW(scrolledTextWindow), GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER(scrolledTextWindow), textView);

    textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));

    gtk_text_mark_set_visible(gtk_text_buffer_get_insert(textBuffer), FALSE);

    if(highlightTag == 0)
        highlightTag =
            gtk_text_buffer_create_tag(textBuffer, NULL,
                                       "background", "#FFE0A0",
                                       "background-set", TRUE,
                                       NULL);

    return scrolledTextWindow;
}


// -----
// State
// -----
ILangEditorTextWindow::State::State(unsigned n, GtkTextBuffer* textBuffer):
    stateNumber(n)
{
    if(textBuffer == 0) return;

    GtkTextIter endIter;
    gtk_text_buffer_get_end_iter(textBuffer, &endIter);

    textBeginMark =
        gtk_text_buffer_create_mark(textBuffer, NULL, &endIter, TRUE);

    std::ostringstream stateText;
    stateText << "State" << n;
    name = stateText.str();
    gtk_text_buffer_insert(textBuffer, &endIter,
                           (std::string("*")+name+":").c_str(), -1);

    stateNameEndMark =
        gtk_text_buffer_create_mark(textBuffer, NULL, &endIter, TRUE);

    gtk_text_buffer_insert(textBuffer, &endIter, "\n", -1);

    GtkTextMark* mark =
        gtk_text_buffer_create_mark(textBuffer, NULL, &endIter, TRUE);

    gtk_text_buffer_insert(textBuffer, &endIter, "\n", -1);

    textEndMark =
        gtk_text_buffer_create_mark(textBuffer, NULL, &endIter, TRUE);

    gtk_text_buffer_get_iter_at_mark(textBuffer, &endIter, mark);
    transitionsTextEndMark =
        gtk_text_buffer_create_mark(textBuffer, NULL, &endIter, FALSE);
    gtk_text_buffer_delete_mark(textBuffer, mark);
}

void ILangEditorTextWindow::State::remove(GtkTextBuffer* textBuffer)
{
    GtkTextIter begin, end;
    gtk_text_buffer_get_iter_at_mark(textBuffer, &begin, textBeginMark);
    gtk_text_buffer_get_iter_at_mark(textBuffer, &end, textEndMark);
    gtk_text_buffer_delete(textBuffer, &begin, &end);

    gtk_text_buffer_delete_mark(textBuffer, textBeginMark);
    gtk_text_buffer_delete_mark(textBuffer, textEndMark);
}

void ILangEditorTextWindow::State::addTransition(unsigned id,
                                                 State* endState,
                                                 const char* actionName,
                                                 GtkTextBuffer* textBuffer)
{
    transitions.push_back(Transition(id, textBuffer, transitionsTextEndMark,
                                     actionName, endState->name.c_str()));
}

void ILangEditorTextWindow::State::highlight(GtkTextBuffer* textBuffer,
                                             GtkTextTag* highlightTag)
{
    GtkTextIter begin, end;
    gtk_text_buffer_get_iter_at_mark(textBuffer, &begin, textBeginMark);
    gtk_text_buffer_get_iter_at_mark(textBuffer, &end, stateNameEndMark);

    gtk_text_buffer_apply_tag(textBuffer, highlightTag, &begin, &end);
}

void ILangEditorTextWindow::State::unhighlight(GtkTextBuffer* textBuffer,
                                               GtkTextTag* highlightTag)
{
    GtkTextIter begin, end;
    gtk_text_buffer_get_iter_at_mark(textBuffer, &begin, textBeginMark);
    gtk_text_buffer_get_iter_at_mark(textBuffer, &end, stateNameEndMark);

    gtk_text_buffer_remove_tag(textBuffer, highlightTag, &begin, &end);
}

void ILangEditorTextWindow::State::scrollToScreen(GtkWidget* textView,
                                                  GtkTextBuffer* textBuffer,
                                                  unsigned transitionID)
{
    GtkTextMark* mark = 0;
    if(transitionID != 0)
    {
        for(std::list<Transition>::iterator i = transitions.begin();
            i != transitions.end(); ++i)
        {
            if(i->getID() == transitionID)
            {
                mark = i->getPrecondBeginMark();
                break;
            }
        }
    }
    if(mark == 0)
    {
        mark =
            transitions.empty() ?
            transitionsTextEndMark :
            transitions.front().getPrecondBeginMark();
    }

    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(textView), mark,
                                 ScrollingMargin, FALSE, 0, 0);
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(textBuffer, &iter, mark);
    gtk_text_buffer_place_cursor(textBuffer, &iter);
}

ILangEditorTextWindow::Transition*
ILangEditorTextWindow::State::getTransition(unsigned id)
{
    for(std::list<Transition>::iterator i = transitions.begin();
        i != transitions.end(); ++i)
        if(i->getID() == id) return &(*i);
    return 0;
}


// ----------
// Transition
// ----------
ILangEditorTextWindow::Transition::Transition(unsigned ID,
                                              GtkTextBuffer* textBuf,
                                              GtkTextMark* where,
                                              const char* actionName,
                                              const char* destStateName):
    id(ID), textBuffer(textBuf),
    textBeginMark(0),
    precondBeginMark(0), precondEndMark(0),
    nameBeginMark(0), nameEndMark(0),
    postcondBeginMark(0), postcondEndMark(0),
    textEndMark(0)
{
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(textBuffer, &iter, where);

    textBeginMark =
        gtk_text_buffer_create_mark(textBuffer, NULL, &iter, TRUE);
    gtk_text_buffer_insert(textBuffer, &iter, "  [", -1);
    precondBeginMark =
        gtk_text_buffer_create_mark(textBuffer, NULL, &iter, TRUE);
    gtk_text_buffer_insert(textBuffer, &iter, "] ", -1);
    nameBeginMark =
        gtk_text_buffer_create_mark(textBuffer, NULL, &iter, TRUE);
    gtk_text_buffer_insert(textBuffer, &iter, " -> [", -1);
    postcondBeginMark =
        gtk_text_buffer_create_mark(textBuffer, NULL, &iter, TRUE);
    gtk_text_buffer_insert(textBuffer, &iter, "] ", -1);
    gtk_text_buffer_insert(textBuffer, &iter, destStateName, -1);
    gtk_text_buffer_insert(textBuffer, &iter, "\n", -1);
    textEndMark =
        gtk_text_buffer_create_mark(textBuffer, NULL, &iter, TRUE);

    gtk_text_buffer_get_iter_at_mark(textBuffer, &iter, precondBeginMark);
    precondEndMark =
        gtk_text_buffer_create_mark(textBuffer, NULL, &iter, FALSE);
    gtk_text_buffer_get_iter_at_mark(textBuffer, &iter, postcondBeginMark);
    postcondEndMark =
        gtk_text_buffer_create_mark(textBuffer, NULL, &iter, FALSE);

    gtk_text_buffer_get_iter_at_mark(textBuffer, &iter, nameBeginMark);
    nameEndMark =
        gtk_text_buffer_create_mark(textBuffer, NULL, &iter, FALSE);
    gtk_text_buffer_insert(textBuffer, &iter, actionName, -1);
}

const gchar* ILangEditorTextWindow::Transition::getText(GtkTextMark* beginMark,
                                                        GtkTextMark* endMark)
{
    GtkTextIter begin, end;
    gtk_text_buffer_get_iter_at_mark(textBuffer, &begin, beginMark);
    gtk_text_buffer_get_iter_at_mark(textBuffer, &end, endMark);
    return gtk_text_buffer_get_text(textBuffer, &begin, &end, TRUE);
}

void ILangEditorTextWindow::Transition::setText(GtkTextMark* beginMark,
                                                GtkTextMark* endMark,
                                                const gchar* text)
{
    GtkTextIter begin, end;
    gtk_text_buffer_get_iter_at_mark(textBuffer, &begin, beginMark);
    gtk_text_buffer_get_iter_at_mark(textBuffer, &end, endMark);
    gtk_text_buffer_delete(textBuffer, &begin, &end);
    gtk_text_buffer_insert(textBuffer, &begin, text, -1);
}

const gchar* ILangEditorTextWindow::Transition::getPrecondText()
{
    return getText(precondBeginMark, precondEndMark);
}

const gchar* ILangEditorTextWindow::Transition::getActionName()
{
    return getText(nameBeginMark, nameEndMark);
}

const gchar* ILangEditorTextWindow::Transition::getActionParameters()
{
    return "";
}

const gchar* ILangEditorTextWindow::Transition::getPostcondText()
{
    return getText(postcondBeginMark, postcondEndMark);
}

void ILangEditorTextWindow::Transition::setPrecondText(const gchar* text)
{
    setText(precondBeginMark, precondEndMark, text);
}

void ILangEditorTextWindow::Transition::setActionName(const gchar* text)
{
    setText(nameBeginMark, nameEndMark, text);
}

void ILangEditorTextWindow::Transition::setActionParameters(const gchar*)
{
}

void ILangEditorTextWindow::Transition::setPostcondText(const gchar* text)
{
    setText(postcondBeginMark, postcondEndMark, text);
}

void ILangEditorTextWindow::Transition::highlight(GtkTextTag* highlightTag)
{
    GtkTextIter begin, end;
    gtk_text_buffer_get_iter_at_mark(textBuffer, &begin, nameBeginMark);
    gtk_text_buffer_get_iter_at_mark(textBuffer, &end, nameEndMark);
    gtk_text_buffer_apply_tag(textBuffer, highlightTag, &begin, &end);
}

void ILangEditorTextWindow::Transition::unhighlight(GtkTextTag* highlightTag)
{
    GtkTextIter begin, end;
    gtk_text_buffer_get_iter_at_mark(textBuffer, &begin, nameBeginMark);
    gtk_text_buffer_get_iter_at_mark(textBuffer, &end, nameEndMark);
    gtk_text_buffer_remove_tag(textBuffer, highlightTag, &begin, &end);
}


// Assign 'selectedTransition' to point to the transition 'id':
void ILangEditorTextWindow::selectTransition(unsigned id)
{
    if(selectedTransition && selectedTransition->getID() == id)
        return;

    if(selectedTransition)
        selectedTransition->unhighlight(highlightTag);

    for(std::list<State>::iterator iter = states.begin();
        iter != states.end(); ++iter)
    {
        selectedTransition = iter->getTransition(id);
        if(selectedTransition)
        {
            selectedTransition->highlight(highlightTag);
            return;
        }
    }
}


// -----------------------------------------------------------
// LSTSEditorMainDrawingWindow::EventNotify callback functions
// -----------------------------------------------------------
void ILangEditorTextWindow::stateCreated(unsigned stateNumber,
                                         bool /*isInitialState*/)
{
    std::cerr << "stateCreated()\n";
    states.push_back(State(stateNumber, textBuffer));

    if(highlightedState != states.end())
        highlightedState->unhighlight(textBuffer, highlightTag);
    highlightedState = --states.end();
    states.back().highlight(textBuffer, highlightTag);
    states.back().scrollToScreen(textView, textBuffer);
}

void ILangEditorTextWindow::stateSelected(unsigned stateNumber)
{
    std::cerr << "stateSelected()\n";
    for(std::list<State>::iterator iter = states.begin();
        iter != states.end(); ++iter)
    {
        if(iter->number() == stateNumber)
        {
            if(highlightedState != states.end())
                highlightedState->unhighlight(textBuffer, highlightTag);
            highlightedState = iter;
            iter->highlight(textBuffer, highlightTag);
            iter->scrollToScreen(textView, textBuffer);
            break;
        }
    }
}

void ILangEditorTextWindow::stateDeleted(unsigned stateNumber)
{
    std::cerr << "stateDeleted()\n";
    for(std::list<State>::iterator iter = states.begin();
        iter != states.end(); ++iter)
    {
        if(iter->number() == stateNumber)
        {
            if(highlightedState == iter)
            {
                highlightedState->unhighlight(textBuffer, highlightTag);
                highlightedState = states.end();
            }
            iter->remove(textBuffer);
            states.erase(iter);
            break;
        }
    }
}

void ILangEditorTextWindow::stateWasMadeInitial(unsigned /*stateNumber*/)
{
    std::cerr << "stateWasMadeInitial()\n";
}

void ILangEditorTextWindow::statePropAddedToState(unsigned /*stateNumber*/,
                                                  unsigned statePropNumber,
                                                  const char* statePropName)
{
    std::cerr << "statePropAddedToState()\n";
    if(statePropNames.size() <= statePropNumber)
        statePropNames.resize(statePropNumber+1);
    statePropNames[statePropNumber] = statePropName;
}

void ILangEditorTextWindow::statePropRemovedFromState(unsigned /*stateNumber*/,
                                                      unsigned /*statePropNumber*/)
{
    std::cerr << "statePropRemovedFromState()\n";
}

void ILangEditorTextWindow::statePropRenamed(unsigned statePropNumber,
                                             const char* newName)
{
    std::cerr << "statePropRenamed() " << newName << "\n";
    statePropNames[statePropNumber] = newName;
}

void ILangEditorTextWindow::statePropRemoved(unsigned statePropNumber)
{
    std::cerr << "statePropRemoved()\n";
    statePropNames.erase(statePropNames.begin()+statePropNumber);
}

void ILangEditorTextWindow::transitionCreated(unsigned id,
                                              unsigned startStateNumber,
                                              unsigned endStateNumber,
                                              unsigned action,
                                              const char* actionName)
{
    std::cerr << "Created transition " << id << std::endl;

    if(actionNames.size() <= action)
        actionNames.resize(action+1);
    actionNames[action] = actionName;

    std::list<State>::iterator
        startState = states.end(), endState = states.end();

    for(std::list<State>::iterator iter = states.begin();
        iter != states.end(); ++iter)
    {
        if(iter->number() == startStateNumber)
        {
            startState = iter;
            if(endState != states.end()) break;
        }
        if(iter->number() == endStateNumber)
        {
            endState = iter;
            if(startState != states.end()) break;
        }
    }

    startState->addTransition(id, &(*endState), actionName, textBuffer);
    startState->scrollToScreen(textView, textBuffer, id);
    if(highlightedState != states.end())
        highlightedState->unhighlight(textBuffer, highlightTag);
    highlightedState = startState;
    startState->highlight(textBuffer, highlightTag);
}

void ILangEditorTextWindow::transitionSelected(unsigned id)
{
    std::cerr << "transitionSelected()\n";
    selectTransition(id);
}

void ILangEditorTextWindow::transitionUnselected(unsigned /*id*/)
{
    std::cerr << "transitionUnselected()\n";
}

void ILangEditorTextWindow::transitionDeleted(unsigned id)
{
    std::cerr << "Deleted transition " << id << std::endl;
}

void ILangEditorTextWindow::transitionActionChanged(unsigned /*id*/,
                                                    unsigned /*newAction*/)
{
    std::cerr << "transitionActionChanged()\n";
}

void ILangEditorTextWindow::editTransitionProperties(unsigned id)
{
    std::cerr << "editTransitionProperties()\n";
    selectTransition(id);

    TransitionPropertiesDialog dialog(parent);
    if(dialog.run(selectedTransition->getPrecondText(),
                  selectedTransition->getActionName(),
                  selectedTransition->getActionParameters(),
                  selectedTransition->getPostcondText()))
    {
        selectedTransition->setPrecondText(dialog.getNewPrecondText());
        selectedTransition->setActionParameters
            (dialog.getNewActionParameters());
        selectedTransition->setPostcondText(dialog.getNewPostcondText());
    }
}

void ILangEditorTextWindow::actionNameChanged(unsigned /*actionNumber*/,
                                              const char* /*newActionName*/)
{
    std::cerr << "actionNameChanged()\n";
}

void ILangEditorTextWindow::actionNameDeleted(unsigned /*actionNumber*/)
{
    std::cerr << "actionNameDeleted()\n";
}

void ILangEditorTextWindow::deleteEverything()
{
    std::cerr << "deleteEverything()\n";
    //states.clear();
}
