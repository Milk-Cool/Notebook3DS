#include "draw.h"

static void draw_hollow_rect(C3D_RenderTarget* top, Shape shape) {
    if(shape.type != ShapeTypeHollowRect) return; // safety
    if(shape.points.size() != 2) return;

    C2D_DrawLine(shape.points[0].x, shape.points[0].y, shape.color, shape.points[1].x, shape.points[0].y, shape.color, shape.thickness, 0);
    C2D_DrawLine(shape.points[0].x, shape.points[0].y, shape.color, shape.points[0].x, shape.points[1].y, shape.color, shape.thickness, 0);
    C2D_DrawLine(shape.points[1].x, shape.points[0].y, shape.color, shape.points[1].x, shape.points[1].y, shape.color, shape.thickness, 0);
    C2D_DrawLine(shape.points[0].x, shape.points[1].y, shape.color, shape.points[1].x, shape.points[1].y, shape.color, shape.thickness, 0);

    C2D_DrawCircleSolid(shape.points[0].x, shape.points[0].y, 0, shape.thickness / 2, shape.color);
    C2D_DrawCircleSolid(shape.points[0].x, shape.points[1].y, 0, shape.thickness / 2, shape.color);
    C2D_DrawCircleSolid(shape.points[1].x, shape.points[0].y, 0, shape.thickness / 2, shape.color);
    C2D_DrawCircleSolid(shape.points[1].x, shape.points[1].y, 0, shape.thickness / 2, shape.color);
}
static void draw_fill_rect(C3D_RenderTarget* top, Shape shape) {
    if(shape.type != ShapeTypeFillRect) return; // safety
    if(shape.points.size() != 2) return;
    C2D_DrawRectSolid(shape.points[0].x, shape.points[0].y, 0, shape.points[1].x
        - shape.points[0].x,shape.points[1].y - shape.points[0].y, shape.color);
}
static void draw_line(C3D_RenderTarget* top, Shape shape) {
    if(shape.type != ShapeTypeLine) return;
    for(uint32_t i = 0; i < shape.points.size() - 1; i++)
        C2D_DrawLine(shape.points[i].x, shape.points[i].y, shape.color, shape.points[i + 1].x, shape.points[i + 1].y, shape.color, shape.thickness, 0);
}

void draw_shape(C3D_RenderTarget* top, Shape shape) {
    switch (shape.type)
    {
    case ShapeTypeHollowRect:
        draw_hollow_rect(top, shape);
        break;
    case ShapeTypeFillRect:
        draw_fill_rect(top, shape);
        break;
    case ShapeTypeLine:
        draw_line(top, shape);
        break;
    default:
        break;
    }
}