#include "backend.h"

void init_backend() {
    struct stat st = { 0 };
    if(stat(PATH_ROOT, &st) == -1) {
        mkdir(PATH_ROOT, 0700);
    }
}
list<Folder> get_folders() {
    list<Folder> out;
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