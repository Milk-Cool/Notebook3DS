#pragma once
#include <list>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/dirent.h>
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

#define PATH_ROOT "/notebook/"
#define NAMEFILE ".notebook"

void init_backend();
list<Folder> get_folders();