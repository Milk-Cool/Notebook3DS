#include "scene_tool_select.h"
#include "common.h"

const char* scene_tool_select_name = "tool_select";

static C2D_TextBuf g_staticBuf, g_dynamicBuf;
static C2D_Text g_staticText;

typedef struct {
    Tool tool;
    C2D_Sprite* sprite;
    string name;
} ToolWithSprite;
static C2D_SpriteSheet spriteSheet;
static C2D_Sprite sprites[5];
static ToolWithSprite tools[] = {
    { .tool = ToolFree, .name = "Free" },
    { .tool = ToolLine, .name = "Line" },
    { .tool = ToolFillRect, .name = "Filled rect" },
    { .tool = ToolHollowRect, .name = "Hollow rect" },
    { .tool = ToolText, .name = "Text" }
};
static s32 current_selection = 0;

bool scene_tool_select_init(AppState* state) {
    g_staticBuf = C2D_TextBufNew(4096);
    g_dynamicBuf = C2D_TextBufNew(4096);

	C2D_TextParse(&g_staticText, g_staticBuf,
        "Tool select\n"
        "Press  to go back"
    );
	C2D_TextOptimize(&g_staticText);

    spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/tools.t3x");
    if(!spriteSheet) svcBreak(USERBREAK_PANIC);

    for(u16 i = 0; i < sizeof(sprites) / sizeof(sprites[0]); i++) {
        C2D_SpriteFromSheet(&sprites[i], spriteSheet, i);
        C2D_SpriteSetCenter(&sprites[i], 0.5f, 0.5f);
        C2D_SpriteSetPos(&sprites[i], 60 + i * 50, 120);
        tools[i].sprite = &sprites[i];
    }

    for(u32 i = 0; i < sizeof(tools) / sizeof(tools[0]); i++) {
        if(tools[i].tool == state->dstate.current_tool) {
            current_selection = i;
            break;
        }
    }

    return true;
}
const char* scene_tool_select_input(AppState* state, u32 down, u32 held) {
    if(down & KEY_B || down & KEY_A) {
        state->dstate.current_tool = tools[current_selection].tool;
        return nullptr;
    }

    else if(down & KEY_DLEFT)
        current_selection--;
    else if(down & KEY_DRIGHT)
        current_selection++;
    
    if(down & KEY_TOUCH) {
        state->dstate.touch_in_another_scene = true;
        touchPosition touch;
        hidTouchRead(&touch);
        if(touch.py >= 120 && touch.py < 170) {
            s32 x = (touch.px - 60 + 25) / 50;
            if(x >= 0 && (u32)x < sizeof(tools) / sizeof(tools[0])) {
                s32 new_selection = x;
                if(new_selection == current_selection) {
                    state->dstate.current_tool = tools[current_selection].tool;
                    return nullptr;
                }
                current_selection = new_selection;
            }
        }
    } else
        state->dstate.touch_in_another_scene = false;

    if(current_selection < 0)
        current_selection = 0;
    else if((u32)current_selection >= sizeof(tools) / sizeof(tools[0]))
        current_selection = sizeof(tools) / sizeof(tools[0]) - 1;
    
    return "";
}
const char* scene_tool_select_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom) {
    // BOTTOM
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, accent_bg);
    C2D_SceneBegin(bottom);

    for(u32 i = 0; i < sizeof(tools) / sizeof(tools[0]); i++) {
        C2D_DrawSprite(tools[i].sprite);
    }

    C2D_Text dynText;
    C2D_TextBufClear(g_dynamicBuf);
    C2D_TextParse(&dynText, g_dynamicBuf, tools[current_selection].name.c_str());
    C2D_TextOptimize(&dynText);
    C2D_DrawText(&dynText, C2D_AlignCenter, 160, 50, 0, 0.5f, 0.5f);

    u64 curtime = osGetTime();
    curtime /= 20;
    if(curtime % 20 < 10)
        curtime = curtime % 10;
    else
        curtime = 10 - (curtime % 10);
    C2D_DrawRectSolid(35 + current_selection * 50, 140, 0, 50, 5, C2D_Color32(0xff, 0x00, 0x00, curtime * 25));

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
void scene_tool_select_deinit(AppState* state) {
    C2D_SpriteSheetFree(spriteSheet);
    // Delete the text buffers
	C2D_TextBufDelete(g_staticBuf);
	C2D_TextBufDelete(g_dynamicBuf);
}