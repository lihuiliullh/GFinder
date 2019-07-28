#pragma once

#ifndef FAST_DYNAMIC_GLOBAL_VARIABLE_
#define FAST_DYNAMIC_GLOBAL_VARIABLE_

#include"stdafx.h"
#include <set>
#include <map>
#include <time.h>
#include "vector"
#include "data_structure.h"

#define RESULT_ENUMERATION
#define EXACT

#define MIN_SIM 1
#define MISSING_EDGE_THRESHOLD_VALUE 0
#define MAX_LABEL 20
#define MAX_TIME 30000

//#define CORE_AND_MAX_NB_FILTER
#define PARTIAL_MATCHING
//#define NLF_CHECK
//#define NLF_M

using namespace std;

#define MAX_QUERY_NODE 944
//#define NO_EDGE_TYPE


extern int IS_ONE_HOP_DATA_GRAPH;
extern int TOPK;
extern int NODE_MISS_COST;
extern int BRIDGE_COST;
extern int EDGE_MISSING_COST;

extern int g_cnt_node_of_data_graph;
extern int * g_nodes_label_after_change_data_graph;

extern int g_cnt_unique_label_data_graph;

extern int * g_label_freqency_data_graph;
extern int * g_transfer_label_mapping;
extern int g_largest_label_data_graph;
extern int * g_core_number_data_graph;
extern int * g_node_degree_data_graph;
extern int g_max_degree_data_graph;
extern EdgeType * g_nodes_adj_list_with_edge_type_data_graph; 
extern int size_of_g_nodes_adj_list_with_edge_type_data_graph;
extern int * g_nodes_adj_list_start_index_data_graph;

// node_id * g_cnt_unique_label_data_graph + label_id = <start, end>
extern pair<int, int> * g_node_id_label_position_matrix_data_graph; 
extern int * g_node_id_sort_by_label_degree_data_graph;
extern pair<int, int> * g_label_to_sort_by_degree_node_data_graph;
extern vector<int> g_degree_of_sort_by_label_degree_data_graph;
extern long long g_max_label_counter;


// time
extern double g_time_search;
extern double g_time_process;
extern double g_time_simulation;
extern double g_time_prepare;
extern double g_time_preliminary;
extern double g_time_readQuery;
extern double g_time_getSequence;
extern double g_time_total;
extern double g_time_rest;
extern double g_time_query;

extern long LIMIT;

extern int g_cnt_node_query_graph;
extern long long g_sum_degree_query_graph;
extern double g_mapping_found;

extern long long MAX_DEGREE_A_NODE_QUERY;
extern long long g_max_sum_degree_query_graphs;
extern vector<EdgeType> g_nodes_adj_list_with_edge_type_query_graph;
extern int g_nodes_adj_list_start_index_query_graph[MAX_QUERY_NODE];
extern long long g_label_cur_node_query_graph;
extern long long g_degree_cur_node_query_graph;

extern int g_nodes_label_query_graph[MAX_QUERY_NODE];
extern long long g_node_degree_query_graph[MAX_QUERY_NODE];
extern long long g_core_number_query_graph[MAX_QUERY_NODE];

extern char g_visited_for_query_graph[MAX_QUERY_NODE];


extern int * NEC_mapping_child_with_same_label_cnt;
extern NEC_element * NEC_mapping_pair;
extern NEC_set_array_element * NEC_set_array;
extern NEC_Node * NEC_Node_array;
extern long long NEC_map[MAX_QUERY_NODE];
extern int * NEC_mapping_Actual_first;

extern long long dfs_stack_query[MAX_QUERY_NODE]; // let algorithm run fast
extern long long g_residual_tree_match_seq_index;
extern long long g_residual_tree_leaf_node_index;
extern long long g_residual_tree_match_seq_query[MAX_QUERY_NODE];
extern int * g_tree_node_parent_query_graph;
// storeage the leaf nodeid in query graph
extern pair <long long, double> g_residual_tree_leaf_node[MAX_QUERY_NODE];
extern long long NEC_set_index;
extern vector<pair<long long, long long> > NEC_set_by_label_index; //include a redunant element to set the end

