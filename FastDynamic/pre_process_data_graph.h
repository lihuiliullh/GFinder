#pragma warning(disable:4996)
#pragma once

#ifndef FAST_DYNAMIC_PRE_PROCESS_DATA_GRAPH_H_
#define FAST_DYNAMIC_PRE_PROCESS_DATA_GRAPH_H_

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1

#include <string>
#include <istream>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <set>
#include "global_function.h"
#include "global_variable.h"
#include "matching_order.h"

using namespace std;


// calculate there are how many distinct label and the max label is what
inline void preprocessDataGraph(string data_graph_file) {
	ifstream fin(data_graph_file);
	string line;
	getline(fin, line);
	vector<string> v;
	split(line, v, ' ');
	g_cnt_node_of_data_graph = atoi(v[2].c_str());

	g_nodes_label_after_change_data_graph = new int[g_cnt_node_of_data_graph];

	FILE *fp;
	char str[255];

	fp = fopen(data_graph_file.c_str(), "r");
	long long largest_label = -1;
	unordered_set<long long> unique_label_set;
	long long count_edge_number = 0;

	while (fgets(str, 255, fp) != NULL) {
		line = string(str);
		if (line.at(0) == 'v') {
			split(line, v, ' ');
			int label = atoi(v[2].c_str());
			if (label > largest_label) {
				largest_label = label;
			}
			unique_label_set.insert(label);
		}
		if (line.at(0) == 'e') {
			count_edge_number++;
		}
	}

	fclose(fp);

	g_largest_label_data_graph = largest_label;
	g_cnt_unique_label_data_graph = unique_label_set.size();
	g_label_freqency_data_graph = new int[g_cnt_unique_label_data_graph + 1];
	memset(g_label_freqency_data_graph, 0, sizeof(int) * (g_cnt_unique_label_data_graph + 1));


	SIZEOK = (g_cnt_node_of_data_graph <= SIZE_OF_EDGE_MATRIX) ? 1 : 0;
	SIZE_OF_EDGE_MATRIX = (g_cnt_node_of_data_graph <= SIZE_OF_EDGE_MATRIX) ? g_cnt_node_of_data_graph : SIZE_OF_EDGE_MATRIX;
	g_data_edge_matrix = new char[SIZE_OF_EDGE_MATRIX * SIZE_OF_EDGE_MATRIX];
	memset(g_data_edge_matrix, 0, sizeof(char) * SIZE_OF_EDGE_MATRIX * SIZE_OF_EDGE_MATRIX);

}



inline void buildAdjListAndLabelPositionMatrix(vector<vector<EdgeType>> & adj_list, long long sum_degree) {
	g_nodes_adj_list_with_edge_type_data_graph = new EdgeType[sum_degree];
	size_of_g_nodes_adj_list_with_edge_type_data_graph = sum_degree;

	long long cur_adj_start_index = 0;
	long long cur_node_id_label_matrix_index = 0;

	g_nodes_adj_list_start_index_data_graph = new int[g_cnt_node_of_data_graph + 1];
	g_node_id_label_position_matrix_data_graph = new pair<int, int>[g_cnt_node_of_data_graph * (g_cnt_unique_label_data_graph + 1)];

	for (int i = 0; i < g_cnt_node_of_data_graph; i++) {
		// put node's sorted adjacent list into the array
		if (adj_list[i].size() == 0) {
			// this shouldn't happen
			g_nodes_adj_list_start_index_data_graph[i] = cur_adj_start_index;
			continue;
		}

		g_nodes_adj_list_start_index_data_graph[i] = cur_adj_start_index;
		sort(adj_list[i].begin(), adj_list[i].end(), sortByLabelAndDegree_edge_type);
		copy(adj_list[i].begin(), adj_list[i].end(), g_nodes_adj_list_with_edge_type_data_graph + cur_adj_start_index);
		cur_adj_start_index += adj_list[i].size();

		// push edge into has table

		// record adj's label info
		long long begin_label = g_nodes_label_after_change_data_graph[adj_list[i][0].node_id];
		long long cur_count = 1;

		if (adj_list[i].size() == 1) {
			g_node_id_label_position_matrix_data_graph[i * (g_cnt_unique_label_data_graph + 1) + begin_label] =
				make_pair(cur_node_id_label_matrix_index, cur_count);
			cur_node_id_label_matrix_index += cur_count;
			continue;
		}
		
		for (long long j = 1; j < adj_list[i].size(); j++) {
			int this_label = g_nodes_label_after_change_data_graph[adj_list[i][j].node_id];
			if (this_label == begin_label) {
				cur_count++;
			}
			else {
				g_node_id_label_position_matrix_data_graph[i * (g_cnt_unique_label_data_graph + 1) + begin_label] =
					make_pair(cur_node_id_label_matrix_index, cur_count);
				cur_node_id_label_matrix_index += cur_count;
				begin_label = this_label;
				cur_count = 1;
			}
		}

		g_node_id_label_position_matrix_data_graph[i * (g_cnt_unique_label_data_graph + 1) + begin_label] =
			make_pair(cur_node_id_label_matrix_index, cur_count);
		cur_node_id_label_matrix_index += cur_count;

	}

	g_nodes_adj_list_start_index_data_graph[g_cnt_node_of_data_graph] = sum_degree;
}

