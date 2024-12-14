#include "scene_folder_select.h"
#include "backend.h"

const char* scene_folder_select_name = "folder_select";

static float textSize = 0.5f;

static C2D_TextBuf g_staticBuf, g_dynamicBuf;
static C2D_Text g_staticText[4];

bool scene_folder_select_init(AppState* state) {
	// Create two text buffers: one for static text, and another one for
	// dynamic text - the latter will be cleared at each frame.
	g_staticBuf  = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
	g_dynamicBuf = C2D_TextBufNew(4096);

	string str = "";
	list<Folder> folders = get_folders();
	for(Folder folder : folders) {
		str += folder.name + "\n";
	}

	// Parse the static text strings
	C2D_TextParse(&g_staticText[0], g_staticBuf, str.c_str());
	C2D_TextParse(&g_staticText[1], g_staticBuf, "I am red skinny text!");
	C2D_TextParse(&g_staticText[2], g_staticBuf, "I am blue fat text!");
	C2D_TextParse(&g_staticText[3], g_staticBuf, "I am justified text!");

	// Optimize the static text strings
	C2D_TextOptimize(&g_staticText[0]);
	C2D_TextOptimize(&g_staticText[1]);
	C2D_TextOptimize(&g_staticText[2]);
	C2D_TextOptimize(&g_staticText[3]);
    return true;
}
const char* scene_folder_select_input(AppState* state, u32 down, u32 held) {
    if (down & KEY_START)
        return nullptr;

    if (held & KEY_L)
        textSize -= 1.0f/128;
    else if (held & KEY_R)
        textSize += 1.0f/128;
    else if (held & KEY_X)
        textSize = 0.5f;
    else if (held & KEY_Y)
        textSize = 1.0f;

    if (textSize < 0.25f)
        textSize = 0.25f;
    else if (textSize > 2.0f)
        textSize = 2.0f;
    return "";
}
const char* scene_folder_select_render(AppState* state, C3D_RenderTarget* top) {
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, C2D_Color32(0x68, 0xB0, 0xD8, 0xFF));
    C2D_SceneBegin(top);
	// Clear the dynamic text buffer
	C2D_TextBufClear(g_dynamicBuf);

	// Draw static text strings
	C2D_DrawText(&g_staticText[0], 0, 8.0f, 8.0f, 0.5f, textSize, textSize);
	C2D_DrawText(&g_staticText[1], C2D_AtBaseline | C2D_WithColor, 16.0f, 210.0f, 0.5f, 0.5f, 0.75f, C2D_Color32f(1.0f,0.0f,0.0f,1.0f));
	C2D_DrawText(&g_staticText[2], C2D_AtBaseline | C2D_WithColor | C2D_AlignRight, 384.0f, 210.0f, 0.5f, 0.75f, 0.5f, C2D_Color32f(0.0f,0.0f,1.0f,0.625f));
	C2D_DrawText(&g_staticText[3], C2D_AtBaseline | C2D_AlignJustified | C2D_WordWrap, 100.0f, 170.0f, 0.5f, 0.75f, 0.75f, 200.0f);

	// Generate and draw dynamic text
	char buf[160];
	C2D_Text dynText;
	snprintf(buf, sizeof(buf), "Current text size: %f (Use  to change)", textSize);
	C2D_TextParse(&dynText, g_dynamicBuf, buf);
	C2D_TextOptimize(&dynText);
	C2D_DrawText(&dynText, C2D_AlignCenter, 200.0f, 220.0f, 0.5f, 0.5f, 0.5f);
    C3D_FrameEnd(0);
    return "";
}
void scene_folder_select_deinit(AppState* state) {
	// Delete the text buffers
	C2D_TextBufDelete(g_dynamicBuf);
	C2D_TextBufDelete(g_staticBuf);
}