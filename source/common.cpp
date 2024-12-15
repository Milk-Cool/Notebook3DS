#include "common.h"

string get_input_name() {
    SwkbdState swkbd;
    char name[60];
    SwkbdButton button = SWKBD_BUTTON_NONE;
    swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, -1);
    swkbdSetHintText(&swkbd, "Enter the name");
    swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT, "Cancel", false);
    swkbdSetButton(&swkbd, SWKBD_BUTTON_RIGHT, "Create", true);
    button = swkbdInputText(&swkbd, name, sizeof(name));
    if(button == SWKBD_BUTTON_RIGHT)
        return string(name);
    return "";
}

u32 accent_bg = C2D_Color32(245, 162, 206, 0xFF);