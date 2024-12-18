#include "scene_help.h"
#include "common.h"

const char* scene_help_name = "help";

static C2D_TextBuf g_staticBuf;
static C2D_Text g_staticText;

bool scene_help_init(AppState* state) {
    g_staticBuf  = C2D_TextBufNew(4096);
	// g_dynamicBuf = C2D_TextBufNew(4096);

	C2D_TextParse(&g_staticText, g_staticBuf,
        "Notebook3DS help\n"
        " - change tool\n"
        " - change thickness\n"
        " - change color\n"
        " - help\n"
        "2x - undo\n"
        "2x - redo\n"
        " - change pages/scroll\n"
        "START - quit\n"
        "SELECT - danger zone\n"
        "\n"
        "Press  to go back"
    );
	C2D_TextOptimize(&g_staticText);
    return true;
}
const char* scene_help_input(AppState* state, u32 down, u32 held) {
    if(down & KEY_B)
        return nullptr;
    
    return "";
}
const char* scene_help_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom) {
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
	C2D_DrawText(&g_staticText, C2D_AlignCenter, 200, 15, 0, .55, .55);

    C3D_FrameEnd(0);

    return "";
}
void scene_help_deinit(AppState* state) {
    // Delete the text buffers
	C2D_TextBufDelete(g_staticBuf);
}