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

vector<Topic> get_topics(string folder_id) {
    vector<Topic> out;
    char folder_path[256];
    snprintf(folder_path, 256, PATH_ROOT "%s/", folder_id.c_str());
    DIR* dir = opendir(folder_path);
    struct dirent* entry;
    if(!dir) return out;
    while((entry = readdir(dir)) != NULL) {
        if(entry->d_type != DT_DIR) continue;
        Topic topic;
        topic.id = entry->d_name;
        char path[256];
        sniprintf(path, 256, "%s%s/%s", folder_path, entry->d_name, NAMEFILE);
        FILE* file = fopen(path, "r");
        if(file == nullptr) continue;
        char name[256];
        fgets(name, 256, file);
        fclose(file);
        topic.name = strdup(name);
        out.push_back(topic);
    }
    closedir(dir);
    return out;
}

vector<Page> get_pages(string folder_id, string topic_id) {
    vector<Page> out;
    char topic_path[256];
    snprintf(topic_path, 256, PATH_ROOT "%s/%s/", folder_id.c_str(), topic_id.c_str());
    DIR* dir = opendir(topic_path);
    struct dirent* entry;
    if(!dir) return out;
    while((entry = readdir(dir)) != NULL) {
        if(entry->d_type != DT_REG) continue;
        if(!strcmp(entry->d_name, NAMEFILE)) continue;
        char path[512];
        sniprintf(path, 512, "%s%s", topic_path, entry->d_name);
        FILE* file = fopen(path, "r");
        if(file == nullptr) continue;
        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        fseek(file, 0, SEEK_SET);
        uint8_t buf[size];
        fread(buf, size, 1, file);
        fclose(file);
        Page page = bin2page(buf, size);
        page.index = stoul(entry->d_name);
        out.push_back(page);
    }
    closedir(dir);
    return out;
}

string create_folder(string folder_name) {
    DIR* dir = opendir(PATH_ROOT);
    struct dirent* entry;
    if(!dir) return "";
    uint32_t max_n = 0;
    while((entry = readdir(dir)) != NULL) {
        if(entry->d_type != DT_DIR) continue;
        unsigned long int name = stoul(entry->d_name);
        if(name > max_n) max_n = name;
    }
    string folder_id = to_string(max_n + 1);

    char folder_path[256];
    snprintf(folder_path, 256, PATH_ROOT "%s/", folder_id.c_str());
    struct stat st = { 0 };
    if(stat(folder_path, &st) != -1) return "";
    mkdir(folder_path, 0700);
    char name_path[512];
    snprintf(name_path, 512, "%s%s", folder_path, NAMEFILE);
    FILE* name = fopen(name_path, "w");
    fwrite(folder_name.c_str(), folder_name.size(), 1, name);
    fclose(name);

    return folder_id;
}

string create_topic(string folder_id, string topic_name) {
    char folder_path[256];
    snprintf(folder_path, 256, PATH_ROOT "%s/", folder_id.c_str());
    DIR* dir = opendir(folder_path);
    struct dirent* entry;
    if(!dir) return "";
    uint32_t max_n = 0;
    while((entry = readdir(dir)) != NULL) {
        if(entry->d_type != DT_DIR) continue;
        unsigned long int name = stoul(entry->d_name);
        if(name > max_n) max_n = name;
    }
    string topic_id = to_string(max_n + 1);

    char topic_path[512];
    snprintf(topic_path, 512, "%s%s/", folder_path, topic_id.c_str());
    struct stat st = { 0 };
    if(stat(topic_path, &st) != -1) return "";
    mkdir(topic_path, 0700);
    char name_path[1024];
    snprintf(name_path, 1024, "%s%s", topic_path, NAMEFILE);
    FILE* name = fopen(name_path, "w");
    fwrite(topic_name.c_str(), topic_name.size(), 1, name);
    fclose(name);
    return topic_id;
}

void save_page(string folder_id, string topic_id, Page page) {
    string page_n = to_string(page.index);
    char page_path[256];
    sniprintf(page_path, 256, PATH_ROOT "%s/%s/%s", folder_id.c_str(), topic_id.c_str(), page_n.c_str());
    FILE* file = fopen(page_path, "w");
    vector<uint8_t> bin = page2bin(page);
    fwrite(bin.data(), 1, bin.size(), file);
    fclose(file);
}

bool remove_dir_rec(string path) {
    DIR* dir = opendir(path.c_str());
    if(!dir) return false;
    struct dirent* entry;
    if(path[path.size() - 1] != '/')
        path.push_back('/');
    while((entry = readdir(dir)) != NULL) {
        if(entry->d_type == DT_DIR)
            if(!remove_dir_rec(path + entry->d_name)) return false;
        unlink((path + entry->d_name).c_str());
    }
    closedir(dir);
    rmdir(path.c_str());
    return true;
}

bool remove_folder(string folder_id) {
    return remove_dir_rec(PATH_ROOT + folder_id);
}

bool remove_topic(string folder_id, string topic_id) {
    return remove_dir_rec(PATH_ROOT + folder_id + "/" + topic_id);
}

