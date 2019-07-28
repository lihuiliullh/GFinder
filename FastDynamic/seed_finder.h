#pragma once
#ifndef FAST_DYNAMIC_SEED_FINDER_H_
#define FAST_DYNAMIC_SEED_FINDER_H_

#include <cfloat>
#include <algorithm>
#include "global_variable.h"
#include "data_structure.h"
#include <functional>
#include <math.h>

using namespace std;

inline long long selectRootFromQuery() {

	double least_ranking = DBL_MAX;
	long long start_node = -1;
	double ranking;
	long long label;
	long long degree;

	for (long long i = 0; i < g_cnt_node_query_graph; i++) {

		if (g_core_number_query_graph[i] < 2)	//root node must be selected from the core structure
			continue;

		label = g_nodes_label_query_graph[i];
		degree = g_node_degree_query_graph[i];

		//binary search used here
		long long s = g_label_to_sort_by_degree_node_data_graph[label - 1].second;
		long long end = g_label_to_sort_by_degree_node_data_graph[label].second;
		vector<int>::iterator pos = lower_bound(g_degree_of_sort_by_label_degree_data_graph.begin() + s, g_degree_of_sort_by_label_degree_data_graph.begin() + end, degree);
		long long start = pos - g_degree_of_sort_by_label_degree_data_graph.begin();

		// neighbor / degree
		ranking = (double)(end - start) / (double)degree;

		
		query_root_sort_list.push_back(floor(ranking * ONE_M) * ONE_M + i);

		if (ranking < least_ranking) {
			least_ranking = ranking;
			start_node = i;
		}
	}
	sort(query_root_sort_list.begin(), query_root_sort_list.end(), greater<long long>());
	return start_node;
}

inline long long centerPiece() {}


inline boolean checkCandidate(long long cand_id, long long query_id) {
	double a = g_node_similarity_matrix[query_id * g_cnt_node_of_data_graph + cand_id];
	if (a >= MIN_SIM) {
		return true;
	}
	else {
		return false;
	}
}


inline void findRootCandidate_() {
	long long label = g_nodes_label_query_graph[g_root_node_id_of_query];
	long long degree = g_node_degree_query_graph[g_root_node_id_of_query];
	long long max_nb_degree_query = 0;
	long long core = g_core_number_query_graph[g_root_node_id_of_query];

	long long first = g_nodes_adj_list_start_index_query_graph[g_root_node_id_of_query];
	memset(NLF_array_query, 0, sizeof(long long) * NLF_size);

	// generate neighbor label filter by query graph
	for (long long j = first; j < first + degree; j++) {
		long long local_label = g_nodes_label_query_graph[g_nodes_adj_list_with_edge_type_query_graph[j].node_id];
		long long idx = NLF_size - 1 - local_label / SIZEOF_INT;
		long long pos = local_label % SIZEOF_INT;
		NLF_array_query[idx] |= (1 << pos);
		long long nb_degree = g_node_degree_query_graph[g_nodes_adj_list_with_edge_type_query_graph[j].node_id];
		if (nb_degree > max_nb_degree_query) //find the max neighbor degree
			max_nb_degree_query = nb_degree;
	}
	//============== check each candidate =======================
	long long count_global_temp_array_1 = 0;
	long long s = g_label_to_sort_by_degree_node_data_graph[label - 1].second;
	long long end = g_label_to_sort_by_degree_node_data_graph[label].second;
	vector<int>::iterator pos = lower_bound(g_degree_of_sort_by_label_degree_data_graph.begin() + s, g_degree_of_sort_by_label_degree_data_graph.begin() + end, degree);
	long long start = pos - g_degree_of_sort_by_label_degree_data_graph.begin();
	for (long long j = start; j < end; j++) {
		long long cand_id = g_node_id_sort_by_label_degree_data_graph[j];

		if (g_core_number_data_graph[cand_id] < core || max_nb_degree_query > MAX_NB_degree_data[cand_id])
			continue;

		char flag_add = 1;
		for (long long pos = NLF_size - 1; pos >= 0; pos--) {
			if (NLF_check[cand_id * NLF_size + pos] != (NLF_array_query[pos] | NLF_check[cand_id * NLF_size + pos])) {
				flag_add = 0;
				break;
			}
		}
		if (flag_add) {
			//root_node_unit.candidates[count_global_temp_array_1++] = can_id;
			g_root_candidates[count_global_temp_array_1++] = cand_id;
		}
	}
	g_root_candidates_size = count_global_temp_array_1;
}


