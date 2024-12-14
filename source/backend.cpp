#include "backend.h"

void init_backend() {
    struct stat st = { 0 };
    if(stat(PATH_ROOT, &st) == -1) {
        mkdir(PATH_ROOT, 0700);
    }
}
vector<Folder> get_folders() {
    vector<Folder> out;
    DIR* dir = opendir(PATH_ROOT);
    struct dirent* entry;
    if(!dir) return out;
    while((entry = readdir(dir)) != NULL) {
        if(entry->d_type != DT_DIR) continue;
        Folder folder;
        folder.id = entry->d_name;
        char path[256];
        sniprintf(path, 256, "%s%s/%s", PATH_ROOT, entry->d_name, NAMEFILE);
        FILE* file = fopen(path, "r");
        if(file == nullptr) continue;
        char name[256];
        fgets(name, 256, file);
        fclose(file);
        folder.name = strdup(name);
        out.push_back(folder);
    }
    closedir(dir);
    return out;
}

static ShapeType shape_types[] = {
    ShapeTypeFillRect,
    ShapeTypeHollowRect,
    ShapeTypeLine
};

static void push_back_bytes(vector<uint8_t>* buf, size_t bytes) {
    for(size_t i = 0; i < bytes; i++)
        buf->push_back(0);
}
static void write_bytes_to_end(vector<uint8_t>* buf, uint8_t* data, size_t bytes) {
    memcpy(buf->data() + buf->size() - bytes, data, bytes);
}
vector<uint8_t> page2bin(Page page) {
    vector<uint8_t> out;
    push_back_bytes(&out, sizeof(uint32_t));
    uint32_t shapes_len = page.shapes.size();
    memcpy(out.data(), (uint8_t*)&shapes_len, sizeof(uint32_t));
    for(uint32_t i = 0; i < shapes_len; i++) {
        uint8_t ind = UINT8_MAX;
        for(uint8_t j = 0; j < sizeof(shape_types) / sizeof(ShapeType); j++)
            if(shape_types[j] == page.shapes[i].type) {
                ind = j;
                break;
            }
        if(ind == UINT8_MAX) continue;
        out.push_back(ind);
        push_back_bytes(&out, sizeof(uint32_t));
        write_bytes_to_end(&out, (uint8_t*)&page.shapes[i].color, sizeof(uint32_t));
        uint32_t thickness_conv = (uint32_t)(page.shapes[i].thickness * 1000); // CRUTCH: simply using float crashes 3ds
        push_back_bytes(&out, sizeof(uint32_t));
        write_bytes_to_end(&out, (uint8_t*)&thickness_conv, sizeof(uint32_t));
        uint32_t points_len = page.shapes[i].points.size();
        push_back_bytes(&out, sizeof(uint32_t));
        write_bytes_to_end(&out, (uint8_t*)&points_len, sizeof(uint32_t));
        for(uint32_t j = 0; j < points_len; j++) {
            push_back_bytes(&out, sizeof(uint16_t));
            write_bytes_to_end(&out, (uint8_t*)&page.shapes[i].points[j].x, sizeof(uint16_t));
            push_back_bytes(&out, sizeof(uint16_t));
            write_bytes_to_end(&out, (uint8_t*)&page.shapes[i].points[j].y, sizeof(uint16_t));
        }
    }
    return out;
}
Page bin2page(vector<uint8_t> bin) {
    Page out;
    out.index = 0xffffffff;
    uint32_t shapes_len = *(uint32_t*)bin.data();
    uint32_t ind = sizeof(uint32_t);
    for(uint32_t i = 0; i < shapes_len; i++) {
        Shape shape;
        shape.type = shape_types[bin[ind]]; // u8
        ind += sizeof(uint8_t);
        shape.color = *(uint32_t*)(bin.data() + ind); // u32
        ind += sizeof(uint32_t);
        shape.thickness = *(uint32_t*)(bin.data() + ind); // u32
        shape.thickness /= 1000;
        ind += sizeof(uint32_t);
        uint32_t points_len = *(uint32_t*)(bin.data() + ind);
        ind += sizeof(uint32_t);
        for(uint32_t j = 0; j < points_len; j++) {
            Point point;
            point.x = *(uint16_t*)(bin.data() + ind);
            ind += sizeof(uint16_t);
            point.y = *(uint16_t*)(bin.data() + ind);
            ind += sizeof(uint16_t);
            shape.points.push_back(point);
        }
        out.shapes.push_back(shape);
    }
    return out;
}
Page bin2page(uint8_t* bin, size_t size) {
    return bin2page(vector<uint8_t>(bin, bin + size));
}