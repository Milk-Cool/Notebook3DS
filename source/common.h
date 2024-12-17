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
extern u32 accent_bg;