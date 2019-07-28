#pragma once

#ifndef FAST_DYNAMIC_ENUMERATION_
#define FAST_DYNAMIC_ENUMERATION_

#include <algorithm>
#include "global_variable.h"
#include "matching_order.h"


extern SearchIterator search_iterator[MAX_QUERY_NODE];
extern long long actual_mapping_tree[MAX_QUERY_NODE];

extern SearchIterator search_unit_tree_iterator[MAX_QUERY_NODE];
extern pair<long long, pair<long long, double> >  NEC_set_ranking[MAX_QUERY_NODE];
extern pair <long long, long long>  v_nec_count[MAX_QUERY_NODE]; // used in the map special tree function
extern long long nec_count_set_size;
extern long long  nec_count_set[MAX_QUERY_NODE];

extern long long m_already_mapped;
extern long long m_back_trace;


inline bool sort_by_second_double(pair<long long, pair<long long, double> > a,
	pair<long long, pair<long long, double> > b) {
	return (a.second.second < b.second.second);
}

inline bool sort_by_second_element(pair<long long, long long> p1, pair<long long, long long> p2) {
	return (p1.second < p2.second);
}





inline void finishCoreAndNoForest(long long & already_matched_size) {

	for (long long i = 0; i < g_matching_order_size_of_core; i++) //for core
		all_mapping[g_matching_order_unit_of_query[i].node] = g_actual_mapping[i];

#ifdef xxxxx

	// find a core matching
	cout << "find one: core size " << g_core_size << endl;
	for (int k = 0; k < already_matched_size; k++) {
		cout << g_actual_mapping[k] << " ";
	}
	cout << endl;

#endif // 

	cout << "Mapping " << " => ";
	for (long long i = 0; i < g_cnt_node_query_graph; i++)
		cout << i << ":" << all_mapping[i] << " ";
	cout << endl;
	
}

inline void findAllMatching() {

	MatchOrderUnit * unit;
	SearchIterator * temp_search_unit;
	CPINode * index_unit_current;
	int * already_mapping_flag_data = g_good_count_data_graph;

	long long current_id_query;
	long long parent_id;

	for (int i = 0; i < indexSet[g_root_node_id_of_query].size; i++) {
		long long root_cand_id_data = indexSet[g_root_node_id_of_query].candidates[i];
		if (root_cand_id_data == -1) {
			continue;
		}
		// dynamic may generate partial match
		// core-size is different from forward sequence length
		if (g_core_size == 1) {

		}

		// reuse g_good_count_data_graph
		already_mapping_flag_data[root_cand_id_data] = 1;
		g_actual_mapping[0] = root_cand_id_data;
		g_cand_pos_in_indexset[g_root_node_id_of_query] = i;


		char back_trace = 0;
		long long already_matched_size = 1;
		while (true) {
			if (already_matched_size == 0) break; // no results
			if (already_matched_size == g_core_size) {

				if (g_residual_tree_match_seq_index == 0) {
					// no forest

					finishCoreAndNoForest(already_matched_size);
				}
				else {
					//getResidualTreeMapping_Enumeration(found_mapping_enumeration, already_mapping_flag_data);
				}

				// 
				already_mapping_flag_data[g_actual_mapping[already_matched_size - 1]] = 0;
				already_matched_size--;
				continue;
			}


			unit = &g_matching_order_unit_of_query[already_matched_size];

			current_id_query = unit->node;

			parent_id = g_matching_order_unit_of_query[unit->pt_index].node;
			index_unit_current = &indexSet[current_id_query];

			// temp_search_unit not init before
			temp_search_unit = &search_iterator[already_matched_size];

			if (temp_search_unit->address == NULL) {
				long long parent_pos_index = g_cand_pos_in_indexset[parent_id];
				temp_search_unit->address = index_unit_current->index_N_up_u[parent_pos_index];
				temp_search_unit->address_size = index_unit_current->size_of_index[parent_pos_index];

				if (temp_search_unit->address_size == 0) {
					temp_search_unit->address = NULL;
					already_matched_size--;
					if (already_matched_size != 0) {
						already_mapping_flag_data[g_actual_mapping[already_matched_size]] = 0;
					}
					continue;
				}
				temp_search_unit->address_pos = 0;
			}
			else {
				temp_search_unit->address_pos++;
				if (temp_search_unit->address_pos == temp_search_unit->address_size) {
					temp_search_unit->address = NULL;
					already_matched_size--;
					already_mapping_flag_data[g_actual_mapping[already_matched_size]] = 0;
					continue;
				}
			}
						
			back_trace = 0;

			while (true) {
				long long pos = index_unit_current->index_N_up_u[g_cand_pos_in_indexset[parent_id]][temp_search_unit->address_pos].index;
				long long data_id_current = index_unit_current->candidates[pos];

				char mapping_ok = 1;

				temp_search_unit->node_id = data_id_current;
				temp_search_unit->query_id = current_id_query;

				if (already_mapping_flag_data[data_id_current]) {
					m_already_mapped++;
				}

				if (!already_mapping_flag_data[data_id_current]) {
					//CoreQueryTreeNode *unit = &core_query_tree[current_id_query];
					if (unit->nte_length) {

						for (int j = unit->start_pos; j < unit->start_pos + unit->nte_length; j++) {


#ifdef EXACT
							if (SIZEOK == 1) {
								if (g_data_edge_matrix[data_id_current * SIZE_OF_EDGE_MATRIX + g_actual_mapping[nte_array_for_matching_unit[j]]] == 0) {
									mapping_ok = 0;
									break;
								}
							}
							else {
								if (g_data_edge_matrix[(data_id_current % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (g_actual_mapping[nte_array_for_matching_unit[j]] % SIZE_OF_EDGE_MATRIX)] == 0) {
									mapping_ok = 0;
									break;
								}
								else {
									//validate the non tree edge by querying the edgeIndex
									if (!g_hash_table[data_id_current]->query(g_actual_mapping[nte_array_for_matching_unit[j]])) {
										mapping_ok = 0;
										break;
									}
								}
							}
#else

							// check whether it is good
							long long a = nte_array_for_matching_unit[j];
							long long b = g_actual_mapping[nte_array_for_matching_unit[j]];
							long long to_id_query = g_core_tree_node_nte_array[j];
							int t_pos = g_cand_pos_in_indexset[to_id_query];
							long long to_id_data = indexSet[to_id_query].candidates[t_pos];
							// check whether connect
							to_id_data = g_actual_mapping[nte_array_for_matching_unit[j]];
							set<long long>::iterator it1 = g_adj_list_data_graph[data_id_current].find(to_id_data);
							if (it1 == g_adj_list_data_graph[data_id_current].end()) {
								mapping_ok = 0;
								break;
							}
#endif

						}
					}
				}
				else {
					mapping_ok = 0;
				}

				if (mapping_ok) {
					g_actual_mapping[already_matched_size] = data_id_current;
					g_cand_pos_in_indexset[current_id_query] = pos;
					already_mapping_flag_data[data_id_current] = 1;
					break;
				}
				else {
					// no mapping
					temp_search_unit->address_pos++;
					if (temp_search_unit->address_pos == temp_search_unit->address_size) {
						back_trace = 1;
						break;
					}
				}
			}

			if (back_trace) {

				m_back_trace++;

				back_trace = 0;
				temp_search_unit->address = NULL;
				already_matched_size--;
				already_mapping_flag_data[g_actual_mapping[already_matched_size]] = 0;
			}
			else {
				already_matched_size++;
			}
		}
		already_mapping_flag_data[root_cand_id_data] = 0;
	}

}




#endif