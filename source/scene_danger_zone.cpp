#include "scene_danger_zone.h"
#include "common.h"

const char* scene_danger_zone_name = "danger_zone";

static C2D_TextBuf g_staticBuf;
static C2D_Text g_staticText;

bool scene_danger_zone_init(AppState* state) {
    g_staticBuf  = C2D_TextBufNew(4096);
	// g_dynamicBuf = C2D_TextBufNew(4096);

	C2D_TextParse(&g_staticText, g_staticBuf,
        "Danger zone\n"
        "2x - clear\n"
        " - quit without saving\n"
        "\n"
        "Press  to go back"
    );
	C2D_TextOptimize(&g_staticText);
    return true;
}
const char* scene_danger_zone_input(AppState* state, u32 down, u32 held) {
    if(down & KEY_B)
        return nullptr;

    u64 curtime = osGetTime();
    if(down & KEY_A) {
        state->dstate.input_queue_a.push(curtime);
        if(state->dstate.input_queue_a.size() >= 2) {
            if(state->dstate.input_queue_a.size() == 3) state->dstate.input_queue_a.pop();
            if(state->dstate.input_queue_a.back() - state->dstate.input_queue_a.front() < DBLPRESS_MAX_DELAY) {
                state->current_pages[state->current_page_index].shapes.clear();
                clear_undid(state);
                return nullptr;
            }
        }
    } else if(down & KEY_X) {
        state->dstate.exit = true;
        return nullptr;
    }
    
    return "";
}
const char* scene_danger_zone_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom) {
    // BOTTOM
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, accent_bg);
    C2D_SceneBegin(bottom);

    C3D_FrameEnd(0);

	// TOP
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, accent_bg);
    C2D_SceneBegin(top);

	// Draw static text strings
	C2D_DrawText(&g_staticText, C2D_AlignCenter, 200, 75, 0, .55, .55);

    C3D_FrameEnd(0);

    return "";
}
void scene_danger_zone_deinit(AppState* state) {
    // Delete the text buffers
	C2D_TextBufDelete(g_staticBuf);
}