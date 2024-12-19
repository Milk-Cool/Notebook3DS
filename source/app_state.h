#pragma once
#include "backend.h"
#include "drawing_state.h"

typedef struct {
	SortType sort_type;
	SortDirection sort_direction;

	vector<Folder> folders;

	uint32_t current_folder_index;
	vector<Topic> current_topics;
	uint32_t current_topic_index;
	vector<Page> current_pages;
	uint32_t current_page_index;

	bool needs_reinit;

	DrawingState dstate;
} AppState;