#pragma once
#include <stdint.h>
#include <queue>

typedef enum {
    ToolFree,
    ToolLine,
    ToolFillRect,
    ToolHollowRect,
    ToolText,
    ToolEraser,
} Tool;

typedef struct {
    Tool current_tool;
    uint32_t current_color;
    float current_thickness;

    uint64_t last_point_time;
    bool last_touched;
    int32_t scroll;
    queue<uint64_t> input_queue_l;
    queue<uint64_t> input_queue_r;
    queue<uint64_t> input_queue_a;

    bool exit;
    bool touch_in_another_scene;
} DrawingState;