#pragma once
#include "stdafx.h"
#include "global_variable.h"


long long g_max_label_counter = 0;
int g_cnt_node_of_data_graph = 0;
int g_cnt_unique_label_data_graph = 0;
long long g_cnt_unique_label_data_graph_actual = 0;

int g_largest_label_data_graph = 0;
long long MAX_DEGREE_A_NODE_QUERY = 0;
vector<int> g_degree_of_sort_by_label_degree_data_graph(0);

int * g_nodes_label_after_change_data_graph;
int * g_label_freqency_data_graph;
int * g_transfer_label_mapping;
int * g_core_number_data_graph;
int * g_node_degree_data_graph;
EdgeType * g_nodes_adj_list_with_edge_type_data_graph;
int size_of_g_nodes_adj_list_with_edge_type_data_graph;
int * g_nodes_adj_list_start_index_data_graph;

// node_id * g_cnt_unique_label_data_graph + label_id = <start, end>
pair<int, int> * g_node_id_label_position_matrix_data_graph;
int * g_node_id_sort_by_label_degree_data_graph;
pair<int, int> * g_label_to_sort_by_degree_node_data_graph;

// time
double g_time_search;
double g_time_process;
double g_time_simulation;
double g_time_prepare;
double g_time_preliminary;
double g_time_readQuery;
double g_time_getSequence;
double g_time_total;
double g_time_rest;
double g_time_query;

long LIMIT;

int g_cnt_node_query_graph;
long long g_sum_degree_query_graph;
double g_mapping_found;

long long g_max_sum_degree_query_graphs;
vector<EdgeType> g_nodes_adj_list_with_edge_type_query_graph;
int g_nodes_adj_list_start_index_query_graph[MAX_QUERY_NODE];
long long g_label_cur_node_query_graph;
long long g_degree_cur_node_query_graph;

int g_nodes_label_query_graph[MAX_QUERY_NODE];
long long g_node_degree_query_graph[MAX_QUERY_NODE];
long long g_core_number_query_graph[MAX_QUERY_NODE];


char g_visited_for_query_graph[MAX_QUERY_NODE];


int * NEC_mapping_child_with_same_label_cnt;
NEC_element * NEC_mapping_pair;
NEC_set_array_element * NEC_set_array;
NEC_Node * NEC_Node_array;
long long NEC_map[MAX_QUERY_NODE];
int * NEC_mapping_Actual_first;

long long dfs_stack_query[MAX_QUERY_NODE]; // let algorithm run fast
long long g_residual_tree_match_seq_index = 0;
long long g_residual_tree_leaf_node_index;
long long g_residual_tree_match_seq_query[MAX_QUERY_NODE];
int * g_tree_node_parent_query_graph;
// storeage the leaf nodeid in query graph
pair <long long, double> g_residual_tree_leaf_node[MAX_QUERY_NODE];
long long NEC_set_index;
vector<pair<long long, long long> > NEC_set_by_label_index;

long long g_root_node_id_of_query;
CPINode indexSet[MAX_QUERY_NODE];
long long * NLF_array_query;
long long NLF_size = -1;
long long * NLF_check;
int SIZEOF_INT = sizeof(int) * 8;
int * g_root_candidates;
int g_root_candidates_size = 0;

long long g_nte_array_for_matching_unit_index = 0;
long long g_matching_sequence_index = 0;

// 2 hop pruning
int * g_good_count_data_graph;
int * g_good_count_need_clean_index_data_graph;
long long g_good_count_need_clean_size;
long long g_check_value = 0;
vector<long long> g_forward_build_sequence;
long long g_forward_build_parent[MAX_QUERY_NODE];
int g_forward_level[MAX_QUERY_NODE];

//vector<set <long long>> g_adj_list_data_graph;
long long g_build_cpi_sequence[MAX_QUERY_NODE];
long long g_build_cpi_sequence_size;
int * g_already_has_one_parent_data_node;

CPICell * g_index_array_for_indexSet;
long long count_index_array_for_indexSet;

CoreQueryBFSTreeNode core_query_tree[MAX_QUERY_NODE];
long long g_visited_int_for_query_graph[MAX_QUERY_NODE];
vector <long long> g_core_tree_node_child_array;
vector <long long> g_core_tree_node_nte_array;

long long g_core_tree_node_child_array_index = 0;
long long g_core_tree_node_nte_array_index = 0;
long long g_actual_mapping[MAX_QUERY_NODE];
long long g_cand_pos_in_indexset[MAX_QUERY_NODE];
long long g_core_size = 0;


//for enumerating the result of leaf nodes ====
int * leaf_necs;
long long leaf_necs_idx = 0;
int * all_mapping;

long long NEC_leaf_mapping_pair_index = 0;

bool g_isTree;

char * g_data_edge_matrix;
char SIZEOK;
int SIZE_OF_EDGE_MATRIX = 10000;
HashTable ** g_hash_table;

vector<long long>g_level[MAX_QUERY_NODE + 1];
long long g_level_size = 0;
char * v_cnt;

int * MAX_NB_degree_data;

long long g_BFS_sequence_length = 0;
double found_mapping_enumeration = 10;
int * g_one_hop_label_count_data_graph;
int * g_two_hop_label_count_data_graph;
int * g_one_hop_label_count_query_graph = NULL;
int * g_two_hop_label_count_query_graph = NULL;
vector<set<long long>> g_set_of_node_adj_list_query_graph;
vector < pair<long long, long long> > level_index;

long long BFS_parent_query[MAX_QUERY_NODE];
long long BFS_level_query[MAX_QUERY_NODE];

//vector<set <int>> g_adj_list_one_to_four_data_graph;
HashTable * g_adj_list_one_hop_distance_data_graph;
HashTable * g_adj_list_two_hop_distance_data_graph;

char * g_sim_visited;
int * g_sim_visited_stack;
long long g_sim_visited_length = 0;


int TOPK = 10;
int NODE_MISS_COST = 3;
int BRIDGE_COST = 10;
int EDGE_MISSING_COST = 10;
int IS_ONE_HOP_DATA_GRAPH = 0;
int DISTINCT_LABEL = 0;
long long g_maxPartialNum = 0;

int g_max_degree_data_graph = 0;

bool DYNAMIC = false;

vector<long long> query_root_sort_list;
long long ONE_M = 100000;

clock_t g_clock;

char * g_already_visited_data_graph;
double * g_node_similarity_matrix;

int * g_edge_matching_array;
bool g_is_init_edge_matchinig_array = false;
//=============================================

