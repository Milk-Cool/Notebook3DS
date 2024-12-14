#pragma once
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/dirent.h>
#include <vector>
#include <list>
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
    vector<Point> points;
} Shape;
typedef struct {
    uint32_t index;
    vector<Shape> shapes;
} Page;
typedef struct {
	string id;
	string name;
} Topic;

#define PATH_ROOT "/notebook/"
#define NAMEFILE ".notebook"

void init_backend();
vector<Folder> get_folders();

vector<uint8_t> page2bin(Page page); // does not initialize the index
Page bin2page(vector<uint8_t> bin);
Page bin2page(uint8_t* bin, size_t size);