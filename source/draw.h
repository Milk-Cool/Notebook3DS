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
void draw_shape(C3D_RenderTarget* top, Shape shape);