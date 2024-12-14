#include "scene_folder_select.h"
#include "backend.h"
#include "draw.h"

const char* scene_folder_select_name = "folder_select";

static float textSize = 0.5f;

static C2D_TextBuf g_staticBuf, g_dynamicBuf;
static C2D_Text g_staticText;

static Page new_page;

bool scene_folder_select_init(AppState* state) {
	// Create two text buffers: one for static text, and another one for
	// dynamic text - the latter will be cleared at each frame.
	g_staticBuf  = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
	g_dynamicBuf = C2D_TextBufNew(4096);

	state->folders = get_folders();

	string str = "";
	Page test_page;

	Shape shape1;
	shape1.color = C2D_Color32(255, 255, 0, 255);
	shape1.type = ShapeTypeLine;
	shape1.thickness = 2;
	shape1.text = "";
	shape1.points.push_back((Point){ .x = 30, .y = 50 });
	shape1.points.push_back((Point){ .x = 130, .y = 150 });
	shape1.points.push_back((Point){ .x = 200, .y = 30 });
	test_page.shapes.push_back(shape1);

	Shape shape2;
	shape2.color = C2D_Color32(255, 0, 255, 255);
	shape2.type = ShapeTypeFillRect;
	shape2.thickness = .5;
	shape2.text = "";
	shape2.points.push_back((Point){ .x = 70, .y = 90 });
	shape2.points.push_back((Point){ .x = 170, .y = 190 });
	test_page.shapes.push_back(shape2);

	Shape shape3;
	shape3.color = C2D_Color32(255, 0, 255, 255);
	shape3.type = ShapeTypeHollowRect;
	shape3.thickness = 7;
	shape3.text = "";
	shape3.points.push_back((Point){ .x = 90, .y = 110 });
	shape3.points.push_back((Point){ .x = 190, .y = 210 });
	test_page.shapes.push_back(shape3);

	Shape shape4;
	shape4.color = C2D_Color32(0, 255, 32, 255);
	shape4.type = ShapeTypeText;
	shape4.thickness = .5;
	shape4.text = "Hello, world!";
	shape4.points.push_back((Point){ .x = 200, .y = 100 });
	test_page.shapes.push_back(shape4);

	// vector<Folder> folders = get_folders();
	// for(Folder folder : folders) {
	// 	str += folder.name + "\n";
	// }
	vector<uint8_t> buf = page2bin(test_page);
	FILE* f = fopen("/debug.bin", "w");
	fwrite(buf.data(), 1, buf.size(), f);
	fclose(f);
	new_page = bin2page(buf);
	new_page.index = 0;

	for(uint8_t i = 0; i < new_page.shapes.size(); i++) {
		Shape cur = new_page.shapes[i];
		switch(cur.type) {
			case ShapeTypeFillRect:
				str += "fillrect ";
				break;
			case ShapeTypeHollowRect:
				str += "hollowrect ";
				break;
			case ShapeTypeLine:
				str += "line ";
				break;
			case ShapeTypeText:
				str += "text ";
				break;
		}
		str += to_string(cur.color) + " ";
		str += to_string(cur.thickness) + " ";
		for(uint8_t j = 0; j < cur.points.size(); j++)
			str += to_string(cur.points[j].x) + " " + to_string(cur.points[j].y) + " ";
		str += ";\n";
	}

	// Parse the static text strings
	C2D_TextParse(&g_staticText, g_staticBuf, str.c_str());

	// Optimize the static text strings
	C2D_TextOptimize(&g_staticText);
    return true;
}
void save_as_test() {
	string folder_id = create_folder("TestFolder01");
	if(folder_id == "") return;
	string topic_id = create_topic(folder_id, "TestTopic");
	if(topic_id == "") return;
	save_page(folder_id, topic_id, new_page);
}
const char* scene_folder_select_input(AppState* state, u32 down, u32 held) {
    if (down & KEY_START) {
        return nullptr;
	}

	if(down & KEY_DUP) {
		save_as_test();
	}

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
	C2D_DrawText(&g_staticText, 0, 8.0f, 8.0f, 0, textSize, textSize);

	// Generate and draw dynamic text
	char buf[160];
	C2D_Text dynText;
	snprintf(buf, sizeof(buf), "Current text size: %f (Use  to change)", textSize);
	C2D_TextParse(&dynText, g_dynamicBuf, buf);
	C2D_TextOptimize(&dynText);
	C2D_DrawText(&dynText, C2D_AlignCenter, 200.0f, 220.0f, 0, 0.5f, 0.5f);

	for(uint8_t i = 0; i < new_page.shapes.size(); i++) {
		draw_shape(top, new_page.shapes[i]);
	}

    C3D_FrameEnd(0);

    return "";
}
void scene_folder_select_deinit(AppState* state) {
	// Delete the text buffers
	C2D_TextBufDelete(g_dynamicBuf);
	C2D_TextBufDelete(g_staticBuf);
}