void rename_folder(string folder_id, string new_name) {
    FILE* name = fopen((PATH_ROOT + folder_id + "/" + NAMEFILE).c_str(), "w");
    fwrite(new_name.c_str(), new_name.size(), 1, name);
    fclose(name);
}

void rename_topic(string folder_id, string topic_id, string new_name) {
    FILE* name = fopen((PATH_ROOT + folder_id + "/" + topic_id + "/" + NAMEFILE).c_str(), "w");
    fwrite(new_name.c_str(), new_name.size(), 1, name);
    fclose(name);
}

static ShapeType shape_types[] = {
    ShapeTypeFillRect,
    ShapeTypeHollowRect,
    ShapeTypeLine,
    ShapeTypeText
};

static void push_back_bytes(vector<uint8_t>* buf, size_t bytes) {
    for(size_t i = 0; i < bytes; i++)
        buf->push_back(0);
}
static void write_bytes_to_end(vector<uint8_t>* buf, uint8_t* data, size_t bytes) {
    memcpy(buf->data() + buf->size() - bytes, data, bytes);
}
static void write_shape_to_end(vector<uint8_t>& out, Shape shape) {
    uint8_t ind = UINT8_MAX;
    for(uint8_t j = 0; j < sizeof(shape_types) / sizeof(ShapeType); j++)
        if(shape_types[j] == shape.type) {
            ind = j;
            break;
        }
    if(ind == UINT8_MAX) return;
    out.push_back(ind);
    push_back_bytes(&out, sizeof(uint32_t));
    write_bytes_to_end(&out, (uint8_t*)&shape.color, sizeof(uint32_t));
    uint32_t thickness_conv = (uint32_t)(shape.thickness * 1000); // CRUTCH: simply using float crashes 3ds
    push_back_bytes(&out, sizeof(uint32_t));
    write_bytes_to_end(&out, (uint8_t*)&thickness_conv, sizeof(uint32_t));
    uint32_t points_len = shape.points.size();
    push_back_bytes(&out, sizeof(uint32_t));
    write_bytes_to_end(&out, (uint8_t*)&points_len, sizeof(uint32_t));
    for(uint32_t j = 0; j < points_len; j++) {
        push_back_bytes(&out, sizeof(uint16_t));
        write_bytes_to_end(&out, (uint8_t*)&shape.points[j].x, sizeof(uint16_t));
        push_back_bytes(&out, sizeof(uint16_t));
        write_bytes_to_end(&out, (uint8_t*)&shape.points[j].y, sizeof(uint16_t));
    }
    uint32_t text_len = shape.text.size();
    push_back_bytes(&out, text_len);
    write_bytes_to_end(&out, (uint8_t*)shape.text.c_str(), text_len);
    out.push_back(0);
}
vector<uint8_t> page2bin(Page page) {
    vector<uint8_t> out;

    uint32_t shapes_len = page.shapes.size();
    push_back_bytes(&out, sizeof(uint32_t));
    write_bytes_to_end(&out, (uint8_t*)&shapes_len, sizeof(uint32_t));
    for(uint32_t i = 0; i < shapes_len; i++) {
        write_shape_to_end(out, page.shapes[i]);
    }

    uint32_t undid_len = page.undid.size();
    push_back_bytes(&out, sizeof(uint32_t));
    write_bytes_to_end(&out, (uint8_t*)&undid_len, sizeof(uint32_t));
    for(uint32_t i = 0; i < undid_len; i++) {
        write_shape_to_end(out, page.undid[i]);
    }
    return out;
}
static Shape read_shape(vector<uint8_t>& bin, uint32_t& ind) {
    Shape shape;
    shape.type = shape_types[bin[ind]]; // u8
    ind += sizeof(uint8_t);
    shape.color = *(uint32_t*)(bin.data() + ind); // u32
    ind += sizeof(uint32_t);
    uint32_t thickness_u32;
    // CRUTCH: doesn't work without memcpy, probably because it's copied to a float in the end.
    // God, I hate ARM.
    memcpy(&thickness_u32, bin.data() + ind, sizeof(uint32_t));
    float thickness_fl = thickness_u32;
    thickness_fl /= 1000.0f;
    shape.thickness = thickness_fl;
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
    while(true) {
        char c = bin[ind++];
        if(c == '\0') break;
        shape.text.push_back(c);
    }
    return shape;
}
Page bin2page(vector<uint8_t> bin) {
    Page out;
    out.index = 0xffffffff;
    uint32_t shapes_len = *(uint32_t*)bin.data();
    uint32_t ind = sizeof(uint32_t);
    for(uint32_t i = 0; i < shapes_len; i++) {
        out.shapes.push_back(read_shape(bin, ind));
    }
    uint32_t undid_len = *(uint32_t*)(bin.data() + ind);
    ind += sizeof(uint32_t);
    for(uint32_t i = 0; i < undid_len; i++) {
        out.undid.push_back(read_shape(bin, ind));
    }
    return out;
}
Page bin2page(uint8_t* bin, size_t size) {
    return bin2page(vector<uint8_t>(bin, bin + size));
}