inline void findRootCandidate() {
	//================= First step: deal with the ROOT node ===============================

	long long label = g_nodes_label_query_graph[g_root_node_id_of_query];
	long long degree = g_node_degree_query_graph[g_root_node_id_of_query];

#ifdef CORE_AND_MAX_NB_FILTER
	long long max_nb_degree_query = 0;
	long long core = g_core_number_query_graph[g_root_node_id_of_query];
#endif
	//============== generate the neighborhood label array =======================
	// first is the first neighborhood node id
	long long first = g_nodes_adj_list_start_index_query_graph[g_root_node_id_of_query];
	memset(NLF_array_query, 0, sizeof(long long) * NLF_size);

	// generate neighbor label filter by query graph
	for (long long j = first; j < first + degree; j++) {
		long long local_label = g_nodes_label_query_graph[g_nodes_adj_list_with_edge_type_query_graph[j].node_id];
		long long idx = NLF_size - 1 - local_label / SIZEOF_INT;
		long long pos = local_label % SIZEOF_INT;
		NLF_array_query[idx] |= (1 << pos);

#ifdef CORE_AND_MAX_NB_FILTER
		long long nb_degree = g_node_degree_query_graph[g_nodes_adj_list_with_edge_type_query_graph[j].node_id];
		if (nb_degree > max_nb_degree_query) //find the max neighbor degree
			max_nb_degree_query = nb_degree;
#endif


	}
	//============== check each candidate =======================
	long long count_global_temp_array_1 = 0;
	long long s = g_label_to_sort_by_degree_node_data_graph[label - 1].second;
	long long end = g_label_to_sort_by_degree_node_data_graph[label].second;
	vector<int>::iterator pos = lower_bound(g_degree_of_sort_by_label_degree_data_graph.begin() + s, g_degree_of_sort_by_label_degree_data_graph.begin() + end, degree);
	long long start = pos - g_degree_of_sort_by_label_degree_data_graph.begin();
	for (long long j = start; j < end; j++) {
		long long cand_id = g_node_id_sort_by_label_degree_data_graph[j];

#ifdef CORE_AND_MAX_NB_FILTER
		if (g_core_number_data_graph[cand_id] < core || max_nb_degree_query > MAX_NB_degree_data[cand_id])
			continue;
#endif

#ifdef NLF_CHECK
		char flag_add = 1;
		for (long long pos = NLF_size - 1; pos >= 0; pos--) {
			if (NLF_check[cand_id * NLF_size + pos] != (NLF_array_query[pos] | NLF_check[cand_id * NLF_size + pos])) {
				flag_add = 0;
				break;
			}
		}
#endif

		// check candidate
		boolean isGood = checkCandidate(cand_id, g_root_node_id_of_query);
		if (!isGood) {
			continue;
		}

#ifdef NLF_M
		if (flag_add) {
			//root_node_unit.candidates[count_global_temp_array_1++] = can_id;
			g_root_candidates[count_global_temp_array_1++] = cand_id;
		}
#else
		g_root_candidates[count_global_temp_array_1++] = cand_id;
#endif
		//g_root_candidates[count_global_temp_array_1++] = cand_id;
	}
	g_root_candidates_size = count_global_temp_array_1;

}


#endif // !FAST_DYNAMIC_SEED_FINDER_H_