extern long long g_root_node_id_of_query;
extern CPINode indexSet[MAX_QUERY_NODE];
extern long long * NLF_array_query; //for query
extern long long NLF_size;
extern long long * NLF_check;
extern int SIZEOF_INT;
extern int * g_root_candidates;
extern int g_root_candidates_size;

extern long long g_nte_array_for_matching_unit_index;
extern long long g_matching_sequence_index;

// 2 hop pruning
extern int * g_good_count_data_graph;
extern int * g_good_count_need_clean_index_data_graph;
extern long long g_good_count_need_clean_size;
extern long long g_check_value;
extern vector<long long> g_forward_build_sequence;
extern long long g_forward_build_parent[MAX_QUERY_NODE];
extern int g_forward_level[MAX_QUERY_NODE];

//extern vector<set <long long>> g_adj_list_data_graph;
extern long long g_build_cpi_sequence[MAX_QUERY_NODE];
extern long long g_build_cpi_sequence_size;
extern int * g_already_has_one_parent_data_node;

extern CPICell * g_index_array_for_indexSet;
extern long long count_index_array_for_indexSet;

extern CoreQueryBFSTreeNode core_query_tree[MAX_QUERY_NODE];
extern long long g_visited_int_for_query_graph[MAX_QUERY_NODE];
extern vector <long long> g_core_tree_node_child_array;
extern vector <long long> g_core_tree_node_nte_array;
extern long long g_core_tree_node_child_array_index;
extern long long g_core_tree_node_nte_array_index;
extern long long g_actual_mapping[MAX_QUERY_NODE];
extern long long g_cand_pos_in_indexset[MAX_QUERY_NODE];
extern long long g_core_size;

//for enumerating the result of leaf nodes ====
extern int * leaf_necs;
extern long long leaf_necs_idx;
extern int * all_mapping;


extern long long NEC_leaf_mapping_pair_index;

extern bool g_isTree;

extern char * g_data_edge_matrix;
extern char SIZEOK;
extern int SIZE_OF_EDGE_MATRIX;
extern HashTable ** g_hash_table;


extern vector<long long>g_level[MAX_QUERY_NODE + 1];
extern long long g_level_size;
extern char * v_cnt;  // it is the same as g_good_count_data_graph

extern int * MAX_NB_degree_data;

extern long long g_BFS_sequence_length;

extern double found_mapping_enumeration;

extern int * g_one_hop_label_count_data_graph;
extern int * g_two_hop_label_count_data_graph;
extern int * g_one_hop_label_count_query_graph;
extern int * g_two_hop_label_count_query_graph;
extern vector<set<long long>> g_set_of_node_adj_list_query_graph;
extern vector < pair<long long, long long> > level_index;

extern long long BFS_parent_query[MAX_QUERY_NODE];
extern long long BFS_level_query[MAX_QUERY_NODE];

//extern vector<set <int>> g_adj_list_one_to_four_data_graph;
extern HashTable * g_adj_list_one_hop_distance_data_graph;
extern HashTable * g_adj_list_two_hop_distance_data_graph;


extern char * g_sim_visited;
extern int * g_sim_visited_stack;
extern long long g_sim_visited_length;
extern int DISTINCT_LABEL;
extern long long g_maxPartialNum;
extern bool DYNAMIC;


extern vector<long long> query_root_sort_list;
extern long long ONE_M;
extern clock_t g_clock;

extern char * g_already_visited_data_graph;
extern double * g_node_similarity_matrix;

extern int * g_edge_matching_array;
extern bool g_is_init_edge_matchinig_array;
//=============================================

#endif // !FAST_DYNAMIC_GLOBAL_VARIABLE_