inline void buildLabel2NodeID_only_used_in_seedfinder() {
	long long last_label = 0;
	long long last_degree = 0;
	g_node_id_sort_by_label_degree_data_graph = new int[g_cnt_node_of_data_graph];

	for (int i = 0; i < g_cnt_node_of_data_graph; i++) {
		g_node_id_sort_by_label_degree_data_graph[i] = i;
	}

	sort(g_node_id_sort_by_label_degree_data_graph, g_node_id_sort_by_label_degree_data_graph + g_cnt_node_of_data_graph, sortByLabelAndDegree);
	g_degree_of_sort_by_label_degree_data_graph.resize(g_cnt_node_of_data_graph);

	for (long long i = 0; i < g_cnt_node_of_data_graph; i++) {
		g_degree_of_sort_by_label_degree_data_graph[i] = g_node_degree_data_graph[g_node_id_sort_by_label_degree_data_graph[i]];
	}

	g_label_to_sort_by_degree_node_data_graph = new pair<int, int>[g_cnt_unique_label_data_graph + 1];

	for (long long i = 0; i < g_cnt_node_of_data_graph; i++) {
		long long v = g_node_id_sort_by_label_degree_data_graph[i];
		long long label = g_nodes_label_after_change_data_graph[v];
		long long degree = g_node_degree_data_graph[v];
		if (i == 0) {
			g_label_to_sort_by_degree_node_data_graph[0] = make_pair(0, 0);
			last_label = label;
			last_degree = degree;
		}
		else {
			if (label != last_label) {
				g_label_to_sort_by_degree_node_data_graph[last_label] = make_pair(last_degree, i);
			}
			last_label = label;
			last_degree = degree;
		}
	}

	g_label_to_sort_by_degree_node_data_graph[last_label] = make_pair(last_degree, g_cnt_node_of_data_graph);
}


inline void buildNLFCheck() {
	NLF_size = (g_cnt_unique_label_data_graph + 1) / SIZEOF_INT + 1;
	NLF_array_query = new long long[NLF_size]; // the array for query graph
	NLF_check = new long long[g_cnt_node_of_data_graph * NLF_size];
	memset(NLF_check, 0, sizeof(long long) * NLF_size * g_cnt_node_of_data_graph);
}

inline void initNEC() {
	NEC_mapping_child_with_same_label_cnt = new int[(g_cnt_unique_label_data_graph + 1) * MAX_QUERY_NODE]; //label and the parent node id
	memset(NEC_mapping_child_with_same_label_cnt, 0, sizeof(int) * (g_cnt_unique_label_data_graph + 1) * MAX_QUERY_NODE);

	NEC_mapping_pair = new NEC_element[(g_cnt_unique_label_data_graph + 1) * MAX_QUERY_NODE]; // <label, parent node id>
	NEC_set_array = new NEC_set_array_element[(g_cnt_unique_label_data_graph + 1) * MAX_QUERY_NODE]; // <parent_id, sum>
}




