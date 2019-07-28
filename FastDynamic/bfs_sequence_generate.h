#pragma once

#include "cpi_builder.h"

extern vector < pair<long long, long long> > level_index;
extern long long BFS_level_query[MAX_QUERY_NODE];
extern long long BFS_parent_query[MAX_QUERY_NODE];
extern long long BFS_sequence_query_of_all[MAX_QUERY_NODE];
extern long long iterateCoreSequence[MAX_QUERY_NODE];

inline void bfsSequenceGenerate() {

	long long iterateCoreSequence_indx = 0;

	resetTreeNodes();

	g_core_tree_node_child_array_index = 0;
	g_core_tree_node_nte_array_index = 0;

	long long interateCoreSequence_index = 0;

	long long * visited = visited_int_for_query;
	memset(visited, 0, sizeof(long long) * g_cnt_node_query_graph);

	long long * queue_array = queue_array_query;

	queue_array[0] = g_root_node_id_of_query;

	long long pointer_this_layer_start = 0;
	long long pointer_this_layer_end = 1;
	// queue end
	long long queue_end_pointer = 1;
	long long current_level = 1;

	g_BFS_sequence_length = 0;
	level_index.clear();
	visited[g_root_node_id_of_query] = 1;
	BFS_level_query[g_root_node_id_of_query] = 1;
	BFS_parent_query[g_root_node_id_of_query] = -1;
	initializeTreeNode(core_query_tree[g_root_node_id_of_query], -1);

	// generate BFS sequence
	while (true) {
		long long start = g_BFS_sequence_length;

		while (pointer_this_layer_start != pointer_this_layer_end) {
			long long current_node = queue_array[pointer_this_layer_start];
			pointer_this_layer_start++;

			long long start = g_nodes_adj_list_start_index_query_graph[current_node];
			long long end = g_nodes_adj_list_start_index_query_graph[current_node + 1];

			for (long long i = start; i < end; i++) {
				long long neighbor = g_nodes_adj_list_query_graph[i];
				if (visited[neighbor] != 0) {
					if (neighbor != core_query_tree[current_node].parent_node) {
						addNonTreeEdgeToTreeNode(core_query_tree[current_node], neighbor);
					}
				}
				else {
					visited[neighbor] = current_level + 1;
					queue_array[queue_end_pointer] = neighbor;
					queue_end_pointer++;

					BFS_level_query[neighbor] = current_level + 1;
					BFS_parent_query[neighbor] = current_node;

					if (g_core_number_query_graph[neighbor] < 2)
						continue;

					initializeTreeNode(core_query_tree[neighbor], current_node);
					addChildToTreeNode(core_query_tree[current_node], neighbor);
				}
			}

			BFS_sequence_query_of_all[g_BFS_sequence_length] = current_node;
			g_BFS_sequence_length++;
		}
		long long end = g_BFS_sequence_length;
		level_index.push_back(make_pair(start, end));

		for (long long i = end - 1; i >= start; i--) {
			long long node = BFS_sequence_query_of_all[i];
			if (g_core_number_query_graph[node] < 2)
				continue;
			iterateCoreSequence[iterateCoreSequence_indx++] = node;
		}

		if (queue_end_pointer == pointer_this_layer_end) {
			break; // no node anymore
		}

		pointer_this_layer_start = pointer_this_layer_end;
		pointer_this_layer_end = queue_end_pointer;
		current_level++;

	}
}
