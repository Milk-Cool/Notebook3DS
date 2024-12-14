#pragma once
#include <list>
#include <string>
using namespace std;

typedef struct {
	string id;
	string name;
} Folder;
typedef struct {
    uint16_t x;
    uint16_t y;
} Point;
typedef enum {
    ShapeTypeFillRect,
    ShapeTypeHollowRect,
    ShapeTypeLine
} ShapeType;
typedef struct {
    ShapeType type;
    uint32_t color;
    list<Point> points;
} Shape;
typedef struct {
    uint32_t index;
    list<Shape> shapes;
} Page;
typedef struct {
	string id;
	string name;
} Topic;