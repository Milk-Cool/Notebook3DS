#pragma once
#include "backend.h"

typedef struct {
	vector<Folder> folders;

	uint32_t current_folder_index;
	vector<Topic> current_topics;
	uint32_t current_topic_index;
	vector<Page> current_pages;
	uint32_t current_page_index;
} AppState;