#pragma once

#ifndef FAST_DYNAMIC_CORE_QUERY_DECOMPOSITION_H_
#define FAST_DYNAMIC_CORE_QUERY_DECOMPOSITION_H_

#include "global_variable.h"
#include <algorithm>

using namespace std;

inline bool sort_by_NEC_label(NEC_element p1, NEC_element p2) {
	return (p1.label < p2.label);
}

inline void coreDecompositionQuery() {

	//begin starting the core-decomposition, core number is the degree number
	long long bin_query[MAX_QUERY_NODE];
	long long pos_query[MAX_QUERY_NODE];
	long long vert_query[MAX_QUERY_NODE];
	long long * bin = bin_query;	//	int bin [MAX_DEGREE_QUERY + 1];
	long long * pos = pos_query;	//	int pos [cnt_node_query];
	long long * vert = vert_query;	//	int vert [cnt_node_query];

	memset(bin, 0, sizeof(long long) * (MAX_DEGREE_A_NODE_QUERY + 1));

	for (long long i = 0; i < g_cnt_node_query_graph; i++)
		bin[g_core_number_query_graph[i]] ++;

	long long start = 0;
	long long num;

	for (long long d = 0; d <= MAX_DEGREE_A_NODE_QUERY; d++) {
		num = bin[d];
		bin[d] = start;
		start += num;
	}


	for (long long i = 0; i < g_cnt_node_query_graph; i++) {
		pos[i] = bin[g_core_number_query_graph[i]];
		vert[pos[i]] = i;
		bin[g_core_number_query_graph[i]] ++;
	}

	for (long long d = MAX_DEGREE_A_NODE_QUERY; d > 0; d--)
		bin[d] = bin[d - 1];
	bin[0] = 0;

	for (long long i = 0; i < g_cnt_node_query_graph; i++) {

		long long v = vert[i];

		for (long long j = g_nodes_adj_list_start_index_query_graph[v]; j < g_nodes_adj_list_start_index_query_graph[v + 1]; j++) {

			long long u = g_nodes_adj_list_with_edge_type_query_graph[j].node_id; // nodes_query[v][j];

			if (g_core_number_query_graph[u] > g_core_number_query_graph[v]) {

				long long du = g_core_number_query_graph[u];
				long long pu = pos[u];

				long long pw = bin[du];
				long long w = vert[pw];

				if (u != w) {	//if not the same node, switch the position of the two nodes.
					pos[u] = pw;
					pos[w] = pu;
					vert[pu] = w;
					vert[pw] = u;
				}

				bin[du] ++;
				g_core_number_query_graph[u]--;
			}
		}
	}

}

