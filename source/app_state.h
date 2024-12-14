#pragma once
#include "backend.h"

typedef struct {
	list<Folder> folders;

	uint32_t current_folder_index;
	list<Topic> current_topics;
	uint32_t current_topic_index;
	list<Page> current_pages;
	uint32_t current_page_index;
} AppState;