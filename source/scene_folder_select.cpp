#include "scene_folder_select.h"
#include "backend.h"
#include "draw.h"
#include "common.h"

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

	// string str = "";
	// Page test_page;

	// Shape shape1;
	// shape1.color = C2D_Color32(255, 255, 0, 255);
	// shape1.type = ShapeTypeLine;
	// shape1.thickness = 2;
	// shape1.text = "";
	// shape1.points.push_back((Point){ .x = 30, .y = 50 });
	// shape1.points.push_back((Point){ .x = 130, .y = 150 });
	// shape1.points.push_back((Point){ .x = 200, .y = 30 });
	// test_page.shapes.push_back(shape1);

	// Shape shape2;
	// shape2.color = C2D_Color32(255, 0, 255, 255);
	// shape2.type = ShapeTypeFillRect;
	// shape2.thickness = .5;
	// shape2.text = "";
	// shape2.points.push_back((Point){ .x = 70, .y = 90 });
	// shape2.points.push_back((Point){ .x = 170, .y = 190 });
	// test_page.shapes.push_back(shape2);

	// Shape shape3;
	// shape3.color = C2D_Color32(255, 0, 255, 255);
	// shape3.type = ShapeTypeHollowRect;
	// shape3.thickness = 7;
	// shape3.text = "";
	// shape3.points.push_back((Point){ .x = 90, .y = 110 });
	// shape3.points.push_back((Point){ .x = 190, .y = 210 });
	// test_page.shapes.push_back(shape3);

	// Shape shape4;
	// shape4.color = C2D_Color32(0, 255, 32, 255);
	// shape4.type = ShapeTypeText;
	// shape4.thickness = .5;
	// shape4.text = "Hello, world!";
	// shape4.points.push_back((Point){ .x = 200, .y = 100 });
	// test_page.shapes.push_back(shape4);

	// // vector<Folder> folders = get_folders();
	// // for(Folder folder : folders) {
	// // 	str += folder.name + "\n";
	// // }
	// vector<uint8_t> buf = page2bin(test_page);
	// FILE* f = fopen("/debug.bin", "w");
	// fwrite(buf.data(), 1, buf.size(), f);
	// fclose(f);
	// new_page = bin2page(buf);
	// new_page.index = 0;

	// for(uint8_t i = 0; i < new_page.shapes.size(); i++) {
	// 	Shape cur = new_page.shapes[i];
	// 	switch(cur.type) {
	// 		case ShapeTypeFillRect:
	// 			str += "fillrect ";
	// 			break;
	// 		case ShapeTypeHollowRect:
	// 			str += "hollowrect ";
	// 			break;
	// 		case ShapeTypeLine:
	// 			str += "line ";
	// 			break;
	// 		case ShapeTypeText:
	// 			str += "text ";
	// 			break;
	// 	}
	// 	str += to_string(cur.color) + " ";
	// 	str += to_string(cur.thickness) + " ";
	// 	for(uint8_t j = 0; j < cur.points.size(); j++)
	// 		str += to_string(cur.points[j].x) + " " + to_string(cur.points[j].y) + " ";
	// 	str += ";\n";
	// }

	// // Parse the static text strings
	C2D_TextParse(&g_staticText, g_staticBuf, "Select folder or press  to create a new one\nPress  to remove, or SELECT to rename");
	C2D_TextParse(&g_staticTitle, g_staticBuf, "Notebook3DS");

	// Optimize the static text strings
	C2D_TextOptimize(&g_staticText);
	C2D_TextOptimize(&g_staticTitle);
    return true;
}
const char* scene_folder_select_input(AppState* state, u32 down, u32 held) {
    if (down & KEY_START) {
        return nullptr;
	}

	if(down & KEY_DOWN) {
		current_selection++;
		if(current_selection == (s32)state->folders.size()) current_selection = 0;
	} else if(down & KEY_UP) {
		current_selection--;
		if(current_selection == -1) current_selection = state->folders.size() - 1;
	}

	if(down & KEY_X) {
		string name = get_input_name();
		if(name != "") {
			create_folder(name);
			reinit_folders(state);
		}
	} else if(down & KEY_Y) {
		remove_folder(state->folders[current_selection].id);
		reinit_folders(state);
	} else if(down & KEY_SELECT) {
		string name = get_input_name();
		if(name != "") {
			rename_folder(state->folders[current_selection].id, name);
			reinit_folders(state);
		}
	}
    return "";
}
const char* scene_folder_select_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom) {
	// BOTTOM
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, C2D_Color32(245, 162, 206, 0xFF)); // TODO: make color const
    C2D_SceneBegin(bottom);
	// Clear the dynamic text buffer
	C2D_TextBufClear(g_dynamicBuf);

	// Draw static text strings
	C2D_DrawText(&g_staticText, C2D_AlignCenter, 160, 5, 0, .55, .55);

	C2D_DrawRectSolid(5, 45, 0, 310, 60, C2D_Color32(0x00, 0x00, 0x00, 0x9F));
	C2D_DrawRectSolid(5, 110, 0, 310, 60, C2D_Color32(0x00, 0x00, 0x00, 0x9F));
	C2D_DrawRectSolid(5, 175, 0, 310, 60, C2D_Color32(0x00, 0x00, 0x00, 0x9F));

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

	// for(uint8_t i = 0; i < new_page.shapes.size(); i++) {
	// 	draw_shape(top, new_page.shapes[i]);
	// }

    C3D_FrameEnd(0);

	// TOP
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, C2D_Color32(245, 162, 206, 0xFF));
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