inline void extractResidualStructures() {
	
	memset(NEC_map, -1, sizeof(long long) * g_cnt_node_query_graph);

	char * visited = g_visited_for_query_graph;
	memset(visited, 0, sizeof(char) * g_cnt_node_query_graph);

	
	g_residual_tree_match_seq_index = 0;
	g_residual_tree_leaf_node_index = 0;
	NEC_leaf_mapping_pair_index = 0;

	for (long long core_node_i = 0; core_node_i < g_cnt_node_query_graph; core_node_i++) {
		if (g_core_number_query_graph[core_node_i] < 2) {
			continue;
		}

		for (long long j = g_nodes_adj_list_start_index_query_graph[core_node_i]; j < g_nodes_adj_list_start_index_query_graph[core_node_i + 1]; j++) {
			long long neighbor = g_nodes_adj_list_with_edge_type_query_graph[j].node_id;
			if (g_core_number_query_graph[neighbor] < 2) {
				// two cases here, the NEC node or a residual tree
				if (g_node_degree_query_graph[neighbor] == 1) {
					long long label = g_nodes_label_query_graph[neighbor];
					// node i first meet this label
					if (NEC_mapping_child_with_same_label_cnt[label * MAX_QUERY_NODE + core_node_i] == 0) {
						NEC_mapping_pair[NEC_leaf_mapping_pair_index++] = NEC_element(label, core_node_i, neighbor);
						NEC_map[neighbor] = neighbor;//NEC map
						NEC_mapping_Actual_first[label * MAX_QUERY_NODE + core_node_i] = neighbor;

#ifdef RESULT_ENUMERATION
						NEC_Node_array[neighbor].node = neighbor;
						NEC_Node_array[neighbor].nextAddress = NULL;
#endif
					}
					else {
						NEC_map[neighbor] = NEC_mapping_Actual_first[label * MAX_QUERY_NODE + core_node_i];//NEC map

#ifdef RESULT_ENUMERATION
						long long rep = NEC_mapping_Actual_first[label * MAX_QUERY_NODE + core_node_i];
						NEC_Node_array[neighbor].node = neighbor;
						NEC_Node_array[neighbor].nextAddress = NEC_Node_array[rep].nextAddress;
						NEC_Node_array[rep].nextAddress = &NEC_Node_array[neighbor];
#endif
					}
					NEC_mapping_child_with_same_label_cnt[label * MAX_QUERY_NODE + core_node_i] ++; // the label with parent being i, nec_count ++
				}
				else {
					// forest
					long long * dfs_stack = dfs_stack_query;
					long long dfs_stack_index = 0;

					visited[core_node_i] = 1; // this is the start node's parent node (a marked node)
					visited[neighbor] = 1; // this is the start node
					dfs_stack[dfs_stack_index++] = neighbor;
					g_residual_tree_match_seq_query[g_residual_tree_match_seq_index++] = neighbor;
					g_tree_node_parent_query_graph[neighbor] = core_node_i;

					while (dfs_stack_index != 0) {
						long long current_node = dfs_stack[dfs_stack_index - 1];
						dfs_stack_index--;

						long long added_child = 0;

						for (long long m = g_nodes_adj_list_start_index_query_graph[current_node]; m < g_nodes_adj_list_start_index_query_graph[current_node + 1]; m++) {

							long long child_node = g_nodes_adj_list_with_edge_type_query_graph[m].node_id;

							if (!visited[child_node]) {

								visited[child_node] = 1;

								//======== special treatment here: if a node is a leaf (degree being 1), then put it into nec node set
								if (g_node_degree_query_graph[child_node] == 1) {

									long long label = g_nodes_label_query_graph[child_node];

									if (NEC_mapping_child_with_same_label_cnt[label * MAX_QUERY_NODE + current_node] == 0) {
										NEC_mapping_pair[NEC_leaf_mapping_pair_index++] = NEC_element(label, current_node, child_node);// child is the repesentive node
										NEC_map[child_node] = child_node;//NEC map
										NEC_mapping_Actual_first[label * MAX_QUERY_NODE + current_node] = child_node;
#ifdef RESULT_ENUMERATION
										NEC_Node_array[child_node].node = child_node;
										NEC_Node_array[child_node].nextAddress = NULL;
#endif
									}
									else {
										NEC_map[child_node] = NEC_mapping_Actual_first[label * MAX_QUERY_NODE + current_node];//NEC map
#ifdef RESULT_ENUMERATION
										long long rep = NEC_mapping_Actual_first[label * MAX_QUERY_NODE + current_node];
										NEC_Node_array[child_node].node = child_node;
										NEC_Node_array[child_node].nextAddress = NEC_Node_array[rep].nextAddress;
										NEC_Node_array[rep].nextAddress = &NEC_Node_array[child_node];
#endif
									}
									NEC_mapping_child_with_same_label_cnt[label * MAX_QUERY_NODE + current_node]++; // the label with parent being i, nec_count ++
									continue;
								}
								//===========================================================
								g_tree_node_parent_query_graph[child_node] = current_node;
								added_child++;
								dfs_stack[dfs_stack_index++] = child_node;
								g_residual_tree_match_seq_query[g_residual_tree_match_seq_index++] = child_node;
							}

							if (added_child == 0)//this information is recorded for extracting the matching sequence for the tree matching sequence.
								g_residual_tree_leaf_node[g_residual_tree_leaf_node_index++] = make_pair(current_node, 0);
						}
					}
				}
			}
		}
	}

	sort(NEC_mapping_pair, NEC_mapping_pair + NEC_leaf_mapping_pair_index, sort_by_NEC_label);
	long long last_label;
	NEC_set_index = 0;
	NEC_set_by_label_index.clear();
	long long sum;

	// only one NEC Node
	if (NEC_leaf_mapping_pair_index == 1) {
		NEC_element & nec_ele = NEC_mapping_pair[0];
		long long label = nec_ele.label;
		long long parent_id = nec_ele.parent_id;
		long long represent_child = nec_ele.represent_node;
		sum = NEC_mapping_child_with_same_label_cnt[label * MAX_QUERY_NODE + parent_id];
		NEC_mapping_child_with_same_label_cnt[label * MAX_QUERY_NODE + parent_id] = 0;
		NEC_set_by_label_index.push_back(make_pair(label, NEC_set_index));
		NEC_set_array[NEC_set_index++] = NEC_set_array_element(parent_id, represent_child, sum);
		NEC_set_by_label_index.push_back(make_pair(-1, NEC_leaf_mapping_pair_index)); // redundant element to set the end
	}
	else {
		for (long long i = 0; i < NEC_leaf_mapping_pair_index; i++) {
			NEC_element & nec_ele = NEC_mapping_pair[i];
			long long label = nec_ele.label;
			long long parent_id = nec_ele.parent_id;
			long long represent_child = nec_ele.represent_node;
			sum = NEC_mapping_child_with_same_label_cnt[label * MAX_QUERY_NODE + parent_id];
			NEC_mapping_child_with_same_label_cnt[label * MAX_QUERY_NODE + parent_id] = 0; //reset it back to 0

			if (i == 0) {
				// first
				NEC_set_by_label_index.push_back(make_pair(label, NEC_set_index));
				NEC_set_array[NEC_set_index++] = NEC_set_array_element(parent_id, represent_child, sum);
				last_label = label;
				continue;
			}
			else if (i == NEC_leaf_mapping_pair_index - 1) {
				// last
				if (label != last_label)
					NEC_set_by_label_index.push_back(make_pair(label, NEC_set_index));
				NEC_set_array[NEC_set_index++] = NEC_set_array_element(parent_id, represent_child, sum);
				NEC_set_by_label_index.push_back(make_pair(-1, NEC_leaf_mapping_pair_index)); // redunant element to set the end
				continue;
			}

			// find a new NEC class
			if (label != last_label) {
				NEC_set_by_label_index.push_back(make_pair(label, NEC_set_index));
				last_label = label;
			}

			NEC_set_array[NEC_set_index++] = NEC_set_array_element(parent_id, represent_child, sum);
		}
	}

#ifdef RESULT_ENUMERATION
	for (long long i = 0; i < g_cnt_node_query_graph; i++) {
		if (g_node_degree_query_graph[i] != 1)
			continue;
		NEC_Node * next = NEC_Node_array[i].nextAddress;
		if (next == NULL)
			continue;
		while (next != NULL)
			next = next->nextAddress;
	}

#endif

	int a = 0;
}


#endif

