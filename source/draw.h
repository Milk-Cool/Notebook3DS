#pragma once
extern "C" {
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <citro2d.h>
}
#include "backend.h"

void draw_init();
void draw_deinit();
void draw_shape(Shape shape, uint32_t scroll);
void draw_shape_top(Shape shape, uint32_t scroll);