#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <citro2d.h>
#include "app_state.h"

extern const char* scene_danger_zone_name;

bool scene_danger_zone_init(AppState* state);
const char* scene_danger_zone_input(AppState* state, u32 down, u32 held);
const char* scene_danger_zone_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom);
void scene_danger_zone_deinit(AppState* state);