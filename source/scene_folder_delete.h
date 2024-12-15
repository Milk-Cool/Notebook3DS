#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <citro2d.h>
#include "app_state.h"

extern const char* scene_folder_delete_name;

bool scene_folder_delete_init(AppState* state);
const char* scene_folder_delete_input(AppState* state, u32 down, u32 held);
const char* scene_folder_delete_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom);
void scene_folder_delete_deinit(AppState* state);