#pragma once
extern "C" {
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <citro2d.h>
}
#include <string>
#include "app_state.h"
using namespace std;

#define DBLPRESS_MAX_DELAY 700

#define PAGE_HEIGHT (240 * 10)
#define CURRENT_VERSION 10000 // AABBCC = AA.BB.CC

string get_input_name();
string get_input_text();
float dist2(float x1, float y1, float x2, float y2);
float min_dist2(float x1, float y1, float x2, float y2, float x3, float y3);
s32 get_selection(s32 current_selection, u32 size, touchPosition touch);
s32 get_stop();
void clear_undid(AppState* state);
extern u32 accent_bg;
void overclock();