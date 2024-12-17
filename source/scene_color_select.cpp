#include "scene_color_select.h"
#include "common.h"

const char* scene_color_select_name = "color_select";

static C2D_TextBuf g_staticBuf;
static C2D_Text g_staticText;

static u32 colors[] = {
    C2D_Color32(0xff, 0xff, 0xff, 0xff),
    C2D_Color32(0x00, 0x00, 0x00, 0xff),
    C2D_Color32(0xff, 0x00, 0x00, 0xff),
    C2D_Color32(0x00, 0xff, 0x00, 0xff),
    C2D_Color32(0x00, 0x00, 0xff, 0xff),
    C2D_Color32(0xff, 0xff, 0x00, 0xff),
    C2D_Color32(0xff, 0x00, 0xff, 0xff),
    C2D_Color32(0x00, 0xff, 0xff, 0xff),
    C2D_Color32(0x80, 0x80, 0x80, 0xff),
    C2D_Color32(0x80, 0x00, 0x00, 0xff),
    C2D_Color32(0x00, 0x80, 0x00, 0xff),
    C2D_Color32(0x00, 0x00, 0x80, 0xff),
    C2D_Color32(0x80, 0x80, 0x00, 0xff),
    C2D_Color32(0x80, 0x00, 0x80, 0xff),
    C2D_Color32(0x00, 0x80, 0x80, 0xff),
    C2D_Color32(0x64, 0x95, 0xed, 0xff),
    C2D_Color32(0xff, 0xbc, 0xd9, 0xff),
    C2D_Color32(0xff, 0x00, 0x00, 0x80),
    C2D_Color32(0x00, 0xff, 0x00, 0x80),
    C2D_Color32(0x00, 0x00, 0xff, 0x80),
    C2D_Color32(0xff, 0xff, 0x00, 0x80),
    C2D_Color32(0xff, 0x00, 0xff, 0x80),
    C2D_Color32(0x00, 0xff, 0xff, 0x80),
};
static u32 per_row;
static s32 current_selection;

bool scene_color_select_init(AppState* state) {
    g_staticBuf  = C2D_TextBufNew(4096);

    per_row = ceil(sqrt((float)sizeof(colors) / sizeof(colors[0])));

    for(u32 i = 0; i < sizeof(colors) / sizeof(colors[0]); i++) {
        if(colors[i] == state->dstate.current_color) {
            current_selection = i;
            break;
        }
    }

	C2D_TextParse(&g_staticText, g_staticBuf,
        "Color select\n"
        "Press  to go back"
    );
	C2D_TextOptimize(&g_staticText);
    return true;
}
const char* scene_color_select_input(AppState* state, u32 down, u32 held) {
    if(down & KEY_B || down & KEY_A) {
        state->dstate.current_color = colors[current_selection];
        return nullptr;
    }

    else if(down & KEY_DUP)
        current_selection -= per_row;
    else if(down & KEY_DDOWN)
        current_selection += per_row;
    else if(down & KEY_DLEFT)
        current_selection--;
    else if(down & KEY_DRIGHT)
        current_selection++;
    
    if(current_selection < 0)
        current_selection = 0;
    else if(current_selection >= (s32)(sizeof(colors) / sizeof(colors[0])))
        current_selection = sizeof(colors) / sizeof(colors[0]) - 1;
    
    return "";
}
const char* scene_color_select_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom) {
    // BOTTOM
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, C2D_Color32(0xff, 0xff, 0xff, 0xff));
    C2D_SceneBegin(bottom);

    for(u32 x = 0; x < 320; x += 20)
        for(u32 y = 0; y < 240; y += 20)
            if(x % 40 + y % 40 == 20) C2D_DrawRectSolid(x, y, 0, 20, 20, C2D_Color32(0x80, 0x80, 0x80, 0xff));

    u64 curtime = osGetTime();
    curtime /= 20;
    if(curtime % 20 < 10)
        curtime = curtime % 10;
    else
        curtime = 10 - (curtime % 10);
    for(u32 i = 0; i < sizeof(colors) / sizeof(colors[0]); i++) {
        C2D_DrawRectSolid(160 - (per_row * 20) + (i % per_row) * 40, 120 - (per_row * 20) + (i / per_row) * 40, 0, 40, 40, colors[i]);
        if((s32)i == current_selection) {
            C2D_DrawRectSolid(160 - (per_row * 20) + (i % per_row) * 40, 120 - (per_row * 20) + (i / per_row) * 40, 0, 40, 5, C2D_Color32(0x00, 0x00, 0xff, curtime * 25));
            C2D_DrawRectSolid(160 - (per_row * 20) + (i % per_row) * 40, 155 - (per_row * 20) + (i / per_row) * 40, 0, 40, 5, C2D_Color32(0xff, 0x00, 0x00, curtime * 25));
        }
    }

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
void scene_color_select_deinit(AppState* state) {
    // Delete the text buffers
	C2D_TextBufDelete(g_staticBuf);
}