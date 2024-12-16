#include "scene_folder_select.h"
#include "backend.h"
#include "common.h"
#include "scene_topic_select.h"
#include "scene_folder_delete.h"

const char* scene_folder_select_name = "folder_select";

static C2D_TextBuf g_staticBuf, g_dynamicBuf;
static C2D_Text g_staticText, g_staticTitle;

static s32 current_selection;

void init_folders(AppState* state) {
	state->folders = get_folders();
	current_selection = 0;
}
void deinit_folders(AppState* state) {
	state->folders.erase(state->folders.begin(), state->folders.end());
}
void reinit_folders(AppState* state) {
	deinit_folders(state); init_folders(state);
}

bool scene_folder_select_init(AppState* state) {
	// Create two text buffers: one for static text, and another one for
	// dynamic text - the latter will be cleared at each frame.
	g_staticBuf  = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
	g_dynamicBuf = C2D_TextBufNew(4096);

	init_folders(state);

	// // Parse the static text strings
	C2D_TextParse(&g_staticText, g_staticBuf, "Select folder or press  to create a new one\nPress  to remove, or SELECT to rename");
	C2D_TextParse(&g_staticTitle, g_staticBuf, "Notebook3DS");

	// Optimize the static text strings
	C2D_TextOptimize(&g_staticText);
	C2D_TextOptimize(&g_staticTitle);
    return true;
}
const char* scene_folder_select_input(AppState* state, u32 down, u32 held) {
	// input events come before rendering
	if(state->needs_reinit) {
		reinit_folders(state);
		state->needs_reinit = false;
	}

    if (down & KEY_START) {
        return nullptr;
	}
	if(down & KEY_X) {
		string name = get_input_name();
		if(name != "") {
			create_folder(name);
			reinit_folders(state);
		}
	}

	if(state->folders.size() != 0) {
		if(down & KEY_DOWN) {
			current_selection++;
			if(current_selection == (s32)state->folders.size()) current_selection = 0;
		} else if(down & KEY_UP) {
			current_selection--;
			if(current_selection == -1) current_selection = state->folders.size() - 1;
		}

		if(down & KEY_A) {
			state->current_folder_index = current_selection;
			return scene_topic_select_name;
		} else if(down & KEY_Y) {
			state->current_folder_index = current_selection;
			return scene_folder_delete_name;
		} else if(down & KEY_SELECT) {
			string name = get_input_name();
			if(name != "") {
				rename_folder(state->folders[current_selection].id, name);
				reinit_folders(state);
			}
		}
	}
    return "";
}
const char* scene_folder_select_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom) {
	// BOTTOM
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, accent_bg);
    C2D_SceneBegin(bottom);
	// Clear the dynamic text buffer
	C2D_TextBufClear(g_dynamicBuf);

	// Draw static text strings
	C2D_DrawText(&g_staticText, C2D_AlignCenter, 160, 5, 0, .55, .55);

	C2D_DrawRectSolid(5, 45, 0, 310, 60, C2D_Color32(0x00, 0x00, 0x00, 0x9F));
	C2D_DrawRectSolid(5, 110, 0, 310, 60, C2D_Color32(0x00, 0x00, 0x00, 0x9F));
	C2D_DrawRectSolid(5, 175, 0, 310, 60, C2D_Color32(0x00, 0x00, 0x00, 0x9F));

	if(state->folders.size() != 0) {
		u8 sel_y;
		u32 first;
		if(current_selection == 0) {
			sel_y = 45;
			first = 0;
		} else if(current_selection == (s32)state->folders.size() - 1 && current_selection >= 2) {
			sel_y = 175;
			first = current_selection - 2;
		} else {
			sel_y = 110;
			first = current_selection - 1;
		}
		u32 white = C2D_Color32(0xff, 0xff, 0xff, 0xff);
		C2D_DrawLine(5, sel_y + 2, white, 315, sel_y + 2, white, 4, 0);
		C2D_DrawLine(5, sel_y + 58, white, 315, sel_y + 58, white, 4, 0);
		C2D_DrawLine(7, sel_y + 4, white, 7, sel_y + 56, white, 4, 0);
		C2D_DrawLine(313, sel_y + 4, white, 313, sel_y + 56, white, 4, 0);

		// Generate and draw dynamic text
		for(u32 i = first; i < first + 3 && i < state->folders.size(); i++) {
			C2D_Text dynText;
			C2D_TextParse(&dynText, g_dynamicBuf, state->folders[i].name.c_str());
			C2D_TextOptimize(&dynText);
			C2D_DrawText(&dynText, C2D_AlignCenter | C2D_WithColor, 160, 65 + 65 * (i - first), 0, 0.5f, 0.5f, white);
		}
	}

    C3D_FrameEnd(0);

	// TOP
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, accent_bg);
    C2D_SceneBegin(top);

	C2D_DrawText(&g_staticTitle, C2D_AlignCenter, 200, 100, 0, .8, .8);

    C3D_FrameEnd(0);

    return "";
}
void scene_folder_select_deinit(AppState* state) {
	// Delete the text buffers
	C2D_TextBufDelete(g_dynamicBuf);
	C2D_TextBufDelete(g_staticBuf);

	// idk if this is necessary but whatever
	deinit_folders(state);
}