#include "scene_topic_select.h"
#include "backend.h"
#include "common.h"
#include "scene_topic_delete.h"
#include "scene_page.h"

const char* scene_topic_select_name = "topic_select";

static C2D_TextBuf g_staticBuf, g_dynamicBuf;
static C2D_Text g_staticText, g_staticTitle, g_buttons[4], g_sorting;

static s32 current_selection;

void init_topics(AppState* state) {
	state->current_topics = get_topics(state->folders[state->current_folder_index].id,
			state->sort_type, state->sort_direction);
	current_selection = 0;
}
void deinit_topics(AppState* state) {
	state->current_topics.erase(state->current_topics.begin(), state->current_topics.end());
}
void reinit_topics(AppState* state) {
	deinit_topics(state); init_topics(state);
}

bool scene_topic_select_init(AppState* state) {
	// Create two text buffers: one for static text, and another one for
	// dynamic text - the latter will be cleared at each frame.
	g_staticBuf  = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
	g_dynamicBuf = C2D_TextBufNew(4096);

	init_topics(state);

	// Parse the static text strings
	C2D_TextParse(&g_staticText, g_staticBuf, "Select topic or press  to create a new one\nPress  to remove, or SELECT to rename\nHold  & press  to change sorting method\nHold  & press  to change sorting direction");
	C2D_TextParse(&g_staticTitle, g_staticBuf, state->folders[state->current_folder_index].name.c_str());

	C2D_TextParse(&g_buttons[0], g_staticBuf, "Back");
	C2D_TextParse(&g_buttons[1], g_staticBuf, "Create");
	C2D_TextParse(&g_buttons[2], g_staticBuf, "Remove");
	C2D_TextParse(&g_buttons[3], g_staticBuf, "Rename");

	// Optimize the static text strings
	C2D_TextOptimize(&g_staticText);
	C2D_TextOptimize(&g_staticTitle);
	for(int i = 0; i < 3; i++)
		C2D_TextOptimize(&g_buttons[i]);
    return true;
}
static void wrap_selection(AppState* state) {
	if(current_selection >= (s32)state->current_topics.size()) current_selection = 0;
	if(current_selection < 0) current_selection = state->current_topics.size() - 1;
}
static void rename(AppState* state) {
	string name = get_input_name();
	if(name != "") {
		rename_topic(state->folders[state->current_folder_index].id, state->current_topics[current_selection].id, name);
		reinit_topics(state);
	}
}
static void create(AppState* state) {
	string name = get_input_name();
	if(name != "") {
		create_topic(state->folders[state->current_folder_index].id, name);
		reinit_topics(state);
	}
}
const char* scene_topic_select_input(AppState* state, u32 down, u32 held) {
	// input events come before rendering
	if(state->needs_reinit) {
		reinit_topics(state);
		state->needs_reinit = false;
	}

    if (down & KEY_START || down & KEY_B) {
        return nullptr;
	}
	if(down & KEY_L && held & KEY_R) {
		// TODO: cycle, not just change
		if(state->sort_direction == SortDirectionAscending)
			state->sort_direction = SortDirectionDescending;
		else
			state->sort_direction = SortDirectionAscending;
		reinit_topics(state);
		return "";
	} else if(down & KEY_R && held & KEY_L) {
		if(state->sort_type == SortTypeAlphabetical)
			state->sort_type = SortTypeModification;
		else
			state->sort_type = SortTypeAlphabetical;
		reinit_topics(state);
		return "";
	}

	if(state->current_topics.size() != 0) {
		if(down & KEY_TOUCH) {
			state->dstate.touch_in_another_scene = true;
			touchPosition touch;
			hidTouchRead(&touch);
			if(touch.px >= 5 && touch.px < 78 && touch.py >= 5 && touch.py < 40) {
				return nullptr;
			} else if(touch.px >= 83 && touch.px < 156 && touch.py >= 5 && touch.py < 40) {
				create(state);
			} else if(touch.px >= 161 && touch.px < 234 && touch.py >= 5 && touch.py < 40) {
				state->current_topic_index = current_selection;
				return scene_topic_delete_name;
			} else if(touch.px >= 239 && touch.px < 315 && touch.py >= 5 && touch.py < 40) {
				rename(state);
			}
			s32 new_selection = get_selection(current_selection, state->folders.size(), touch);
			if(new_selection == get_stop()) {
				state->current_topic_index = current_selection;
				// we do not load pages in scene_page but rather here
				state->current_pages = get_pages(
					state->folders[state->current_folder_index].id,
					state->current_topics[state->current_topic_index].id
				);
				return scene_page_name;
			}
			current_selection = new_selection;
			wrap_selection(state);
		} else
			state->dstate.touch_in_another_scene = false;

		if(down & KEY_A) {
			state->current_topic_index = current_selection;
			// we do not load pages in scene_page but rather here
			state->current_pages = get_pages(
				state->folders[state->current_folder_index].id,
				state->current_topics[state->current_topic_index].id
			);
			return scene_page_name;
		} else if(down & KEY_DOWN) {
			current_selection++;
			wrap_selection(state);
		} else if(down & KEY_UP) {
			current_selection--;
			wrap_selection(state);
		} else if(down & KEY_Y) {
			state->current_topic_index = current_selection;
			return scene_topic_delete_name;
		} else if(down & KEY_SELECT) {
			rename(state);
		}
	} else if(down & KEY_TOUCH) {
		state->dstate.touch_in_another_scene = true;
		touchPosition touch;
		hidTouchRead(&touch);
		if(touch.px >= 5 && touch.px < 78 && touch.py >= 5 && touch.py < 40) {
			return nullptr;
		} else if(touch.px >= 83 && touch.px < 156 && touch.py >= 5 && touch.py < 40) {
			create(state);
		}
	} else state->dstate.touch_in_another_scene = false;

	if(down & KEY_X) {
		create(state);
	}
    return "";
}
const char* scene_topic_select_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom) {
	// BOTTOM
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, accent_bg);
    C2D_SceneBegin(bottom);
	// Clear the dynamic text buffer
	C2D_TextBufClear(g_dynamicBuf);

	C2D_DrawRectSolid(5, 5, 0, 73, 35, C2D_Color32(0x00, 0x00, 0x00, 0x9F));
	C2D_DrawRectSolid(83, 5, 0, 73, 35, C2D_Color32(0x00, 0x00, 0x00, 0x9F));
	C2D_DrawRectSolid(161, 5, 0, 73, 35, C2D_Color32(0x00, 0x00, 0x00, 0x9F));
	C2D_DrawRectSolid(239, 5, 0, 76, 35, C2D_Color32(0x00, 0x00, 0x00, 0x9F));

	C2D_DrawRectSolid(5, 45, 0, 310, 60, C2D_Color32(0x00, 0x00, 0x00, 0x9F));
	C2D_DrawRectSolid(5, 110, 0, 310, 60, C2D_Color32(0x00, 0x00, 0x00, 0x9F));
	C2D_DrawRectSolid(5, 175, 0, 310, 60, C2D_Color32(0x00, 0x00, 0x00, 0x9F));

	if(state->current_topics.size() != 0) {
		u8 sel_y;
		u32 first;
		if(current_selection == 0) {
			sel_y = 45;
			first = 0;
		} else if(current_selection == (s32)state->current_topics.size() - 1 && current_selection >= 2) {
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
		for(u32 i = first; i < first + 3 && i < state->current_topics.size(); i++) {
			C2D_Text dynText;
			C2D_TextParse(&dynText, g_dynamicBuf, state->current_topics[i].name.c_str());
			C2D_TextOptimize(&dynText);
			C2D_DrawText(&dynText, C2D_AlignCenter | C2D_WithColor, 160, 65 + 65 * (i - first), 0, 0.5f, 0.5f, white);
		}
	}
	for(int i = 0; i < 4; i++)
		C2D_DrawText(&g_buttons[i], C2D_AlignCenter | C2D_WithColor, 40 + 78 * i, 12.5, 0, .55, .55, C2D_Color32(0xff, 0xff, 0xff, 0xff));

    C3D_FrameEnd(0);

	// TOP
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, accent_bg);
    C2D_SceneBegin(top);

	// Draw static text strings
	C2D_DrawText(&g_staticTitle, C2D_AlignCenter, 200, 70, 0, .8, .8);
	C2D_DrawText(&g_staticText, C2D_AlignCenter, 200, 105, 0, .55, .55);

	C2D_TextParse(&g_sorting, g_dynamicBuf, (string("Sorting ") + (
		state->sort_type == SortTypeAlphabetical ? "alphabetically" : (state->sort_type == SortTypeModification ? "by modification" : "randomly")
	) + " " + (
		state->sort_direction == SortDirectionAscending ? "(ascending)" : "(descending)"
	)).c_str());
	C2D_TextOptimize(&g_sorting);
	C2D_DrawText(&g_sorting, C2D_AlignCenter, 200, 225, 0, .4, .4);

    C3D_FrameEnd(0);

    return "";
}
void scene_topic_select_deinit(AppState* state) {
	// Delete the text buffers
	C2D_TextBufDelete(g_dynamicBuf);
	C2D_TextBufDelete(g_staticBuf);

	// idk if this is necessary but whatever
	deinit_topics(state);
}