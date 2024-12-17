#include "scene_page.h"
#include "common.h"
#include "scene_help.h"
#include "draw.h"
#include "scene_color_select.h"
#include "scene_thickness_select.h"
#include "scene_tool_select.h"

#define DBLPRESS_MAX_DELAY 700

const char* scene_page_name = "page";

static C2D_TextBuf g_staticBuf, g_dynamicBuf;
static C2D_Text g_staticText;

static uint32_t scroll_add = 5;

bool scene_page_init(AppState* state) {
    g_staticBuf  = C2D_TextBufNew(4096);
	g_dynamicBuf = C2D_TextBufNew(4096);

	C2D_TextParse(&g_staticText, g_staticBuf, " - help");
	C2D_TextOptimize(&g_staticText);

    state->dstate.current_tool = ToolFree;
    state->dstate.current_color = C2D_Color32(0x00, 0x00, 0x00, 0xff);
    state->dstate.current_thickness = 3;
    state->dstate.last_point_time = 0;
    state->dstate.last_touched = false;
    state->dstate.scroll = 0;

    state->current_page_index = 0;
    if(state->current_pages.size() == 0)
        state->current_pages.push_back((Page){ .index = 0 });

    return true;
}
static void clear_undid(AppState* state) {
    state->current_pages[state->current_page_index].undid.clear();
}
static void handle_input(AppState* state, u32 down, u32 held, Page* current_page, touchPosition touch, u64 curtime) {
    if(state->dstate.current_tool == ToolFree
            && curtime - state->dstate.last_point_time > 10) {
        if(!state->dstate.last_touched) {
            clear_undid(state);
            current_page->shapes.push_back((Shape){
                .type = ShapeTypeLine,
                .color = state->dstate.current_color,
                .thickness = state->dstate.current_thickness
            });
        }
        uint16_t y = touch.py + (int32_t)state->dstate.scroll;
        if(y >= PAGE_HEIGHT)
            y = PAGE_HEIGHT - 1;
        current_page->shapes.back().points.push_back((Point){
            .x = touch.px,
            .y = y
        });
        state->dstate.last_point_time = curtime;
    } else if(state->dstate.current_tool == ToolText
            && !state->dstate.last_touched) {
        string text = get_input_text();
        if(text.size() > 0) {
            clear_undid(state);
            current_page->shapes.push_back((Shape){
                .type = ShapeTypeText,
                .color = state->dstate.current_color,
                .thickness = state->dstate.current_thickness / 2.5f,
                .text = text
            });
            current_page->shapes.back().points.push_back((Point){
                .x = touch.px,
                .y = touch.py + (int32_t)state->dstate.scroll
            });
        }
    }
    state->dstate.last_touched = true;
}
static void undo(AppState* state) {
    Page& page = state->current_pages[state->current_page_index];
    if(page.shapes.size() == 0)
        return;
    page.undid.push_back(
        page.shapes.back()
    );
    page.shapes.pop_back();
}
static void redo(AppState* state) {
    Page& page = state->current_pages[state->current_page_index];
    if(page.undid.size() == 0)
        return;
    page.shapes.push_back(
        page.undid.back()
    );
    page.undid.pop_back();
}
const char* scene_page_input(AppState* state, u32 down, u32 held) {
    if(down & KEY_Y)
        return scene_help_name;
    else if(down & KEY_X)
        return scene_color_select_name;
    else if(down & KEY_B)
        return scene_thickness_select_name;
    else if(down & KEY_A)
        return scene_tool_select_name;
    else if(down & KEY_START) {
        for(uint32_t i = state->current_pages.size() - 1; i >= 0; i--) {
            if(state->current_pages[i].shapes.size() > 0) {
                state->current_pages.resize(i + 1);
                break;
            }
        }
        for(Page page : state->current_pages)
            save_page(state->folders[state->current_folder_index].id, state->current_topics[state->current_topic_index].id, page);
        return nullptr;
    }

    if(held & KEY_DDOWN) {
        state->dstate.scroll += scroll_add;
        if(state->dstate.scroll > PAGE_HEIGHT)
            state->dstate.scroll = PAGE_HEIGHT;
    } else if(held & KEY_DUP) {
        state->dstate.scroll -= scroll_add;
        if(state->dstate.scroll < 0)
            state->dstate.scroll = 0;
    }
    if(down & KEY_DRIGHT) {
        state->current_page_index++;
        if(state->current_pages.size() == state->current_page_index)
            state->current_pages.push_back((Page){ .index = state->current_page_index });
    } else if(down & KEY_DLEFT) {
        if(state->current_page_index > 0)
            state->current_page_index--;
    }

    u64 curtime = osGetTime();
    if(down & KEY_L) {
        state->dstate.input_queue_l.push(curtime);
        if(state->dstate.input_queue_l.size() == 3) {
            state->dstate.input_queue_l.pop();
            if(state->dstate.input_queue_l.back() - state->dstate.input_queue_l.front() < DBLPRESS_MAX_DELAY)
                undo(state);
        }
    }
    if(down & KEY_R) {
        state->dstate.input_queue_r.push(curtime);
        if(state->dstate.input_queue_r.size() == 3) {
            state->dstate.input_queue_r.pop();
            if(state->dstate.input_queue_r.back() - state->dstate.input_queue_r.front() < DBLPRESS_MAX_DELAY)
                redo(state);
        }
    }

    touchPosition touch;
    hidTouchRead(&touch);

    Page* current_page = &state->current_pages[state->current_page_index];

    if(held & KEY_TOUCH)
        handle_input(state, down, held, current_page, touch, curtime);
    else
        state->dstate.last_touched = false;
    
    return "";
}
const char* scene_page_render(AppState* state, C3D_RenderTarget* top, C3D_RenderTarget* bottom) {
    // BOTTOM
    u32 white = C2D_Color32(0xff, 0xff, 0xff, 0xff);
    u32 black = C2D_Color32(0x00, 0x00, 0x00, 0xff);
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(bottom, accent_bg);
    C2D_SceneBegin(bottom);
    int32_t height_bottom = PAGE_HEIGHT - state->dstate.scroll;
    C2D_DrawRectSolid(0, 0, -1, 320, height_bottom < 240 ? height_bottom : 240, white);
    C2D_DrawRectSolid(0, height_bottom, -1, 320, 5, black);

    Page* current_page = &state->current_pages[state->current_page_index];
    for(Shape shape : current_page->shapes)
        draw_shape(shape, state->dstate.scroll);

    C3D_FrameEnd(0);

	// TOP
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, accent_bg);
    C2D_SceneBegin(top);
    int32_t height_top = state->dstate.scroll;
    C2D_DrawRectSolid(40, 240 - height_top, -1, 320, PAGE_HEIGHT, white);
    C2D_DrawRectSolid(40, 240 - height_top - 5, -1, 320, 5, black);
    for(Shape shape : current_page->shapes) {
        draw_shape_top(shape, state->dstate.scroll);
    }
	C2D_TextBufClear(g_dynamicBuf);
    C2D_Text dynText;
    C2D_TextParse(&dynText, g_dynamicBuf, ("Page " + to_string(state->current_page_index + 1)).c_str());
    C2D_TextOptimize(&dynText);
    C2D_DrawText(&dynText, C2D_AlignCenter, 200, 225, 0, .4, .4);
	// Draw static text strings
	C2D_DrawText(&g_staticText, C2D_AlignCenter, 200, 5, 0, .4, .4);

    C3D_FrameEnd(0);

    return "";
}
void scene_page_deinit(AppState* state) {
    // Delete the text buffers
	C2D_TextBufDelete(g_staticBuf);
    C2D_TextBufDelete(g_dynamicBuf);
}