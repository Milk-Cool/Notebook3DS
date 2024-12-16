#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <citro2d.h>
#include "app_state.h"

extern const char* scene_page_name;

bool scene_page_init(AppState* state);
const char* scene_page_input(AppState* state, u32 down, u32 held);
const char* scene_page_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom);
void scene_page_deinit(AppState* state);