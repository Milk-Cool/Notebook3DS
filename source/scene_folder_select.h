#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <citro2d.h>
#include "app_state.h"

extern const char* scene_folder_select_name;

bool scene_folder_select_init(AppState* state);
const char* scene_folder_select_input(AppState* state, u32 down, u32 held);
const char* scene_folder_select_render(AppState* state, C3D_RenderTarget* top);
void scene_folder_select_deinit(AppState* state);