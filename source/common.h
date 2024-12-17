#pragma once
extern "C" {
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <citro2d.h>
}
#include <string>
using namespace std;

#define PAGE_HEIGHT (240 * 10)

string get_input_name();
string get_input_text();
float dist2(float x1, float y1, float x2, float y2);
float min_dist2(float x1, float y1, float x2, float y2, float x3, float y3);
extern u32 accent_bg;