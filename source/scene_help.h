#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <citro2d.h>
#include "app_state.h"

extern const char* scene_help_name;

bool scene_help_init(AppState* state);
const char* scene_help_input(AppState* state, u32 down, u32 held);
const char* scene_help_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom);
void scene_help_deinit(AppState* state);