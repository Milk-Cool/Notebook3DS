extern "C" {
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>
#include <citro2d.h>
}
#include "backend.h"
#include "app_state.h"
#include "draw.h"
#include <stack>

// Scenes
#include "scene_folder_select.h"
#include "scene_folder_delete.h"
#include "scene_topic_select.h"
#include "scene_topic_delete.h"
#include "scene_page.h"
#include "scene_help.h"
#include "scene_color_select.h"
#include "scene_thickness_select.h"
#include "scene_tool_select.h"

using namespace std;

AppState state;

// Basic scene system
// Don't wanna make anything too complicated for now
stack<u32> scene_stack;
typedef bool (*SceneInit)(AppState* state);
typedef const char* (*SceneInput)(AppState* state, u32 down, u32 held);
typedef const char* (*SceneRender)(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom);
typedef void (*SceneDeinit)(AppState* state);
typedef struct {
	const char* name;
	SceneInit init;
	SceneInput input;
	SceneRender render;
	SceneDeinit deinit;
} Scene;
Scene scenes[] = {
	{
		.name = scene_folder_select_name,
		.init = scene_folder_select_init,
		.input = scene_folder_select_input,
		.render = scene_folder_select_render,
		.deinit = scene_folder_select_deinit
	},
	{
		.name = scene_topic_select_name,
		.init = scene_topic_select_init,
		.input = scene_topic_select_input,
		.render = scene_topic_select_render,
		.deinit = scene_topic_select_deinit
	},
	{
		.name = scene_folder_delete_name,
		.init = scene_folder_delete_init,
		.input = scene_folder_delete_input,
		.render = scene_folder_delete_render,
		.deinit = scene_folder_delete_deinit
	},
	{
		.name = scene_topic_delete_name,
		.init = scene_topic_delete_init,
		.input = scene_topic_delete_input,
		.render = scene_topic_delete_render,
		.deinit = scene_topic_delete_deinit
	},
	{
		.name = scene_page_name,
		.init = scene_page_init,
		.input = scene_page_input,
		.render = scene_page_render,
		.deinit = scene_page_deinit
	},
	{
		.name = scene_help_name,
		.init = scene_help_init,
		.input = scene_help_input,
		.render = scene_help_render,
		.deinit = scene_help_deinit
	},
	{
		.name = scene_color_select_name,
		.init = scene_color_select_init,
		.input = scene_color_select_input,
		.render = scene_color_select_render,
		.deinit = scene_color_select_deinit
	},
	{
		.name = scene_thickness_select_name,
		.init = scene_thickness_select_init,
		.input = scene_thickness_select_input,
		.render = scene_thickness_select_render,
		.deinit = scene_thickness_select_deinit
	},
	{
		.name = scene_tool_select_name,
		.init = scene_tool_select_init,
		.input = scene_tool_select_input,
		.render = scene_tool_select_render,
		.deinit = scene_tool_select_deinit
	},
};

void init() {
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
}
void deinit() {
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
}

void push_scene(const char* scene_name) {
	u32 ind = UINT32_MAX;
	for(u32 i = 0; i < sizeof(scenes) / sizeof(Scene); i++)
		if(!strcmp(scenes[i].name, scene_name)) {
			ind = i;
			break;
		}
	if(ind == UINT32_MAX) return;
	scene_stack.push(ind);
}

void init_new_scene() {
	u32 new_scene = scene_stack.top();
	scenes[new_scene].init(&state);
}

int main()
{
	// Initialize the libs
	init();
	init_backend();
	draw_init();

	state.needs_reinit = false;

	overclock();

	// Create screen
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	scene_stack.push(0);
	if(!scenes[0].init(&state)) {
		deinit();
		return 1;
	}

	// Main loop
	while (aptMainLoop())
	{
		u32 current_scene = scene_stack.top();
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();

		const char* res_input = scenes[current_scene].input(&state, kDown, kHeld);
		if(res_input == nullptr) {
			scenes[current_scene].deinit(&state);
			scene_stack.pop();
			if(scene_stack.size() == 0) break;
			continue;
		} else if(strcmp(res_input, "")) {
			push_scene(res_input);
			init_new_scene();
			continue;
		}

		// Render the scene
		const char* res_render = scenes[current_scene].render(&state, top, bottom);
		if(res_render == nullptr) {
			scenes[current_scene].deinit(&state);
			scene_stack.pop();
			if(scene_stack.size() == 0) break;
			continue;
		} else if(strcmp(res_input, "")) {
			push_scene(res_render);
			init_new_scene();
			continue;
		}
	}

	// Deinitialize the libs
	deinit();
	draw_deinit();
	return 0;
}
