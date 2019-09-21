#pragma once
#ifndef FAST_DYNAMIC_SIM_CPI_BUILDER_H_
#define FAST_DYNAMIC_SIM_CPI_BUILDER_H_

#include "cpi_builder.h"


////////////////////////////////////////////////////////////////////////////////

inline double calculateSimilarity(long long data_id, long long query_id) {

	return g_node_similarity_matrix[query_id * g_cnt_node_of_data_graph + data_id];
}


extern set<SimCell> g_need_clean;


inline void select_candidate_by_edge_type_and_sim(long long parent_data_id, long long need_label, EdgeType * edge_type_query, long long need_query_id, long long check_value) {

	set<SimCell> goodNode;
	// iterate parent_data_id's one hop and two hop adj
	for (long long j = g_nodes_adj_list_start_index_data_graph[parent_data_id]; j < g_nodes_adj_list_start_index_data_graph[parent_data_id + 1]; j++) {
		long long onehop = g_nodes_adj_list_with_edge_type_data_graph[j].node_id;
		double sim = 0;

		EdgeType * onehop_edge_type = &g_nodes_adj_list_with_edge_type_data_graph[j];

		// parent -> node (need find), node label, and edge type
		if (g_nodes_label_after_change_data_graph[onehop] == need_label && g_sim_visited[onehop] == 0 && onehop_edge_type->is_in_edge_type(edge_type_query)) {
			sim = calculateSimilarity(onehop, need_query_id);
			if (sim >= MIN_SIM) {
				SimCell e;
				e.id = onehop;
				e.sim = sim;
				goodNode.insert(e);
			}
			g_sim_visited[onehop] = 1;
			g_sim_visited_stack[g_sim_visited_length++] = onehop;

		}

		// iterate two hop
		if (IS_ONE_HOP_DATA_GRAPH != 1) {
			for (long long two = g_nodes_adj_list_start_index_data_graph[onehop]; two < g_nodes_adj_list_start_index_data_graph[onehop + 1]; two++) {
				long long twohop = g_nodes_adj_list_with_edge_type_data_graph[two].node_id;
				EdgeType * twohop_edge_type = &g_nodes_adj_list_with_edge_type_data_graph[two];

				if (g_nodes_label_after_change_data_graph[twohop] == need_label && g_sim_visited[twohop] == 0 && (twohop_edge_type->is_in_edge_type(edge_type_query))) {
					double tSim = calculateSimilarity(twohop, need_query_id);
					if (tSim >= MIN_SIM) {
						SimCell e;
						e.id = twohop;
						e.sim = tSim;
						goodNode.insert(e);
					}

					g_sim_visited[twohop] = 1;
					g_sim_visited_stack[g_sim_visited_length++] = twohop;
				}
			}
		}



	}

	for (int i = 0; i < g_sim_visited_length; i++) {
		g_sim_visited[g_sim_visited_stack[i]] = 0;
		g_sim_visited_stack[i] = 0;
	}
	g_sim_visited_length = 0;

	// clean
	set<SimCell>::iterator it;
	for (it = goodNode.begin(); it != goodNode.end(); ++it) {
		long long e = (*it).id;

		if (g_good_count_data_graph[e] >= check_value - MISSING_EDGE_THRESHOLD_VALUE) {
			g_good_count_data_graph[e] ++;
			g_need_clean.insert(*it);
		}
	}
}


inline void updateCandidatesCheckValueByNeighbor(long long & current_node, char * visited,
	long long & label_cur, long long & check_value) {

	// iterate neighbor of current node
	for (long long j = g_nodes_adj_list_start_index_query_graph[current_node]; j < g_nodes_adj_list_start_index_query_graph[current_node + 1]; j++) {
		long long neighbor = g_nodes_adj_list_with_edge_type_query_graph[j].node_id;
		EdgeType * edge_type = &g_nodes_adj_list_with_edge_type_query_graph[j];

		if (!visited[neighbor]) {
			continue;
		}

		long long pre_visited_adj_node_in_query = neighbor;
		CPINode & nte_pre_node_unit = g_indexSet_idx_is_id[pre_visited_adj_node_in_query];

		// iterate all candidate
		for (long long y = 0; y < nte_pre_node_unit.size; y++) {
			long long nte_neighbor_cand = nte_pre_node_unit.candidates[y];
			// if this candidate has been pruned
			if (nte_neighbor_cand == -1)
				continue;
			if (g_already_visited_data_graph[nte_neighbor_cand] == 1) {
				continue;
			}
			select_candidate_by_edge_type_and_sim(nte_neighbor_cand, label_cur, edge_type, current_node, check_value);
			g_already_visited_data_graph[nte_neighbor_cand] = 1;
		}
	}

	check_value++;
}



