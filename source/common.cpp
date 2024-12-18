#include "common.h"
#include <limits>

#define OVERCLOCK (1 << 0)
#define L2_CACHE (1 << 1)

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
string get_input_text() {
    SwkbdState swkbd;
    char text[60];
    SwkbdButton button = SWKBD_BUTTON_NONE;
    swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 2, -1);
    swkbdSetHintText(&swkbd, "Enter the text");
    swkbdSetButton(&swkbd, SWKBD_BUTTON_LEFT, "Cancel", false);
    swkbdSetButton(&swkbd, SWKBD_BUTTON_RIGHT, "Place", true);
    button = swkbdInputText(&swkbd, text, sizeof(text));
    if(button == SWKBD_BUTTON_RIGHT)
        return string(text);
    return "";
}

float dist2(float x1, float y1, float x2, float y2) {
    return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}
float min_dist2(float x1, float y1, float x2, float y2, float x3, float y3) {
    // x1y1x2y2 is the segment, x3y3 is the other dot
    const float l2 = dist2(x1, y1, x2, y2);
    if(l2 == 0.0) return dist2(x1, y1, x3, y3);

    const float t = max(0.0f, min(1.0f, ((x3 - x1) * (x2 - x1) + (y3 - y1) * (y2 - y1)) / l2));
    return dist2(x3, y3, x1 + t * (x2 - x1), y1 + t * (y2 - y1));
};
s32 get_selection(s32 current_selection, u32 size, touchPosition touch) {
    if(current_selection == 0) {
        if(touch.py >= 110 && touch.py < 170) {
            current_selection++;
        } else if(touch.py >= 45 && touch.py < 105) {
            return numeric_limits<s32>::min();
        } else if(touch.py >= 175 && touch.py < 235) {
            current_selection += 2;
        }
    }
    else if(current_selection == (s32)size - 1
            && size >= 3) {
        if(touch.py >= 110 && touch.py < 170) {
            current_selection--;
        } else if(touch.py >= 45 && touch.py < 105) {
            current_selection -= 2;
        } else if(touch.py >= 175 && touch.py < 235) {
            return numeric_limits<s32>::min();
        }
    }
    else {
        if(touch.py >= 110 && touch.py < 170) {
            return numeric_limits<s32>::min();
        } else if(touch.py >= 45 && touch.py < 105) {
            current_selection--;
        } else if(touch.py >= 175 && touch.py < 235) {
            current_selection++;
        }
    }
    return current_selection;
}
s32 get_stop() {
    return numeric_limits<s32>::min();
}
void clear_undid(AppState* state) {
    state->current_pages[state->current_page_index].undid.clear();
}

u32 accent_bg = C2D_Color32(245, 162, 206, 0xFF);

void overclock() {
    if(!PTMSYSM_CheckNew3DS()) return;
    PTMSYSM_ConfigureNew3DSCPU(OVERCLOCK | L2_CACHE);
}