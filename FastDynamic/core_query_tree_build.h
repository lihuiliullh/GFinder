#pragma once

#ifndef FAST_DYNAMIC_CORE_QUERY_BUILD_H_
#define FAST_DYNAMIC_CORE_QUERY_BUILD_H_

#include "cpi_builder.h"


inline void buildBFSCoreQueryTree() {

	resetTreeNodes();

	g_core_tree_node_child_array_index = 0;
	g_core_tree_node_nte_array_index = 0;

	long long iterateCoreSequence_indx = 0;

	long long * visited = visited_int_for_query;
	memset(visited, 0, sizeof(long long) * g_cnt_node_query_graph);

	long long * queue_array = queue_array_query;
	queue_array[0] = g_root_node_id_of_query;
	// visited layer by layer
	long long pointer_this_layer_start = 0;
	long long pointer_this_layer_end = 1;
	// queue end
	long long queue_end_pointer = 1;
	long long current_level = 1; //initial level starts at 1

	// root node is 0
	g_BFS_sequence_length = 0;
	level_index.clear();
	visited[g_root_node_id_of_query] = 1;
	BFS_level_query[g_root_node_id_of_query] = 1;
	BFS_parent_query[g_root_node_id_of_query] = -1;
	initializeTreeNode(core_query_tree[g_root_node_id_of_query], -1);

	while (true) {

		long long start = g_BFS_sequence_length;

		// BFS algorithm
		// just iterate one level, pointer_this_end is the end of this level
		// 0 != 1
		while (pointer_this_layer_start != pointer_this_layer_end) { // queue not empty

			long long current_node = queue_array[pointer_this_layer_start];
			pointer_this_layer_start++;

			// all neighbor of current_node, this may contains parent, children, same level nodes
			long long start = g_nodes_adj_list_start_index_query_graph[current_node];
			long long end = g_nodes_adj_list_start_index_query_graph[current_node + 1];

			// all neighbor of current_node, this may contains parent, children, same level nodes
			for (long long i = start; i < end; i++) {
				// iterate neighbor
				long long neighbor = g_nodes_adj_list_with_edge_type_query_graph[i].node_id;

				if (visited[neighbor] != 0) { //this neighbor node has been visited before,
					// visited and not parent, so it is s-NTE
					// if core childnode is 1, the condition will be false
					if (neighbor != core_query_tree[current_node].parent_node) {
						//if (BFS_level_query[neighbor] <= current_level) {
						addNonTreeEdgeToTreeNode(core_query_tree[current_node], neighbor);
					}


					//if (visited[neighbor] > current_level)	//this is a cross level nte
					//	addCrossLevelNTEToTreeNode(core_query_tree[neighbor], current_node); //record its cross level nte parent

				}
				else {//this child node has not been visited.

					visited[neighbor] = current_level + 1; //parent node's level plus one

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
			// simulation_sequence is used to store the sequence of BFS
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

		if (queue_end_pointer == pointer_this_layer_end) //no node has been pushed in
			break;

		pointer_this_layer_start = pointer_this_layer_end;
		pointer_this_layer_end = queue_end_pointer;

		current_level++;

	}
	int a = 0;
}




#endif // !FAST_DYNAMIC_CORE_QUERY_BUILD_H_


