#include "scene_topic_delete.h"
#include "backend.h"
#include "common.h"

const char* scene_topic_delete_name = "topic_delete";

static C2D_TextBuf g_staticBuf, g_dynamicBuf;
static C2D_Text g_staticText, g_staticTitle;

bool scene_topic_delete_init(AppState* state) {
	// Create two text buffers: one for static text, and another one for
	// dynamic text - the latter will be cleared at each frame.
	g_staticBuf  = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
	g_dynamicBuf = C2D_TextBufNew(4096);

	// Parse the static text strings
	C2D_TextParse(&g_staticText, g_staticBuf, "Press  to delete,  to cancel");
	C2D_TextParse(&g_staticTitle, g_staticBuf, state->current_topics[state->current_topic_index].name.c_str());

	// Optimize the static text strings
	C2D_TextOptimize(&g_staticText);
	C2D_TextOptimize(&g_staticTitle);
    return true;
}
const char* scene_topic_delete_input(AppState* state, u32 down, u32 held) {
    if(down & KEY_A) {
        remove_topic(state->folders[state->current_folder_index].id, state->current_topics[state->current_topic_index].id);
        state->needs_reinit = true;
        return nullptr;
    }
    if (down & KEY_START || down & KEY_B) {
        return nullptr;
	}

    return "";
}
const char* scene_topic_delete_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom) {
	// BOTTOM
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, accent_bg); // TODO: make color const
    C2D_SceneBegin(bottom);
	// Clear the dynamic text buffer
	C2D_TextBufClear(g_dynamicBuf);

	// Draw static text strings
	C2D_DrawText(&g_staticText, C2D_AlignCenter, 160, 105, 0, .55, .55);

    C3D_FrameEnd(0);

	// TOP
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, accent_bg);
    C2D_SceneBegin(top);

	C2D_DrawText(&g_staticTitle, C2D_AlignCenter, 200, 100, 0, .8, .8);

    C3D_FrameEnd(0);

    return "";
}
void scene_topic_delete_deinit(AppState* state) {
	// Delete the text buffers
	C2D_TextBufDelete(g_dynamicBuf);
	C2D_TextBufDelete(g_staticBuf);
}