//inline void addTwoToThreeNearestNeighborAndDis() {
//
//	// clean
//	memset(g_good_count_data_graph, 0, sizeof(int) * g_cnt_node_of_data_graph);
//	vector<long long> visited;
//	bool isThree = false;
//
//	g_adj_list_one_to_four_distance_data_graph = new char *[g_cnt_node_of_data_graph];
//	for (int i = 0; i < g_cnt_node_of_data_graph; i++) {
//		g_adj_list_one_to_four_distance_data_graph[i] = new char[g_cnt_node_of_data_graph];
//		memset(g_adj_list_one_to_four_distance_data_graph[i], 0, sizeof(char) * g_cnt_node_of_data_graph);
//	}
//
//	for (int node_id = 0; node_id < g_cnt_node_of_data_graph; node_id++) {
//
//		g_good_count_data_graph[node_id] = 1;
//		// iterate neighbor
//		for (long long one = g_nodes_adj_list_start_index_data_graph[node_id]; one < g_nodes_adj_list_start_index_data_graph[node_id + 1]; one++) {
//			long long neighborId = g_nodes_adj_list_with_edge_type_data_graph[one].node_id;
//			// one
//			//g_adj_list_one_to_four_data_graph[node_id].insert(neighborId);
//			g_adj_list_one_to_four_distance_data_graph[node_id][neighborId] = 1;
//			g_good_count_data_graph[neighborId] = 1;
//		}
//
//		for (long long one = g_nodes_adj_list_start_index_data_graph[node_id]; one < g_nodes_adj_list_start_index_data_graph[node_id + 1]; one++) {
//			long long neighborId = g_nodes_adj_list_with_edge_type_data_graph[one].node_id;
//
//			for (long long two = g_nodes_adj_list_start_index_data_graph[neighborId]; two < g_nodes_adj_list_start_index_data_graph[neighborId + 1]; two++) {
//				long long twoNeighbor = g_nodes_adj_list_with_edge_type_data_graph[two].node_id;
//				// no visited before, two
//				if (g_good_count_data_graph[twoNeighbor] == 0) {
//					//g_adj_list_one_to_four_data_graph[node_id].insert(twoNeighbor);
//					g_adj_list_one_to_four_distance_data_graph[node_id][twoNeighbor] = 2;
//					g_good_count_data_graph[twoNeighbor] = 1;
//				}
//				if (isThree) {
//					for (long long three = g_nodes_adj_list_start_index_data_graph[twoNeighbor]; three < g_nodes_adj_list_start_index_data_graph[twoNeighbor + 1]; three++) {
//						long long threeNeighbor = g_nodes_adj_list_with_edge_type_data_graph[three].node_id;
//						// three
//						if (g_good_count_data_graph[threeNeighbor] == 0) {
//							//g_adj_list_one_to_four_data_graph[node_id].insert(threeNeighbor);
//							g_adj_list_one_to_four_distance_data_graph[node_id][threeNeighbor] = 3;
//							g_good_count_data_graph[threeNeighbor] = 1;
//						}
//					}
//
//				}
//			}
//
//		}
//
//		memset(g_good_count_data_graph, 0, sizeof(int) * g_cnt_node_of_data_graph);
//	}
//
//}