inline bool sortByBridgeLength(CPICell p1, CPICell p2) {

	if (p1.bridge_length == p2.bridge_length) {
		return p1.similarity < p2.similarity;
	}

	return p1.bridge_length < p2.bridge_length;
}

typedef pair<int, int> edge;

// forward backward
inline void buildBFSTreeCPI(long long root_candidate_id) {

	pair<int, int>* seq_edge_this_level = new pair<int, int>[10000];

	memset(g_already_visited_data_graph, 0, sizeof(char) * g_cnt_node_of_data_graph);

	g_need_clean.clear();

	char* visited_idx_is_id = g_visited_for_query_graph_idx_is_id;
	memset(visited_idx_is_id, 0, sizeof(char) * g_cnt_node_query_graph);
	visited_idx_is_id[g_root_node_id_of_query] = 1;

	CPINode & root_node_indexSet_unit = g_indexSet_idx_is_id[g_root_node_id_of_query];

	// no NLF check here, and only one node for root indexSet
	root_node_indexSet_unit.candidates[0] = root_candidate_id;
	root_node_indexSet_unit.size = 1;
	fill(root_node_indexSet_unit.path, root_node_indexSet_unit.path + 1, 1);

	
	int seq_edge_this_level_index = 0;
	long long current_level = 0;

	// forward
	for (long long i = 1; i < g_BFS_sequence_length_of_query_all_node; i++) {

		long long current_node_id_in_query = BFS_visit_sequence_of_query_contain_all_nodes[i];
		long long BFS_parent = BFS_parent_query_idx_is_id[current_node_id_in_query];
		visited_idx_is_id[current_node_id_in_query] = 1;
		long long label_cur = g_nodes_label_query_graph_idx_is_id[current_node_id_in_query];
		long long degree_cur = g_node_degree_query_graph_idx_is_id[current_node_id_in_query];
		long long level_cur = BFS_level_query_idx_is_id[current_node_id_in_query];


		//================== BACKWARD PRUNTING ===================================
		//backward pruning for same-level ntes with one level is finished
		// level_cur is the BFS level of current node, when current_level is updated after level_cur
		if (level_cur != current_level) {
			current_level = level_cur;
			seq_edge_this_level_index--;
			while (seq_edge_this_level_index != -1) {
				edge nte = seq_edge_this_level[seq_edge_this_level_index];
				int refinee = nte.first;
				CPINode& refinee_node_unit = g_indexSet_idx_is_id[refinee];
				int label = g_nodes_label_query_graph_idx_is_id[refinee];

				while (nte.first == refinee) {
					int child = nte.second;
					CPINode& child_node_unit = g_indexSet_idx_is_id[child];
					int can_id;
					pair<int, int> result;
					for (int x = 0; x < child_node_unit.size; x++) {
						can_id = child_node_unit.candidates[x];
						if (can_id == -1) {
							continue;
						}

						
					}
				}
			}
		}

#ifdef CORE_AND_MAX_NB_FILTER

		long long core_cur = g_core_number_query_graph[current_node];
#endif
		long long max_nb_degree = 0;
		generateNLFForQueryNode(current_node_id_in_query, degree_cur, max_nb_degree);
		//===================================================================================

		CPINode & cur_node_unit = g_indexSet_idx_is_id[current_node_id_in_query];
		CPINode & parent_unit = g_indexSet_idx_is_id[BFS_parent];

		initCpiNodeByParent(cur_node_unit, parent_unit);

		long long check_value = 0;

		// updateCandidatesCheckValueByNeighbor(current_node, visited, level_cur, label_cur, check_value);
		memset(g_already_visited_data_graph, 0, sizeof(char) * g_cnt_node_of_data_graph);
		updateCandidatesCheckValueByNeighbor(current_node_id_in_query, visited_idx_is_id, label_cur, check_value);

		long long child_index = 0;
		for (long long parent_cand_index = 0; parent_cand_index < parent_unit.size; parent_cand_index++) {
			long long cand_parent = parent_unit.candidates[parent_cand_index];
			if (cand_parent == -1)
				continue;
			count_index_array_for_indexSet = 0;

			// iterate all candidate 
			set<SimCell>::iterator it;
			for (it = g_need_clean.begin(); it != g_need_clean.end(); ++it) {
				long long can_id = (*it).id;
				if (g_good_count_data_graph[can_id] < check_value - MISSING_EDGE_THRESHOLD_VALUE)
					continue;

				if (g_good_count_data_graph[can_id] < 1) {
					continue;
				}

				int dis = 0;

				if (IS_ONE_HOP_DATA_GRAPH > 0) {

					if (SIZEOK == 1) {
						if (g_data_edge_matrix[cand_parent * SIZE_OF_EDGE_MATRIX + can_id] == 0) {
							continue;
						}
					}
					else {
						if (g_data_edge_matrix[(cand_parent % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (can_id % SIZE_OF_EDGE_MATRIX)] == 0) {
							continue;
						}
						else {
							//validate the non tree edge by querying the edgeIndex
							if (!g_hash_table[cand_parent]->query(can_id)) {
								continue;
							}
						}
					}

				}
				else {
					// not in 4 nearest neighbor

					if (g_adj_list_one_hop_distance_data_graph[cand_parent].query(can_id)) {
						dis = 1;
					}
					else if (g_adj_list_two_hop_distance_data_graph[cand_parent].query(can_id)) {
						dis = 2;
					}
					else {
						continue;
					}

				}



				// it has been check before, this means this candid node connectd to diffent parent_cand, like v5 or v6 in Figure 5
				if (g_already_has_one_parent_data_node[can_id] != -1) {
					long long su_zu_pos = g_already_has_one_parent_data_node[can_id];
					cur_node_unit.path[su_zu_pos] += parent_unit.path[parent_cand_index];
					// add to index_N_up_u
					g_index_array_for_indexSet[count_index_array_for_indexSet].index = g_already_has_one_parent_data_node[can_id];
					// add length

					if (IS_ONE_HOP_DATA_GRAPH > 0) {
						dis = 1;
					}
					else {
						dis = dis;
					}

					g_index_array_for_indexSet[count_index_array_for_indexSet].bridge_length = dis;
					g_index_array_for_indexSet[count_index_array_for_indexSet++].similarity = (*it).sim;
					continue;
				}

#ifdef CORE_AND_MAX_NB_FILTER
				if (g_node_degree_data_graph[can_id] < degree_cur || g_core_number_data_graph[can_id] < core_cur || MAX_NB_degree_data[can_id] < max_nb_degree)
					continue;
#endif

				cur_node_unit.candidates[child_index] = can_id;
				cur_node_unit.path[child_index] = parent_unit.path[parent_cand_index];
				g_already_has_one_parent_data_node[can_id] = child_index;
				// add to index_N_up_u
				g_index_array_for_indexSet[count_index_array_for_indexSet].index = child_index;
				// add length

				if (IS_ONE_HOP_DATA_GRAPH > 0) {
					dis = 1;
				}
				else {
					dis = dis;
				}

				g_index_array_for_indexSet[count_index_array_for_indexSet].bridge_length = dis;
				g_index_array_for_indexSet[count_index_array_for_indexSet].similarity = (*it).sim;
				count_index_array_for_indexSet++;

				child_index++;
			}
			if (cur_node_unit.size_of_index[parent_cand_index] < count_index_array_for_indexSet) {
				cur_node_unit.index_N_up_u[parent_cand_index] = new CPICell[count_index_array_for_indexSet];
			}
			//copy(g_index_array_for_indexSet, g_index_array_for_indexSet + count_index_array_for_indexSet, cur_node_unit.index_N_up_u[parent_cand_index]);
			sort(g_index_array_for_indexSet, g_index_array_for_indexSet + count_index_array_for_indexSet, sortByBridgeLength);
			memcpy(cur_node_unit.index_N_up_u[parent_cand_index], g_index_array_for_indexSet, sizeof(CPICell) * count_index_array_for_indexSet);
			cur_node_unit.size_of_index[parent_cand_index] = count_index_array_for_indexSet;

		}

		cur_node_unit.size = child_index;

		for (long long x = 0; x < child_index; x++)
			g_already_has_one_parent_data_node[cur_node_unit.candidates[x]] = -1;
		set<SimCell>::iterator it;
		for (it = g_need_clean.begin(); it != g_need_clean.end(); ++it) {
			long long can_id = (*it).id;
			g_good_count_data_graph[can_id] = 0;
		}
		g_need_clean.clear();
	}

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// build current index, on parent
// parent_id_query is only use for building adj_list in CPI
inline int buildSimCPINode(long long currentId_query, long long parent_id_query) {

	memset(g_already_visited_data_graph, 0, sizeof(char) * g_cnt_node_of_data_graph);

	int pre_visited_num = 0;

	long long curLabel = g_nodes_label_query_graph_idx_is_id[currentId_query];
	char * visited = g_visited_for_query_graph_idx_is_id;
	// build according to its neighbor
	long long current_id_ = currentId_query;

	initCpiNodeByParent(g_indexSet_idx_is_id[currentId_query], g_indexSet_idx_is_id[parent_id_query]);

	long long check_value = 0;

	updateCandidatesCheckValueByNeighbor(currentId_query, visited, curLabel, check_value);

	CPINode & cur_node_unit = g_indexSet_idx_is_id[current_id_];
	CPINode & parent_unit = g_indexSet_idx_is_id[parent_id_query];

	long long child_index = 0;
	for (long long parent_cand_index = 0; parent_cand_index < parent_unit.size; parent_cand_index++) {
		long long cand_parent = parent_unit.candidates[parent_cand_index];
		if (cand_parent == -1)
			continue;
		count_index_array_for_indexSet = 0;

		// iterate all candidate 
		set<SimCell>::iterator it;
		for (it = g_need_clean.begin(); it != g_need_clean.end(); ++it) {
			long long can_id = (*it).id;
			if (g_good_count_data_graph[can_id] < check_value - MISSING_EDGE_THRESHOLD_VALUE)
				continue;

			if (g_good_count_data_graph[can_id] < 1) {
				continue;
			}

			int dis = 0;

			if (IS_ONE_HOP_DATA_GRAPH > 0) {

				if (SIZEOK == 1) {
					if (g_data_edge_matrix[cand_parent * SIZE_OF_EDGE_MATRIX + can_id] == 0) {
						continue;
					}
				}
				else {
					if (g_data_edge_matrix[(cand_parent % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (can_id % SIZE_OF_EDGE_MATRIX)] == 0) {
						continue;
					}
					else {
						//validate the non tree edge by querying the edgeIndex
						if (!g_hash_table[cand_parent]->query(can_id)) {
							continue;
						}
					}
				}

			}
			else {
				// not in 4 nearest neighbor

				if (g_adj_list_one_hop_distance_data_graph[cand_parent].query(can_id)) {
					dis = 1;
				}
				else if (g_adj_list_two_hop_distance_data_graph[cand_parent].query(can_id)) {
					dis = 2;
				}
				else {
					continue;
				}

			}

			// it has been check before, this means this candid node connectd to diffent parent_cand, like v5 or v6 in Figure 5
			if (g_already_has_one_parent_data_node[can_id] != -1) {
				long long su_zu_pos = g_already_has_one_parent_data_node[can_id];
				cur_node_unit.path[su_zu_pos] += parent_unit.path[parent_cand_index];
				// add to index_N_up_u
				g_index_array_for_indexSet[count_index_array_for_indexSet].index = g_already_has_one_parent_data_node[can_id];
				// add length
				if (IS_ONE_HOP_DATA_GRAPH > 0) {
					dis = 1;
				}
				else {

					dis = dis;

				}

				g_index_array_for_indexSet[count_index_array_for_indexSet].bridge_length = dis;
				g_index_array_for_indexSet[count_index_array_for_indexSet++].similarity = (*it).sim;
				continue;
			}

			cur_node_unit.candidates[child_index] = can_id;
			cur_node_unit.path[child_index] = parent_unit.path[parent_cand_index];
			g_already_has_one_parent_data_node[can_id] = child_index;
			// add to index_N_up_u
			g_index_array_for_indexSet[count_index_array_for_indexSet].index = child_index;
			// add length
			if (IS_ONE_HOP_DATA_GRAPH > 0) {
				dis = 1;
			}
			else {
				dis = dis;
			}

			g_index_array_for_indexSet[count_index_array_for_indexSet].bridge_length = dis;
			g_index_array_for_indexSet[count_index_array_for_indexSet].similarity = (*it).sim;
			count_index_array_for_indexSet++;

			child_index++;
		}
		if (cur_node_unit.size_of_index[parent_cand_index] < count_index_array_for_indexSet) {
			cur_node_unit.index_N_up_u[parent_cand_index] = new CPICell[count_index_array_for_indexSet];
		}
		//copy(g_index_array_for_indexSet, g_index_array_for_indexSet + count_index_array_for_indexSet, cur_node_unit.index_N_up_u[parent_cand_index]);
		sort(g_index_array_for_indexSet, g_index_array_for_indexSet + count_index_array_for_indexSet, sortByBridgeLength);
		memcpy(cur_node_unit.index_N_up_u[parent_cand_index], g_index_array_for_indexSet, sizeof(CPICell) * count_index_array_for_indexSet);
		cur_node_unit.size_of_index[parent_cand_index] = count_index_array_for_indexSet;

	}

	cur_node_unit.size = child_index;

	for (long long x = 0; x < child_index; x++)
		g_already_has_one_parent_data_node[cur_node_unit.candidates[x]] = -1;
	set<SimCell>::iterator it;
	for (it = g_need_clean.begin(); it != g_need_clean.end(); ++it) {
		long long can_id = (*it).id;
		g_good_count_data_graph[can_id] = 0;
	}
	g_need_clean.clear();

	return check_value;
}


inline void buildBFSSequence() {

	CoreQueryBFSTreeNode * root = &core_query_tree_idx_is_id[g_root_node_id_of_query];
	BFS_visit_sequence_of_query_contain_all_nodes[0] = g_root_node_id_of_query;
	BFS_parent_query_idx_is_id[g_root_node_id_of_query] = -1;

	for (int i = 0; i < g_forward_build_sequence.size(); i++) {
		long long nodeId = g_forward_build_sequence[i];
		BFS_parent_query_idx_is_id[nodeId] = g_forward_build_parent[i];
	}

}

inline void buildDynamicTreeCPI(long long root_candidate_id) {

	g_need_clean.clear();

	g_forward_build_sequence.clear();
	memset(g_forward_build_parent, -9999, sizeof(long long) * MAX_QUERY_NODE);

	// build root indexSet
	initRootCPI(root_candidate_id);

	g_forward_build_sequence.push_back(g_root_node_id_of_query);
	g_forward_build_parent[g_root_node_id_of_query] = -1;
	memset(g_forward_level, 0, sizeof(int) * MAX_QUERY_NODE);
	g_forward_level[g_root_node_id_of_query] = 1;
	g_level[1].push_back(g_root_node_id_of_query); // 0 is never used

	char * visited = g_visited_for_query_graph_idx_is_id;
	memset(visited, 0, sizeof(char) * g_cnt_node_query_graph);
	visited[g_root_node_id_of_query] = 1;

	// Forward build, backward pruning
	CPINodeSize root;
	root.CPI_id_in_query = g_root_node_id_of_query;
	root.size = 1;
	root.degree = 1;
	root.pre_visited_num = 0;
	root.level = 1;

	vector<CPINodeSize> cpi_small_heap;
	cpi_small_heap.push_back(root);
	make_heap(cpi_small_heap.begin(), cpi_small_heap.end(), greater<CPINodeSize>());

	//////////////////////////////////////////
	resetTreeNodes();

	g_core_tree_node_child_array_index = 0;
	g_core_tree_node_nte_array_index = 0;
	initializeTreeNode(core_query_tree_idx_is_id[g_root_node_id_of_query], -1);
	///////////////////////////////////////////

	g_core_size = 1;
	while (!cpi_small_heap.empty()) {

		// pop top
		pop_heap(cpi_small_heap.begin(), cpi_small_heap.end(), greater<CPINodeSize>());
		CPINodeSize cur = cpi_small_heap.back();
		cpi_small_heap.pop_back();

		long long cur_id_in_query = cur.CPI_id_in_query;
		// after pop a CPI
		// iterate its neighbors
		for (long long m = g_nodes_adj_list_start_index_query_graph[cur_id_in_query]; m < g_nodes_adj_list_start_index_query_graph[cur_id_in_query + 1]; m++) {
			long long child_node_in_query = g_nodes_adj_list_with_edge_type_query_graph[m].node_id;
			//EdgeType * edge_type_between_child_node = & g_nodes_adj_list_with_edge_type_query_graph[m];

			// check if child_node is in core
			//if (g_core_number_query_graph[child_node_in_query] < 2) continue;

			/////////////////////////////////////////////////////
			if (visited[child_node_in_query] != 0) {
				if (child_node_in_query != core_query_tree_idx_is_id[cur_id_in_query].parent_node) {
					addNonTreeEdgeToTreeNode(core_query_tree_idx_is_id[cur_id_in_query], child_node_in_query);
				}
			}
			////////////////////////////////////////////////////
			// this neighbor never visited before
			if (visited[child_node_in_query] == 0) {
				visited[child_node_in_query] = 1;
				// build CPI for child_node_in_query
				int pre_visited_num = buildSimCPINode(child_node_in_query, cur_id_in_query);

				CPINodeSize cpiSize;
				cpiSize.CPI_id_in_query = child_node_in_query;
				cpiSize.size = g_indexSet_idx_is_id[child_node_in_query].size;
				cpiSize.degree = g_node_degree_query_graph_idx_is_id[child_node_in_query];
				cpiSize.pre_visited_num = pre_visited_num;
				cpiSize.level = g_forward_level[cur_id_in_query] + 1;

				if (cpiSize.size <= 0) {
					visited[child_node_in_query] = 0;
					continue;
				}
				g_forward_build_sequence.push_back(child_node_in_query);
				g_forward_build_parent[child_node_in_query] = cur_id_in_query;
				int level = g_forward_level[cur_id_in_query] + 1;
				g_forward_level[child_node_in_query] = level;

				g_level[level].push_back(child_node_in_query); // used in backward prune
				if (g_level_size < level) {
					g_level_size = level;
				}

				cpi_small_heap.push_back(cpiSize);
				push_heap(cpi_small_heap.begin(), cpi_small_heap.end(), greater<CPINodeSize>());
				g_core_size++;

				initializeTreeNode(core_query_tree_idx_is_id[child_node_in_query], cur_id_in_query);
				addChildToTreeNode(core_query_tree_idx_is_id[cur_id_in_query], child_node_in_query);
			}
		}
	}

	buildBFSSequence();

}

inline void forwardPrune(int root_cand_id) {
	char* visited = g_visited_for_query_graph_idx_is_id;
	memset(visited, 0, sizeof(char) * g_cnt_node_query_graph);

	int query_edge_size = g_nodes_adj_list_with_edge_type_query_graph.size();
	int data_edge_size = size_of_g_nodes_adj_list_with_edge_type_data_graph;

	if (!g_is_init_edge_matchinig_array) {
		g_edge_matching_array = new int[query_edge_size * data_edge_size];
		g_is_init_edge_matchinig_array = true;
		memset(g_edge_matching_array, 0, sizeof(int) * query_edge_size * data_edge_size);
	}

	int* edge_matching_array = g_edge_matching_array;


	for (long long level = 2; level <= g_level_size; level++) {
		long long current_level = level;

		stable_sort(g_level[current_level].begin(), g_level[current_level].end(), sortByDegree_Query_dec);

		for (long long seq_index = 0; seq_index < g_level[current_level].size(); seq_index++) {
			long long cur_node = g_level[current_level][seq_index];

			//if this node is a nec node, then it doesnt need to be refined by simulation.
			//if (NEC_map[cur_node] != -1) {
			//	continue;
			//}

			CPINode& cur_node_unit = g_indexSet_idx_is_id[cur_node];
			long long label_cur = g_nodes_label_query_graph_idx_is_id[cur_node];

			//==== FIRST STEP: deal with its BFS children =====================

			CoreQueryBFSTreeNode& tree_node = core_query_tree_idx_is_id[cur_node];

			for (long long j = tree_node.children.first; j < tree_node.children.first + tree_node.children.second; j++) {
				long long child = g_core_tree_node_child_array[j];
				visited[child] = 1;

				if (NEC_map[child] != -1 && NEC_map[child] != child)
					continue;

				CPINode& child_node_unit = g_indexSet_idx_is_id[child];

				// check each candidate node which belongs to current node
				for (long long cand_index = 0; cand_index < cur_node_unit.size; cand_index++) {
					if (cur_node_unit.candidates[cand_index] == -1)
						continue;

					//check the positions one by one
					CPICell* temp_index = child_node_unit.index_N_up_u[cand_index];
					long long& temp_size_of_index = child_node_unit.size_of_index[cand_index];

					for (long long cand_child_index = 0; cand_child_index < temp_size_of_index; cand_child_index++) {
						long long node_pos = temp_index[cand_child_index].index;
						if (child_node_unit.candidates[node_pos] == -1) {
							temp_index[cand_child_index] = temp_index[temp_size_of_index - 1];
							temp_size_of_index--;
							cand_child_index--;
						}
					} // end for : finished checking the positions

					// candiate node were pruned here
					// -1 means prune
					if (temp_size_of_index == 0)
						cur_node_unit.candidates[cand_index] = -1;
				}

			}
			//=================================================================
			//====== SECOND STEP : deal with non-BFS children with larger level =====================

			long long check_value = 0;
			for (long long j = g_nodes_adj_list_start_index_query_graph[cur_node]; j < g_nodes_adj_list_start_index_query_graph[cur_node + 1]; j++) {
				long long neighbor = g_nodes_adj_list_with_edge_type_query_graph[j].node_id;
				EdgeType current_to_child = g_nodes_adj_list_with_edge_type_query_graph[j];
				int query_edge_index = j;
				// only consider bigger, because the index is build in forward sequence.
				//if (g_forward_level[child] <= current_level || visited[child])
				//	continue;

				//children here must have a larger level than the current level, so it would be a child with a cross-level nte
				//if (NEC_map[child] != -1 && NEC_map[child] != child)
				//	continue;

				long long can_id;
				pair<long long, long long> result;
				CPINode& neighbor_node_unit = g_indexSet_idx_is_id[neighbor];
				for (long long x = 0; x < neighbor_node_unit.size; x++) {
					can_id = neighbor_node_unit.candidates[x];
					if (can_id == -1)
						continue;
					// result is the neighbor, not the parent.
					result = g_node_id_label_position_matrix_data_graph[can_id * (g_cnt_unique_label_data_graph + 1) + label_cur];

					for (long long y = result.first; y < result.first + result.second; y++) {
						// check whether temp_node is a good candidate for current_indexset.
						long long temp_node = g_nodes_adj_list_with_edge_type_data_graph[y].node_id;
						EdgeType child_to_current = g_nodes_adj_list_with_edge_type_data_graph[y];
						int data_edge_index = y;

						if (v_cnt[temp_node] == check_value) {
							// not run before
							int index_in_edge_matching = query_edge_index * data_edge_size + data_edge_index;
							if (edge_matching_array[index_in_edge_matching] == 0) {
								if (child_to_current.is_in_edge_type(&current_to_child)) {
									edge_matching_array[index_in_edge_matching] = 1;
									v_cnt[temp_node] ++;
									if (check_value == 0)
										g_good_count_need_clean_index_data_graph[g_good_count_need_clean_size++] = temp_node;
								}
								else {
									edge_matching_array[index_in_edge_matching] = -1;
								}
							}
							else if (edge_matching_array[index_in_edge_matching] == 1) {
								v_cnt[temp_node] ++;
								if (check_value == 0)
									g_good_count_need_clean_index_data_graph[g_good_count_need_clean_size++] = temp_node;
							}

						}
					}
				}
				check_value++;

			}

			for (long long cand_index = 0; cand_index < cur_node_unit.size; cand_index++) {
				int& cand_id = cur_node_unit.candidates[cand_index];
				if (cand_id == -1)
					continue;
				if (v_cnt[cand_id] != check_value || root_cand_id == cand_id)
					cand_id = -1;
			}

			while (g_good_count_need_clean_size != 0)
				v_cnt[g_good_count_need_clean_index_data_graph[--g_good_count_need_clean_size]] = 0;

			for (long long j = tree_node.children.first; j < tree_node.children.first + tree_node.children.second; j++)
				visited[g_core_tree_node_child_array[j]] = 0;
			//=============================================================================================

		}


	}
}

inline void backwardPrune(int root_cand_id) {

	char * visited = g_visited_for_query_graph_idx_is_id;
	memset(visited, 0, sizeof(char) * g_cnt_node_query_graph);

	int query_edge_size = g_nodes_adj_list_with_edge_type_query_graph.size();
	int data_edge_size = size_of_g_nodes_adj_list_with_edge_type_data_graph;

	if (!g_is_init_edge_matchinig_array) {
		g_edge_matching_array = new int[query_edge_size * data_edge_size];
		g_is_init_edge_matchinig_array = true;
		memset(g_edge_matching_array, 0, sizeof(int) * query_edge_size * data_edge_size);
	}

	int * edge_matching_array = g_edge_matching_array;
	

	for (long long level = g_level_size; level > 1; level--) {
		long long current_level = level;

		stable_sort(g_level[current_level].begin(), g_level[current_level].end(), sortByDegree_Query_dec);

		for (long long seq_index = 0; seq_index < g_level[current_level].size(); seq_index++) {
			long long cur_node = g_level[current_level][seq_index];

			//if this node is a nec node, then it doesnt need to be refined by simulation.
			//if (NEC_map[cur_node] != -1) {
			//	continue;
			//}

			CPINode & cur_node_unit = g_indexSet_idx_is_id[cur_node];
			long long label_cur = g_nodes_label_query_graph_idx_is_id[cur_node];

			//==== FIRST STEP: deal with its BFS children =====================
			
			CoreQueryBFSTreeNode & tree_node = core_query_tree_idx_is_id[cur_node];
			
			for (long long j = tree_node.children.first; j < tree_node.children.first + tree_node.children.second; j++) {
				long long child = g_core_tree_node_child_array[j];
				visited[child] = 1;

				if (NEC_map[child] != -1 && NEC_map[child] != child)
					continue;

				CPINode & child_node_unit = g_indexSet_idx_is_id[child];

				// check each candidate node which belongs to current node
				for (long long cand_index = 0; cand_index < cur_node_unit.size; cand_index++) {
					if (cur_node_unit.candidates[cand_index] == -1)
						continue;

					//check the positions one by one
					CPICell * temp_index = child_node_unit.index_N_up_u[cand_index];
					long long & temp_size_of_index = child_node_unit.size_of_index[cand_index];

					for (long long cand_child_index = 0; cand_child_index < temp_size_of_index; cand_child_index++) {
						long long node_pos = temp_index[cand_child_index].index;
						if (child_node_unit.candidates[node_pos] == -1) {
							temp_index[cand_child_index] = temp_index[temp_size_of_index - 1];
							temp_size_of_index--;
							cand_child_index--;
						}
					} // end for : finished checking the positions

					// candiate node were pruned here
					// -1 means prune
					if (temp_size_of_index == 0)
						cur_node_unit.candidates[cand_index] = -1;
				}

			}
			//=================================================================
			//====== SECOND STEP : deal with non-BFS children with larger level =====================

			long long check_value = 0;
			for (long long j = g_nodes_adj_list_start_index_query_graph[cur_node]; j < g_nodes_adj_list_start_index_query_graph[cur_node + 1]; j++) {
				long long neighbor = g_nodes_adj_list_with_edge_type_query_graph[j].node_id;
				EdgeType current_to_child = g_nodes_adj_list_with_edge_type_query_graph[j];
				int query_edge_index = j;
				// only consider bigger, because the index is build in forward sequence.
				//if (g_forward_level[child] <= current_level || visited[child])
				//	continue;

				//children here must have a larger level than the current level, so it would be a child with a cross-level nte
				//if (NEC_map[child] != -1 && NEC_map[child] != child)
				//	continue;

				long long can_id;
				pair<long long, long long> result;
				CPINode & neighbor_node_unit = g_indexSet_idx_is_id[neighbor];
				for (long long x = 0; x < neighbor_node_unit.size; x++) {
					can_id = neighbor_node_unit.candidates[x];
					if (can_id == -1)
						continue;
					// result is the neighbor, not the parent.
					result = g_node_id_label_position_matrix_data_graph[can_id * (g_cnt_unique_label_data_graph + 1) + label_cur];

					for (long long y = result.first; y < result.first + result.second; y++) {
						// check whether temp_node is a good candidate for current_indexset.
						long long temp_node = g_nodes_adj_list_with_edge_type_data_graph[y].node_id;
						EdgeType child_to_current = g_nodes_adj_list_with_edge_type_data_graph[y];
						int data_edge_index = y;

						if (v_cnt[temp_node] == check_value) {
							// not run before
							int index_in_edge_matching = query_edge_index * data_edge_size + data_edge_index;
							if (edge_matching_array[index_in_edge_matching] == 0) {
								if (child_to_current.is_in_edge_type(&current_to_child)) {
									edge_matching_array[index_in_edge_matching] = 1;
									v_cnt[temp_node] ++;
									if (check_value == 0)
										g_good_count_need_clean_index_data_graph[g_good_count_need_clean_size++] = temp_node;
								}
								else {
									edge_matching_array[index_in_edge_matching] = -1;
								}
							}
							else if (edge_matching_array[index_in_edge_matching] == 1) {
								v_cnt[temp_node] ++;
								if (check_value == 0)
									g_good_count_need_clean_index_data_graph[g_good_count_need_clean_size++] = temp_node;
							}

						}
					}
				}
				check_value++;

			}

			for (long long cand_index = 0; cand_index < cur_node_unit.size; cand_index++) {
				int & cand_id = cur_node_unit.candidates[cand_index];
				if (cand_id == -1)
					continue;
				if (v_cnt[cand_id] != check_value || root_cand_id == cand_id)
					cand_id = -1;
			}

			while (g_good_count_need_clean_size != 0)
				v_cnt[g_good_count_need_clean_index_data_graph[--g_good_count_need_clean_size]] = 0;

			for (long long j = tree_node.children.first; j < tree_node.children.first + tree_node.children.second; j++)
				visited[g_core_tree_node_child_array[j]] = 0;
			//=============================================================================================

		}


	}

	/*
	for (int i = g_forward_build_sequence.size() - 1; i >= 0; i--) {
		long long id = g_forward_build_sequence[i];
		visited[id] = 1;
		int g_check_value = 0;
		CPINode & cpi = indexSet[id];

		for (long long i = g_nodes_adj_list_start_index_query_graph[id]; i < g_nodes_adj_list_start_index_query_graph[id + 1]; i++) {
			// this is neighbor
			long long query_id = g_nodes_adj_list_query_graph[i];
			if (visited[query_id]) {
				continue;
			}
			CPINode & neighborCPI = indexSet[query_id];
			prune(cpi, neighborCPI);
		}
	}
	*/
}




#endif