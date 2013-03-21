#ifndef GUI_CONSTANTS_HH
#define GUI_CONSTANTS_HH

// General settings:
// ----------------
#define MAIN_WINDOW_TITLE "TVT GUI v1.0beta"
#define DEFAULT_APP_WINDOW_SIZE 640, 480

// Drawing area general settings:
// -----------------------------
#define COLUMN_WIDTH 64
#define DAREA_MIN_HEIGHT 200
#define DAREA_BOTTOM_MARGIN 64
#define DRAWING_WINDOW_FONT_SIZE 8
#define MAIN_FG_COLOR 0, 0, 0
#define DIMMED_FG_COLOR 0xC000, 0xC000, 0xC000
#define WHITE_COLOR 0xFFFF, 0xFFFF, 0xFFFF
#define SOURCE_LSTS_COLUMN_COLOR 0xA000, 0xD000, 0xFFFF
#define LSTS_COLUMN_COLOR 0xFFFF, 0xFFFF, 0xFFFF
#define OPERATION_COLUMN_COLOR 0xFFFF, 0xF000, 0xD000

#define ITEM_HOVER_TEXT "Left-click&drag to move (+ctrl to move subtree), right-click for options"
#define COLUMN_HOVER_TEXT "Right-click for options"

// Source lsts graphic settings:
// ----------------------------
#define SOURCE_LSTS_GRAPHIC_BG_COLOR 0xFFFF, 0xFFFF, 0xFFFF
#define SOURCE_LSTS_GRAPHIC_TEXTLINE_COLOR 0x8000, 0x8000, 0x8000
#define SOURCE_LSTS_GRAPHIC_WIDTH 36
#define SOURCE_LSTS_GRAPHIC_HEIGHT 48
#define SOURCE_LSTS_GRAPHIC_FOLD_SIZE 8

// Result lsts graphic settings:
// ----------------------------
#define RESULT_LSTS_GRAPHIC_WIDTH 28
#define RESULT_LSTS_GRAPHIC_HEIGHT 38
#define RESULT_LSTS_SYMBOL_RADIUS 4

// Operation graphic settings:
// --------------------------
#define OPERATION_GRAPHIC_WIDTH (COLUMN_WIDTH-8)
#define OPERATION_GRAPHIC_HEIGHT (OPERATION_GRAPHIC_WIDTH*3/4)
#define MULT_INPUT_OPERATION_INPUTS_SPACING 12

// LSTS editor settings:
// --------------------
#define LSTS_EDITOR_WINDOW_TITLE "TVT graphical LSTS editor"
#define ILANG_EDITOR_WINDOW_TITLE "TVT graphical ilang editor"
#define LSTS_EDITOR_DEFAULT_WINDOW_SIZE 640, 480
#define LSTS_EDITOR_MARGINS_SIZE 64
#define LSTS_EDITOR_STATE_RADIUS 12
#define LSTS_EDITOR_TRANSITIONS_SPACING 12
#define LSTS_EDITOR_TRANSITION_HANDLE_SIZE 8
#define LSTS_EDITOR_TRANSITION_TO_SELF_SIZE (LSTS_EDITOR_STATE_RADIUS*3)
#define LSTS_EDITOR_TRANSITION_TO_SELF_WIDTH LSTS_EDITOR_STATE_RADIUS
#define LSTS_EDITOR_GENERATED_LAYOUT_STATES_SPACING 64
#define LSTS_EDITOR_INITIAL_STATE_BORDER_COLOR 0, 0x8000, 0xFFFF
#define LSTS_EDITOR_TRANSITION_HIGHLIGHT_COLOR 0xFFFF, 0xB000, 0x8000
#define LSTS_EDITOR_STATE_HOVER_TEXT "l-click: Select stateprop; l-click&drag: move; ctrl+click: create transition; r-click: options"
#define LSTS_EDITOR_TRANSITION_TO_SELF_HOVER_TEXT "l-click: select; l-click&drag: rotate; r-click: options"
#define LSTS_EDITOR_TRANSITION_HOVER_TEXT "Left-click: select; right-click: options"
#define LSTS_EDITOR_BACKGROUND_HOVER_TEXT "Right-click for options"

// LSTS viewing settings:
// ---------------------
#define LSTS_VIEW_ARROW_HEAD_LENGTH 14

// Action name editor settings:
// ---------------------------
#define ACTION_NAME_EDITOR_NAMES_SPACING 16
#define ACTION_NAME_EDITOR_SELECTION_COLOR 0xFFFF, 0xF000, 0xC000
#define ACTION_NAME_EDITOR_DEFAULT_TAU_GRAY_SHADE 0xC000
#define ACTION_NAME_EDITOR_HOVER_TEXT "Left-click to select, right-click for options"

// Helper macros
// -------------
#define TableRange(table) table, table+sizeof(table)/sizeof(table[0])

#endif