inline void addOneAndTwoHopNeighbor() {

	// clean
	memset(g_good_count_data_graph, 0, sizeof(int) * g_cnt_node_of_data_graph);
	vector<long long> visited;

	void* raw_memory = operator new[](g_cnt_node_of_data_graph * sizeof(HashTable));
	g_adj_list_one_hop_distance_data_graph = static_cast<HashTable*>(raw_memory);
	for (int i = 0; i < g_cnt_node_of_data_graph; ++i) {
		new(&g_adj_list_one_hop_distance_data_graph[i])HashTable(8);
	}

	void* raw_memory2 = operator new[](g_cnt_node_of_data_graph * sizeof(HashTable));
	g_adj_list_two_hop_distance_data_graph = static_cast<HashTable*>(raw_memory2);
	for (int i = 0; i < g_cnt_node_of_data_graph; ++i) {
		new(&g_adj_list_two_hop_distance_data_graph[i])HashTable(64);
	}

	for (int node_id = 0; node_id < g_cnt_node_of_data_graph; node_id++) {

		g_good_count_data_graph[node_id] = 1;
		// iterate neighbor
		for (long long one = g_nodes_adj_list_start_index_data_graph[node_id]; one < g_nodes_adj_list_start_index_data_graph[node_id + 1]; one++) {
			long long neighborId = g_nodes_adj_list_with_edge_type_data_graph[one].node_id;

			// one
			g_adj_list_one_hop_distance_data_graph[node_id].AddValue(neighborId);

			g_good_count_data_graph[neighborId] = 1;
		}

		for (long long one = g_nodes_adj_list_start_index_data_graph[node_id]; one < g_nodes_adj_list_start_index_data_graph[node_id + 1]; one++) {
			long long neighborId = g_nodes_adj_list_with_edge_type_data_graph[one].node_id;

			for (long long two = g_nodes_adj_list_start_index_data_graph[neighborId]; two < g_nodes_adj_list_start_index_data_graph[neighborId + 1]; two++) {
				long long twoNeighbor = g_nodes_adj_list_with_edge_type_data_graph[two].node_id;
				// no visited before, two
				if (g_good_count_data_graph[twoNeighbor] == 0) {
					g_adj_list_two_hop_distance_data_graph[node_id].AddValue(twoNeighbor);
					
					g_good_count_data_graph[twoNeighbor] = 1;
				}
				
			}
		
		}

		memset(g_good_count_data_graph, 0, sizeof(int) * g_cnt_node_of_data_graph);
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void readAndProcessDataGraph(string data_graph_file) {
	vector<vector <EdgeType>> local_adj_list;
	local_adj_list.resize(g_cnt_node_of_data_graph);
	g_transfer_label_mapping = new int[g_largest_label_data_graph + 1];
	memset(g_transfer_label_mapping, 0, sizeof(int) * (g_largest_label_data_graph + 1));

	g_good_count_data_graph = new int[g_cnt_node_of_data_graph];
	memset(g_good_count_data_graph, 0, sizeof(int) * g_cnt_node_of_data_graph);
	
	int label_index = 1; // start label from 1, because 0 means not found the label
	int node_index = 0;

	// read data from file
	string line;
	vector<string> v;
	FILE *fp;
	char str[255];

	fp = fopen(data_graph_file.c_str(), "r");

	while (fgets(str, 255, fp) != NULL) {
		line = string(str);
		if (line.at(0) == 'v') {
			split(line, v, ' ');
			int label_from_file = atoi(v[2].c_str());

			int actual_label = g_transfer_label_mapping[label_from_file];
			
			if (actual_label == 0) {
				actual_label = label_index;
				g_transfer_label_mapping[label_from_file] = label_index;
				label_index++;
			}
			g_nodes_label_after_change_data_graph[node_index] = actual_label;
			node_index++;
			g_label_freqency_data_graph[actual_label]++;
		}

		if (line.at(0) == 'e') {
			split(line, v, ' ');
			int left_node = atoi(v[1].c_str());
			int right_node = atoi(v[2].c_str());

			//set<long long> edge_type_set = parse_edge_type_string(v[3]);
			//long long edge_type_set = atoll(v[3].c_str());
#ifdef NO_EDGE_TYPE
			edge_type_set = 1;
#endif
			if (left_node != right_node) {
				EdgeType edgeA(right_node, v[3], true);
				EdgeType edgeB(left_node, v[3], false);
				local_adj_list[left_node].push_back(edgeA);
				local_adj_list[right_node].push_back(edgeB);

				int left = left_node % SIZE_OF_EDGE_MATRIX;
				int right = right_node % SIZE_OF_EDGE_MATRIX;
				g_data_edge_matrix[left * SIZE_OF_EDGE_MATRIX + right] = 1;
				g_data_edge_matrix[right * SIZE_OF_EDGE_MATRIX + left] = 1;
			}

		}

	}
	fclose(fp);

	// build the degree and core number array
	g_core_number_data_graph = new int[g_cnt_node_of_data_graph];
	g_node_degree_data_graph = new int[g_cnt_node_of_data_graph];
	g_max_degree_data_graph = 0;
	long long sum_degree = 0;
	for (long long i = 0; i < g_cnt_node_of_data_graph; i++) {
		int degree = local_adj_list[i].size();
		g_node_degree_data_graph[i] = degree;
		g_core_number_data_graph[i] = degree;
		sum_degree += degree;
		if (degree > g_max_degree_data_graph) {
			g_max_degree_data_graph = degree;
		}
	}

	// build adj list
	//g_adj_list_data_graph.resize(g_cnt_node_of_data_graph);
	//for (int i = 0; i < local_adj_list.size(); i++) {
	//	g_adj_list_data_graph[i].insert(local_adj_list[i].begin(), local_adj_list[i].end());
	//}

	buildAdjListAndLabelPositionMatrix(local_adj_list, sum_degree);
	buildLabel2NodeID_only_used_in_seedfinder();

	initNEC();

	buildNLFCheck(); // don't use NLF in this algorithm


	// Use for speeding up at enumeration
	//======  hash table initialization ====================
	
	if (SIZEOK == 0) { //only create the hash tables if the cnt is larger than the size of edge matrix
		g_hash_table = new HashTable *[g_cnt_node_of_data_graph];
		for (long long i = 0; i < local_adj_list.size(); i++) {

			if (g_cnt_node_of_data_graph <= HT_LENGTH)
				g_hash_table[i] = new HashTable();
			else
				g_hash_table[i] = new HashTable(g_node_degree_data_graph[i]);

			for (long long j = 0; j < local_adj_list[i].size(); j++)
				g_hash_table[i]->AddValue(local_adj_list[i][j].node_id);
		}
	}
	
	//========================================================

#ifdef NLF_CHECK
	//================= build the NFL lightweight check =====================
	NLF_size = (g_cnt_unique_label_data_graph + 1) / SIZEOF_INT + 1;
	NLF_array_query = new long long[NLF_size]; // the array for query graph
	NLF_check = new long long[g_cnt_node_of_data_graph * NLF_size];
	memset(NLF_check, 0, sizeof(long long) * NLF_size * g_cnt_node_of_data_graph);
	for (long long i = 0; i < local_adj_list.size(); i++)
		// iterate node i's neighboor	
		for (long long j = 0; j < local_adj_list[i].size(); j++) {
			long long label = g_nodes_label_after_change_data_graph[local_adj_list[i][j].node_id];
			long long idx = NLF_size - 1 - label / SIZEOF_INT;
			long long pos = label % SIZEOF_INT;
			NLF_check[i  * NLF_size + idx] |= (1 << pos);
		}
	//=====================================================================================
#endif

	// use for CORE_AND_MAX_NB_FILTER
	//============== initialize the MAX neighbor degree of data node =====================
	MAX_NB_degree_data = new int[g_cnt_node_of_data_graph];
	for (long long i = 0; i < g_cnt_node_of_data_graph; i++) {
		long long max_degree = 0;
		for (long long j = 0; j < local_adj_list[i].size(); j++) {
			int node = local_adj_list[i][j].node_id;
			if (g_node_degree_data_graph[node] > max_degree)
				max_degree = g_node_degree_data_graph[node];
		}
		MAX_NB_degree_data[i] = max_degree;
	}
	//=====================================================================================


	//buildOneHopLabelCount();
	//buildTwoHopLabelCount();

	if (IS_ONE_HOP_DATA_GRAPH == 0) {
		addOneAndTwoHopNeighbor();
	}

	g_sim_visited = new char[g_cnt_node_of_data_graph];
	g_sim_visited_stack = new int[g_cnt_node_of_data_graph];
	memset(g_sim_visited, 0, sizeof(char) * g_cnt_node_of_data_graph);
	memset(g_sim_visited_stack, 0, sizeof(int) * g_cnt_node_of_data_graph);

	g_already_visited_data_graph = new char[g_cnt_node_of_data_graph];
	memset(g_already_visited_data_graph, 0, sizeof(char) * g_cnt_node_of_data_graph);
}



// this function is useless
inline void coreDecomposition_data() {
	//core-decomposition for the data graph
	//begin starting the core-decomposition, core number is the degree number

	long long *bin = new long long[g_max_degree_data_graph + 1];
	memset(bin, 0, sizeof(long long) * (g_max_degree_data_graph + 1));

	for (long long i = 0; i < g_cnt_node_of_data_graph; i++)
		bin[g_core_number_data_graph[i]] ++;

	long long start = 0;

	long long num;

	for (long long d = 0; d <= g_max_degree_data_graph; d++) {
		num = bin[d];
		bin[d] = start;
		start += num;
	}

	long long *pos = new long long[g_cnt_node_of_data_graph];
	long long *vert = new long long[g_cnt_node_of_data_graph];

	for (long long i = 0; i < g_cnt_node_of_data_graph; i++) {
		pos[i] = bin[g_core_number_data_graph[i]];
		vert[pos[i]] = i;
		bin[g_core_number_data_graph[i]] ++;
	}

	for (long long d = g_max_degree_data_graph; d > 0; d--)
		bin[d] = bin[d - 1];
	bin[0] = 0;

	for (long long i = 0; i < g_cnt_node_of_data_graph; i++) {

		long long v = vert[i];

		for (long long j = g_nodes_adj_list_start_index_data_graph[v]; j < g_nodes_adj_list_start_index_data_graph[v + 1]; j++) {

			long long u = g_nodes_adj_list_with_edge_type_data_graph[j].node_id;

			if (g_core_number_data_graph[u] > g_core_number_data_graph[v]) {

				long long du = g_core_number_data_graph[u];
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
				g_core_number_data_graph[u]--;
			}
		}
	}


	delete bin;
	delete pos;
	delete vert;
}


inline void initParametersBeforeQuery() {

#ifdef RESULT_ENUMERATION
	leaf_necs = new int[MAX_QUERY_NODE];
	NEC_Node_array = new NEC_Node[MAX_QUERY_NODE];
	all_mapping = new int[MAX_QUERY_NODE];
#endif

	for (int i = 0; i < g_cnt_unique_label_data_graph + 1; i++) {
		if (g_label_freqency_data_graph[i] > g_max_label_counter) {
			g_max_label_counter = g_label_freqency_data_graph[i];
		}
	}
	g_max_label_counter++;

	g_tree_node_parent_query_graph = new int[MAX_QUERY_NODE];

	g_good_count_need_clean_index_data_graph = new int[g_max_label_counter];
	memset(g_good_count_need_clean_index_data_graph, 0, sizeof(g_max_label_counter));
	g_good_count_need_clean_size = 0;

	for (long long i = 0; i < MAX_QUERY_NODE; i++) {
		indexSet[i].candidates = new int[g_max_label_counter];
		indexSet[i].path = new long[g_max_label_counter];
		indexSet[i].parent_cand_size = 0;
	}

	g_root_candidates = new int[g_max_label_counter];

	g_already_has_one_parent_data_node = new int[g_cnt_node_of_data_graph];
	memset(g_already_has_one_parent_data_node, -1, sizeof(int) * g_cnt_node_of_data_graph);
	g_index_array_for_indexSet = new CPICell[g_cnt_node_of_data_graph];
	

	NEC_mapping_Actual_first = new int[(g_cnt_unique_label_data_graph + 1) * MAX_QUERY_NODE]; //label and the parent node id
	memset(NEC_mapping_Actual_first, 0, sizeof(int) * (g_cnt_unique_label_data_graph + 1) * MAX_QUERY_NODE);

	memset(NEC_mapping_child_with_same_label_cnt, 0, sizeof(int) * (g_cnt_unique_label_data_graph + 1) * MAX_QUERY_NODE);

	INIT_MODULE_MATCHING_ORDER();

	// for backward prune
	v_cnt = new char[g_cnt_node_of_data_graph];
	memset(v_cnt, 0, sizeof(char) * g_cnt_node_of_data_graph);
}


inline void buildOneHopLabelCountQuery_used_by_sim() {

	if (g_one_hop_label_count_query_graph != NULL) {
		delete[] g_one_hop_label_count_query_graph;
	}

	g_one_hop_label_count_query_graph = new int[g_cnt_node_query_graph * g_cnt_unique_label_data_graph];
	memset(g_one_hop_label_count_query_graph, 0, sizeof(int) * g_cnt_node_query_graph * g_cnt_unique_label_data_graph);
	// iterate all data node
	for (int node_id = 0; node_id < g_cnt_node_query_graph; node_id++) {
		// count all label
		int begin = g_nodes_adj_list_start_index_query_graph[node_id];
		int end = g_nodes_adj_list_start_index_query_graph[node_id + 1];

		for (int i = begin; i < end; i++) {	
			int neighbor = g_nodes_adj_list_with_edge_type_query_graph[i].node_id;
			int label = g_nodes_label_query_graph[neighbor];
			g_one_hop_label_count_query_graph[node_id * g_cnt_unique_label_data_graph + label - 1]++;
		}
	}

}

/*
inline void buildTwoHopLabelCountQuery() {
	// label begin from 1

	if (g_two_hop_label_count_query_graph == NULL) {
		delete[] g_two_hop_label_count_query_graph;
	}


	g_set_of_node_adj_list_query_graph.resize(g_cnt_node_query_graph);

	g_two_hop_label_count_query_graph = new int[g_cnt_node_query_graph * g_cnt_unique_label_data_graph];
	memset(g_two_hop_label_count_query_graph, 0, sizeof(int) * g_cnt_node_query_graph * g_cnt_unique_label_data_graph);


	for (int node_id = 0; node_id < g_cnt_node_query_graph; node_id++) {
		for (long long j = g_nodes_adj_list_start_index_query_graph[node_id]; j < g_nodes_adj_list_start_index_query_graph[node_id + 1]; j++) {
			g_set_of_node_adj_list_query_graph[node_id].insert(g_nodes_adj_list_query_graph[j]);
		}
	}
	set<long long> * already_matched = new set<long long>[g_cnt_unique_label_data_graph];
	// iterate query graph node
	for (int node_id = 0; node_id < g_cnt_node_query_graph; node_id++) {
		// iterate all label
		for (long long label = 1; label <= g_cnt_unique_label_data_graph; label++) {
			already_matched[label - 1].clear();
			// iterate all two hop
			for (long long j = g_nodes_adj_list_start_index_query_graph[node_id]; j < g_nodes_adj_list_start_index_query_graph[node_id + 1]; j++) {
				long long neighborId = g_nodes_adj_list_query_graph[j];
				// two hop
				for (long long two = g_nodes_adj_list_start_index_query_graph[neighborId]; two < g_nodes_adj_list_start_index_query_graph[neighborId + 1]; two++) {
					long long two_neighbor = g_nodes_adj_list_query_graph[two];
					long long label = g_nodes_label_query_graph[two_neighbor];

					set<long long>::iterator iter = g_set_of_node_adj_list_query_graph[node_id].find(two_neighbor);

					if (two_neighbor != node_id && iter == g_set_of_node_adj_list_query_graph[node_id].end()) {
						already_matched[label - 1].insert(two_neighbor);
					}

				}
			}
		}

		for (int i = 0; i < g_cnt_unique_label_data_graph; i++) {
			g_two_hop_label_count_query_graph[node_id * g_cnt_unique_label_data_graph + i] = already_matched[i].size();
		}
	}

	int a = 0;
}

*/


inline void buildSimilarityMatrix() {

	g_node_similarity_matrix = new double[g_cnt_node_of_data_graph * g_cnt_node_query_graph];
	memset(g_node_similarity_matrix, 0, sizeof(double) * g_cnt_node_of_data_graph * g_cnt_node_query_graph);

	int sum = 0;
	int less_num = 0;

	if (g_cnt_unique_label_data_graph == 1) {
		for (int data_id = 0; data_id < g_cnt_node_of_data_graph; data_id++) {
			for (int query_id = 0; query_id < g_cnt_node_query_graph; query_id++) {

				less_num = 0;
				int data_degree = g_node_degree_data_graph[data_id];
				int query_degree = g_node_degree_query_graph[query_id];

				int diff = data_degree - query_degree;

				if (diff < 0) {
					less_num = -diff;
				}

				g_node_similarity_matrix[query_id * g_cnt_node_of_data_graph + data_id] = 1 - ((less_num * 1.0) / query_degree);
			}
		}
	}
	else {

		// if have multi label in graph, not use in out
		for (int data_id = 0; data_id < g_cnt_node_of_data_graph; data_id++) {
			for (int query_id = 0; query_id < g_cnt_node_query_graph; query_id++) {

				sum = 0;
				less_num = 0;
				int query_begin = query_id * g_cnt_unique_label_data_graph;
				// iterate each label
				for (int i = 0; i < g_cnt_unique_label_data_graph; i++) {
					sum += g_one_hop_label_count_query_graph[query_begin + i];
					int data_neighbor = g_node_id_label_position_matrix_data_graph[data_id * (g_cnt_unique_label_data_graph + 1) + i + 1].second;

					int diff = data_neighbor - g_one_hop_label_count_query_graph[query_begin + i];
					if (diff < 0) {
						less_num = less_num - diff;
					}
				}

				g_node_similarity_matrix[query_id * g_cnt_node_of_data_graph + data_id] = 1 - ((less_num * 1.0) / sum);

			}
		}

	}

	return;
}



inline void single_readQueryGraph(string query_graph_file) {

	// build adj list
	int adj_index = 0;
	vector<vector <EdgeType>> local_adj_list;

	string line;
	vector<string> v;
	char str[255];
	FILE *fp;

	fp = fopen(query_graph_file.c_str(), "r");

	while (fgets(str, 255, fp) != NULL) {
		line = string(str);
		if (line.at(0) == 't') {
			split(line, v, ' ');
			int query_node_num = atoi(v[2].c_str());
			local_adj_list.resize(query_node_num);
		}

		if (line.at(0) == 'v') {
			split(line, v, ' ');
			int node_index = atoi(v[1].c_str());
			int label_from_file = atoi(v[2].c_str());
			int actual_label = g_transfer_label_mapping[label_from_file];

			g_nodes_label_query_graph[node_index] = actual_label;
		}

		if (line.at(0) == 'e') {
			split(line, v, ' ');
			int left_node = atoi(v[1].c_str());
			int right_node = atoi(v[2].c_str());

			//set<long long> edge_type_set = parse_edge_type_string(v[3]);
			//long long edge_type_set = atoll(v[3].c_str());
#ifdef NO_EDGE_TYPE
			edge_type_set = 1;
#endif
			if (left_node != right_node) {
				EdgeType edgeA(right_node, v[3], true);
				EdgeType edgeB(left_node, v[3], false);
				local_adj_list[left_node].push_back(edgeA);
				local_adj_list[right_node].push_back(edgeB);
			}
		}
	}
	fclose(fp);

	g_cnt_node_query_graph = local_adj_list.size();

	long long sum_degree = 0;
	for (long long i = 0; i < local_adj_list.size(); i++) {
		int degree = local_adj_list[i].size();
		g_node_degree_query_graph[i] = degree;
		g_core_number_query_graph[i] = degree;
		sum_degree += degree;
		if (degree > MAX_DEGREE_A_NODE_QUERY) {
			MAX_DEGREE_A_NODE_QUERY = degree;

		}
	}
	// need sum degree
	g_max_sum_degree_query_graphs = sum_degree;

	// used for matching
	g_sum_degree_query_graph = sum_degree;
	g_core_tree_node_child_array.resize(g_sum_degree_query_graph);
	g_core_tree_node_nte_array.resize(g_sum_degree_query_graph);

	// build adj list
	g_nodes_adj_list_with_edge_type_query_graph.resize(sum_degree);
	long long cur_adj_start_index = 0;
	int adj_list_index = 0;
	for (int i = 0; i < local_adj_list.size(); i++) {
		if (local_adj_list[i].size() == 0) {
			g_nodes_adj_list_start_index_query_graph[i] = cur_adj_start_index;
			continue;
		}

		g_nodes_adj_list_start_index_query_graph[i] = cur_adj_start_index;
		for (int j = 0; j < local_adj_list[i].size(); j++) {
			g_nodes_adj_list_with_edge_type_query_graph[adj_list_index] = local_adj_list[i][j];
			adj_list_index++;
		}
		cur_adj_start_index += local_adj_list[i].size();
	}
	g_nodes_adj_list_start_index_query_graph[g_cnt_node_query_graph] = sum_degree;

	buildOneHopLabelCountQuery_used_by_sim();

	buildSimilarityMatrix();
}


inline void readQueryGraph(ifstream &fin_query) {

	int m;

	// if current query graph is bigger than before, resize data structure
	if (g_sum_degree_query_graph > g_max_sum_degree_query_graphs) {
		g_max_sum_degree_query_graphs = g_sum_degree_query_graph;
		g_nodes_adj_list_with_edge_type_query_graph.resize(g_sum_degree_query_graph);
		// used for matching
		g_core_tree_node_child_array.resize(g_sum_degree_query_graph);
		g_core_tree_node_nte_array.resize(g_sum_degree_query_graph);
	}

	// build adj list
	int adj_index = 0;
	for (long long i = 0; i < g_cnt_node_query_graph; i++) {

		g_nodes_adj_list_start_index_query_graph[i] = adj_index;
		
		fin_query >> m >> g_label_cur_node_query_graph >> g_degree_cur_node_query_graph;

		if (g_degree_cur_node_query_graph > MAX_DEGREE_A_NODE_QUERY) {
			MAX_DEGREE_A_NODE_QUERY = g_degree_cur_node_query_graph;
		}

		// build label, build degree, build core_number
		g_nodes_label_query_graph[i] = g_transfer_label_mapping[g_label_cur_node_query_graph];
		g_node_degree_query_graph[i] = g_degree_cur_node_query_graph;
		g_core_number_query_graph[i] = g_degree_cur_node_query_graph;
		// build adj list
		for (long long j = 0; j < g_degree_cur_node_query_graph; j++) {
			fin_query >> g_nodes_adj_list_with_edge_type_query_graph[adj_index].node_id;
			adj_index++;
			
		}
	}
	g_nodes_adj_list_start_index_query_graph[g_cnt_node_query_graph] = adj_index;

	buildOneHopLabelCountQuery_used_by_sim();

}


inline void initNECStructure() {
	NEC_mapping_child_with_same_label_cnt = new int[(g_cnt_unique_label_data_graph + 1) * MAX_QUERY_NODE]; //label and the parent node id
	memset(NEC_mapping_child_with_same_label_cnt, 0, sizeof(int) * (g_cnt_unique_label_data_graph + 1) * MAX_QUERY_NODE);

	NEC_mapping_pair = new NEC_element[(g_cnt_unique_label_data_graph + 1) * MAX_QUERY_NODE]; // <label, parent node id>
	NEC_set_array = new NEC_set_array_element[(g_cnt_unique_label_data_graph + 1) * MAX_QUERY_NODE]; // <parent_id, sum>
}


#endif // !FAST_DYNAMIC_PRE_PROCESS_DATA_GRAPH_H_

