/*******************************************************************************************
*
*   SimpleGui v1.0.0 - Tool Description
*
*   MODULE USAGE:
*       #define GUI_SIMPLE_GUI_IMPLEMENTATION
*       #include "gui_simple_gui.h"
*
*       INIT: GuiSimpleGuiState state = InitGuiSimpleGui();
*       DRAW: GuiSimpleGui(&state);
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 raylib technologies. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#ifndef GUI_SIMPLE_GUI_H
#define GUI_SIMPLE_GUI_H

typedef struct {
    // Define controls variables
    int ListView005ScrollIndex;
    int ListView005Active;            // ListView: ListView005
    Color ColorPicker001Value;            // ColorPicker: ColorPicker001

    // Define rectangles
    Rectangle layoutRecs[5];

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

} GuiSimpleGuiState;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiSimpleGuiState InitGuiSimpleGui(void);
void GuiSimpleGui(GuiSimpleGuiState *state);
static void Button002();                // Button: Button002 logic
static void Button003();                // Button: Button003 logic
static void Button004();                // Button: Button004 logic

#ifdef __cplusplus
}
#endif

#endif // GUI_SIMPLE_GUI_H

/***********************************************************************************
*
*   GUI_SIMPLE_GUI IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_SIMPLE_GUI_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiSimpleGuiState InitGuiSimpleGui(void)
{
    GuiSimpleGuiState state = { 0 };

    // Initilize controls variables
    state.ListView005ScrollIndex = 0;
    state.ListView005Active = 0;            // ListView: ListView005
    state.ColorPicker001Value = (Color){ 0, 0, 0, 0 };            // ColorPicker: ColorPicker001

    // Init controls rectangles
    state.layoutRecs[0] = (Rectangle){ 1700, 34, 120, 72 };// ListView: ListView005
    state.layoutRecs[1] = (Rectangle){ 1700, 117, 96, 96 };// ColorPicker: ColorPicker001
    state.layoutRecs[2] = (Rectangle){ 1700, 228, 120, 24 };// Button: Button002
    state.layoutRecs[3] = (Rectangle){ 1700, 260, 120, 24 };// Button: Button003
    state.layoutRecs[4] = (Rectangle){ 1700, 294, 120, 24 };// Button: Button004

    // Custom variables initialization

    return state;
}
// Button: Button002 logic
static void Button002()
{
    // TODO: Implement control logic
    printf("Button pressed.");
}
// Button: Button003 logic
static void Button003()
{
    // TODO: Implement control logic
    printf("Button pressed.");

}
// Button: Button004 logic
static void Button004()
{   
    // TODO: Implement control logic
    printf("Button pressed.");

}


void GuiSimpleGui(GuiSimpleGuiState *state)
{
    // Draw controls
    state->ListView005Active = GuiListView(state->layoutRecs[0], "TILES;ENEMIES;OBJECTS", &state->ListView005ScrollIndex, state->ListView005Active);
    state->ColorPicker001Value = GuiColorPicker(state->layoutRecs[1], state->ColorPicker001Value);
    if (GuiButton(state->layoutRecs[2], "LOAD TILESHEET")) Button002(); 
    if (GuiButton(state->layoutRecs[3], "SAVE FILE")) Button003(); 
    if (GuiButton(state->layoutRecs[4], "SAVE AND EXIT")) Button004(); 
}

#endif // GUI_SIMPLE_GUI_IMPLEMENTATION
