#include "scene_page.h"
#include "common.h"
#include "scene_help.h"

const char* scene_page_name = "page";

static C2D_TextBuf g_staticBuf;
static C2D_Text g_staticText;

bool scene_page_init(AppState* state) {
    g_staticBuf  = C2D_TextBufNew(4096);
	// g_dynamicBuf = C2D_TextBufNew(4096);

	C2D_TextParse(&g_staticText, g_staticBuf, " - help");
	C2D_TextOptimize(&g_staticText);
    return true;
}
const char* scene_page_input(AppState* state, u32 down, u32 held) {
    if(down & KEY_Y)
        return scene_help_name;
    else if(down & KEY_START)
        return nullptr;
    
    return "";
}
const char* scene_page_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom) {
    // BOTTOM
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, C2D_Color32(0xff, 0xff, 0xff, 0xff)); // TODO: make color const
    C2D_SceneBegin(bottom);

    C3D_FrameEnd(0);

	// TOP
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, accent_bg);
    C2D_SceneBegin(top);

	// Draw static text strings
	C2D_DrawText(&g_staticText, C2D_AlignCenter, 200, 5, 0, .4, .4);

    C3D_FrameEnd(0);

    return "";
}
void scene_page_deinit(AppState* state) {
    // Delete the text buffers
	C2D_TextBufDelete(g_staticBuf);
}