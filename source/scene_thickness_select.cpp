#include "scene_thickness_select.h"
#include "common.h"

const char* scene_thickness_select_name = "thickness_select";

static C2D_TextBuf g_staticBuf, g_dynamicBuf;
static C2D_Text g_staticText;

static float current_thickness = 0;
static float limit = 5;

bool scene_thickness_select_init(AppState* state) {
    g_staticBuf  = C2D_TextBufNew(4096);
    g_dynamicBuf  = C2D_TextBufNew(4096);

    current_thickness = state->dstate.current_thickness;

	C2D_TextParse(&g_staticText, g_staticBuf,
        "Thickness select\n"
        "Press  to go back"
    );
	C2D_TextOptimize(&g_staticText);
    return true;
}
const char* scene_thickness_select_input(AppState* state, u32 down, u32 held) {
    if(down & KEY_B || down & KEY_A) {
        state->dstate.current_thickness = current_thickness;
        return nullptr;
    }

    else if(down & KEY_DLEFT)
        current_thickness -= .1;
    else if(down & KEY_DRIGHT)
        current_thickness += .1;
    else if(held & KEY_TOUCH) {
        touchPosition touch;
        hidTouchRead(&touch);
        current_thickness = limit / 2 + ((float)touch.px - 160) / 40;
    }
    
    if(current_thickness < 0)
        current_thickness = 0;
    else if(current_thickness > limit)
        current_thickness = limit;
    
    return "";
}
const char* scene_thickness_select_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom) {
    // BOTTOM
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, accent_bg);
    C2D_SceneBegin(bottom);

	// Clear the dynamic text buffer
	C2D_TextBufClear(g_dynamicBuf);

    C2D_Text dynText;
    C2D_TextParse(&dynText, g_dynamicBuf, to_string(current_thickness).c_str());
    C2D_TextOptimize(&dynText);
    float x = 160 + (current_thickness - limit / 2) * 40;
    C2D_DrawText(&dynText, C2D_AlignCenter, x, 80, 0, 0.5f, 0.5f);
    
    u32 black = C2D_Color32(0x00, 0x00, 0x00, 0xff);
    C2D_DrawTriangle(60, 120, black, 260, 110, black, 260, 130, black, 0);
    u32 white = C2D_Color32(0xff, 0xff, 0xff, 0xff);
    C2D_DrawCircleSolid(x, 120, 0, 15, white);

    C3D_FrameEnd(0);

	// TOP
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, accent_bg);
    C2D_SceneBegin(top);

	// Draw static text strings
	C2D_DrawText(&g_staticText, C2D_AlignCenter, 200, 95, 0, .55, .55);

    C3D_FrameEnd(0);

    return "";
}
void scene_thickness_select_deinit(AppState* state) {
    // Delete the text buffers
    C2D_TextBufDelete(g_dynamicBuf);
	C2D_TextBufDelete(g_staticBuf);
}