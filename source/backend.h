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
    int32_t x;
    int32_t y;
} Point;
typedef enum {
    ShapeTypeFillRect,
    ShapeTypeHollowRect,
    ShapeTypeLine,
    ShapeTypeText
} ShapeType;
typedef struct {
    ShapeType type;
    uint32_t color;
    float thickness;
    vector<Point> points;
    string text;
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
vector<Topic> get_topics(string folder_id);
vector<Page> get_pages(string folder_id, string topic_id);

string create_folder(string folder_name);
string create_topic(string folder_id, string topic_name);
void save_page(string folder_id, string topic_id, Page page);

bool remove_folder(string folder_id);
bool remove_topic(string folder_id, string topic_id);
void rename_folder(string folder_id, string new_name);
void rename_topic(string folder_id, string topic_id, string new_name);

vector<uint8_t> page2bin(Page page); // does not initialize the index
Page bin2page(vector<uint8_t> bin);
Page bin2page(uint8_t* bin, size_t size);