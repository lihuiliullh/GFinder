#include "stdafx.h"
#include "matching_order.h"

long long temp_array_query_index = 0;
long long g_matching_order_size_of_core = 0;

long long g_query_node_id_to_matching_order_num[MAX_QUERY_NODE];
long long g_leaf_nodes_index = 0;
MatchOrderUnit g_matching_order_unit_of_query[MAX_QUERY_NODE];
long long nte_array_for_matching_unit[MAX_NTE_QUERY];

// this variable will be used only in this file
long long sum_nontree_edge_of_every_node[MAX_QUERY_NODE];
long long leaf_nodes[MAX_QUERY_NODE];
long long stack_array_query[MAX_QUERY_NODE];
long long temp_array_query[MAX_QUERY_NODE];
long sum_path[MAX_QUERY_NODE];

double * path_acuu;
double * path_temp;

pair<long long, double> leaf_path_info[MAX_QUERY_NODE];
vector<long long> temp_vector;