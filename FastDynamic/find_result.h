#pragma once

#ifndef DYNAMIC_FAST_FIND_RESULT_H_
#define DYNAMIC_FAST_FIND_RESULT_H_

#include "enumeration.h"

extern vector<Result> g_result_heap;
extern int g_max_cost_in_heap;
#define MAX_BACK_TRACE 30000000

inline void updateMaxHeap(int cost, vector<long long> & result) {
	Result n;
	n.cost = cost;
	n.result = result;

	// delet largest
	pop_heap(g_result_heap.begin(), g_result_heap.end(), less<Result>());
	g_result_heap.pop_back();
	// add new
	g_result_heap.push_back(n);
	push_heap(g_result_heap.begin(), g_result_heap.end(), less<Result>());

}

inline void justPutInHeap(int cost, vector<long long> & result) {

	Result n;
	n.cost = cost;
	n.result = result;

	g_result_heap.push_back(n);
	push_heap(g_result_heap.begin(), g_result_heap.end(), less<Result>());
}

// a little time consuming, try to improve it
inline void checkNow(int already_matched_size, vector<int> & cost) {

	int totalCost = 0;
	for (int i = 0; i < cost.size(); i++) {
		totalCost += cost[i];
	}

	totalCost += (g_cnt_node_query_graph - already_matched_size) * NODE_MISS_COST;

	vector<long long> res;
	if (g_result_heap.size() < TOPK) {

		for (int i = 0; i < already_matched_size; i++) {
			all_mapping[g_matching_order_unit_of_query[i].node] = g_actual_mapping[i];
		}

		//for (long long i = already_matched_size; i < g_cnt_node_query_graph; i++) //for core
		//	all_mapping[g_matching_order_unit_of_query[i].node] = -1;

		for (long long i = 0; i < g_cnt_node_query_graph; i++)
			res.push_back(all_mapping[i]);

		justPutInHeap(totalCost, res);

		g_max_cost_in_heap = g_result_heap[0].cost;
	}
	else if (totalCost < g_result_heap[0].cost) {

		for (int i = 0; i < already_matched_size; i++) {
			all_mapping[g_matching_order_unit_of_query[i].node] = g_actual_mapping[i];
		}

		//for (long long i = already_matched_size; i < g_cnt_node_query_graph; i++) //for core
		//	all_mapping[g_matching_order_unit_of_query[i].node] = -1;

		for (long long i = 0; i < g_cnt_node_query_graph; i++)
			res.push_back(all_mapping[i]);

		updateMaxHeap(totalCost, res);
		g_max_cost_in_heap = g_result_heap[0].cost;
	}

}






inline double LeafMappingEnumeration(double & found_mapping_enumeration, int * already_mapping_flag_data) {

	/*
	* This function only enumerates all the results. Not tuned for performance at all.
	*/

	//step one: get all actual nodes in leaf
	//step two: get the candidates for all nodes.
	//step three: enumerate results until reaching the limit

	//sort the LLC sets
	long long nec_set_size = NEC_set_by_label_index.size() - 1;// -1 is because the last element is actually redunant
	for (long long i = 0; i < nec_set_size; i++) {
		long long cand_sum = 0;
		long long node_sum = 0;
		long long start = NEC_set_by_label_index[i].second;
		long long end = NEC_set_by_label_index[i + 1].second;
		for (long long j = start; j < end; j++) {
			long long parent_id = NEC_set_array[j].parent_id;
			long long sum = NEC_set_array[j].sum;
			long long represent_node = NEC_set_array[j].represent_node;
			long long parent_pos = g_cand_pos_in_indexset[parent_id];
			CPINode & unit = indexSet[represent_node];
			for (long long it = 0; it < unit.size_of_index[parent_pos]; it++) {
				long long can = unit.candidates[unit.index_N_up_u[parent_pos][it].index];
				if (!already_mapping_flag_data[can])
					cand_sum++;
			}
			node_sum += sum; //moved up
		}
		if (cand_sum < node_sum)
			return 0;
		NEC_set_ranking[i] = make_pair(i, make_pair(node_sum, (double)cand_sum / (double)node_sum));
	}
	sort(NEC_set_ranking, NEC_set_ranking + nec_set_size, sort_by_second_double);
	//=================================================================================

	//============= get all leaf nodes and their candidates
	leaf_necs_idx = 0;
	vector <long long> leaf_cands;
	vector < pair<long long, long long> > leaf_cands_info;

	for (long long i = 0; i < nec_set_size; i++) {

		long long label_index = NEC_set_ranking[i].first;
		long long node_sum = NEC_set_ranking[i].second.first;
		double count_local_mapping_label = 0;

		if (node_sum == 1) {//==== CASE ONE : there is only one node in this nec set with this label =====

			long long start = NEC_set_by_label_index[label_index].second;
			long long represent_node = NEC_set_array[start].represent_node;

			long long start_l = leaf_cands.size();

			CPINode & unit = indexSet[represent_node];
			long long parent_id = NEC_set_array[start].parent_id;
			long long parent_pos = g_cand_pos_in_indexset[parent_id];


			leaf_necs[leaf_necs_idx++] = represent_node;

			for (long long it = 0; it < unit.size_of_index[parent_pos]; it++) {
				long long can = unit.candidates[unit.index_N_up_u[parent_pos][it].index];
				if (already_mapping_flag_data[can] == 0)
					leaf_cands.push_back(can);
			}

			long long end_l = leaf_cands.size();

			leaf_cands_info.push_back(make_pair(start_l, end_l));

			count_local_mapping_label = NEC_set_ranking[i].second.second; //we have computed this one in the last step
			if (count_local_mapping_label == 0) {

				return 0;
			}

		}
		else {//==== CASE TWO : more than one node, and possible more than one start nodes (nec_units)

			long long start = NEC_set_by_label_index[label_index].second;
			long long end = NEC_set_by_label_index[label_index + 1].second;
			long long nec_size = end - start; //number of nec this label has

			for (long long j = start, x = 0; j < end; j++, x++)
				v_nec_count[x] = make_pair(j, NEC_set_array[j].sum);
			sort(v_nec_count, v_nec_count + nec_size, sort_by_second_element);
			nec_count_set_size = nec_size;

			for (long long j = 0; j < nec_size; j++) {
				long long nec_index = v_nec_count[j].first;
				long long nec_count = v_nec_count[j].second;
				nec_count_set[j] = nec_count; // the sum of nodes that the nec representative stands for

				long long parent_id = NEC_set_array[nec_index].parent_id;
				long long represent_node = NEC_set_array[nec_index].represent_node;
				long long parent_pos = g_cand_pos_in_indexset[parent_id];

				CPINode & unit = indexSet[represent_node];
				long long start = leaf_cands.size();
				for (long long it = 0; it < unit.size_of_index[parent_pos]; it++) {
					long long can = unit.candidates[unit.index_N_up_u[parent_pos][it].index];
					if (already_mapping_flag_data[can] == 0)
						leaf_cands.push_back(can);
				}
				long long end = leaf_cands.size();

				if (end - start < nec_count)
					return 0;

				leaf_necs[leaf_necs_idx++] = represent_node;
				leaf_cands_info.push_back(make_pair(start, end));
				NEC_Node * next = NEC_Node_array[represent_node].nextAddress;
				while (next != NULL) {
					leaf_necs[leaf_necs_idx++] = next->node;
					leaf_cands_info.push_back(make_pair(start, end));
					next = next->nextAddress;
				}
			}
		}
	}
	//====================================================================

	//	cerr << "before searching " << endl;
	//	for (int i = 0; i < leaf_necs_idx; i ++)
	//		cerr << leaf_necs[i] << " ";
	//	cerr << endl;


	SearchIterator *search_iterator_leaf = new SearchIterator[g_cnt_node_query_graph];
	long long * actual_mapping_leaf = new long long[g_cnt_node_query_graph];
	for (long long i = 0; i < g_cnt_node_query_graph; i++)
		search_iterator_leaf[i].address = NULL;

	//=========== start to mapping =======================================================
	{
		SearchIterator * temp_search_iterator;
		long long data_id;
		long long current_leaf_query_index = 0;
		long long leaf_sequence_size = leaf_necs_idx;

		while (true) {

			//			cerr << " current leaf query index is " << current_leaf_query_index << " out of " << leaf_sequence_size << endl;

			if (current_leaf_query_index == -1)//"No MATCH found!"
				break;

			if (current_leaf_query_index == leaf_sequence_size) { // found a mapping

				found_mapping_enumeration++;

				//				cerr << "found mapping " << found_mapping_enumeration << endl;

				if (g_isTree) {
					for (long long i = 0; i < g_residual_tree_match_seq_index; i++) //for tree if existed
						all_mapping[g_residual_tree_match_seq_query[i]] = actual_mapping_tree[i];
					for (long long i = 0; i < leaf_necs_idx; i++) //for leaf if existed
						all_mapping[leaf_necs[i]] = actual_mapping_leaf[i];
				}
				else {
					for (long long i = 0; i < g_matching_order_size_of_core; i++) //for core
						all_mapping[g_matching_order_unit_of_query[i].node] = g_actual_mapping[i];
					for (long long i = 0; i < g_residual_tree_match_seq_index; i++) //for tree if existed
						all_mapping[g_residual_tree_match_seq_query[i]] = actual_mapping_tree[i];
					for (long long i = 0; i < leaf_necs_idx; i++) //for leaf if existed
						all_mapping[leaf_necs[i]] = actual_mapping_leaf[i];
				}

				cout << "####### Tree Exact Mapping " << found_mapping_enumeration << " => ";
				for (long long i = 0; i < g_cnt_node_query_graph;i++)
					cout << i << ":" << all_mapping[i] << " ";
				cout << endl;

				if (found_mapping_enumeration >= LIMIT) {
					while (current_leaf_query_index != 0) {
						current_leaf_query_index--;
						already_mapping_flag_data[actual_mapping_leaf[current_leaf_query_index]] = 0;
						search_iterator_leaf[current_leaf_query_index].address = NULL;
					}
					break;
				}

				already_mapping_flag_data[actual_mapping_leaf[leaf_sequence_size - 1]] = 0;
				current_leaf_query_index--;
				continue;
			}

			char back_trace = 0;
			temp_search_iterator = &search_iterator_leaf[current_leaf_query_index];

			if (temp_search_iterator->address == NULL) { // has no value

				temp_search_iterator->address = new CPICell();//actual_mapping_leaf;//meaningless. just for giving it a value for code reusing
				temp_search_iterator->address_size = leaf_cands_info[current_leaf_query_index].second;

				if (temp_search_iterator->address_size == 0) {
					temp_search_iterator->address = NULL;//clear the temp_address and it index position
					current_leaf_query_index--; // roll back one node in the matching sequence
					if (current_leaf_query_index != 0)
						already_mapping_flag_data[actual_mapping_leaf[current_leaf_query_index]] = 0;
					continue;
				}
				temp_search_iterator->address_pos = leaf_cands_info[current_leaf_query_index].first;
			}
			else { // has value
				temp_search_iterator->address_pos++; // update the index by one
				if (temp_search_iterator->address_pos == temp_search_iterator->address_size) {
					temp_search_iterator->address = NULL;//clear the temp_address and it index position
					current_leaf_query_index--;
					if (current_leaf_query_index != -1)
						already_mapping_flag_data[actual_mapping_leaf[current_leaf_query_index]] = 0;
					continue;
				}
			}

			back_trace = 0; //actually, this line is not necessary, when processed here, the back_trace must be false...

			while (true) {
				//break, until find a mapping for this node
				//or cannot find a mapping after examining all candidates

				data_id = leaf_cands[temp_search_iterator->address_pos];

				if (data_id != -1 && already_mapping_flag_data[data_id] == 0) { //first check: this id should have not been mapped before
					actual_mapping_leaf[current_leaf_query_index] = data_id;
					already_mapping_flag_data[data_id] = 1;
					break;
				}
				else { //mapping NOT OK!
					temp_search_iterator->address_pos++;//not ok, then we need the next result
					if (temp_search_iterator->address_pos == temp_search_iterator->address_size) { // no more data id, so cannot find a match for this query node
						back_trace = 1; // indicate that no result is being found, so we need to trace back_trace
						break;
					}
				}
			} //end while
			if (back_trace) { //BACK TRACE
				back_trace = 0;
				temp_search_iterator->address = NULL;//clear the temp_address and it index position
				current_leaf_query_index--;
				if (current_leaf_query_index != -1)
					already_mapping_flag_data[actual_mapping_leaf[current_leaf_query_index]] = 0;
			}
			else
				current_leaf_query_index++;
		}
	}

	delete search_iterator_leaf;
	return found_mapping_enumeration;

}


inline void find_inexact_result() {
	bool exact = false;
	double found_mapping_enumeration = 0;
	MatchOrderUnit * unit;
	SearchIterator * temp_search_unit;
	long long current_id;
	long long parent_id;
	CPINode * index_unit_current;
	long long pos;
	vector<int> cost;
	long long data_id;
	char mapping_OK;
	int * already_mapping_flag_data = g_good_count_data_graph;
	long long back_trace_time = 0;

	for (long long i = 0; i < indexSet[g_root_node_id_of_query].size; i++) {
		long long root_cand_id = indexSet[g_root_node_id_of_query].candidates[i];
		if (root_cand_id == -1) {
			continue;
		}

		already_mapping_flag_data[root_cand_id] = 1;
		cost.push_back(0);
		g_actual_mapping[0] = root_cand_id;
		g_cand_pos_in_indexset[g_root_node_id_of_query] = i;
		char back_trace = 0;
		long long already_matched_size = 1; // the current query index of the query sequence, because 0 is the root has already been matched
		int cost_now = 0;

		while (true) {
			if (already_matched_size == 0)//"No MATCH found!"
				break;
			if (already_matched_size == g_matching_order_size_of_core) { // found a mapping

				if (g_residual_tree_match_seq_index == 0) {

					if (NEC_leaf_mapping_pair_index != 0 && !DYNAMIC) {
						LeafMappingEnumeration(found_mapping_enumeration, already_mapping_flag_data);
					}
					else

					{

						for (long long i = 0; i < g_matching_order_size_of_core; i++) //for core
							all_mapping[g_matching_order_unit_of_query[i].node] = g_actual_mapping[i];

						found_mapping_enumeration++;

						/*
						if (found_mapping_enumeration > 100 * KK) {
						while (already_matched_size != 0) {
						already_matched_size--;
						already_mapping_flag_data[g_actual_mapping[already_matched_size]] = 0;
						search_iterator[already_matched_size].address = NULL;
						}
						already_mapping_flag_data[root_cand_id] = 0;
						return;
						}
						*/
						back_trace_time = 0;
						/*
						cout << "Mapping " << found_mapping_enumeration << " => ";
						for (long long i = 0; i < g_cnt_node_query_graph;i++)
						cout << i << ":" << all_mapping[i] << " ";
						cout << "   cost: " << cost_now;
						cout << endl;
						*/
						checkNow(already_matched_size, cost);
					}
				}
				else {
					//getResidualTreeMapping_Enumeration(found_mapping_enumeration);
				}

				// max output match
				/*
				if (found_mapping_enumeration >= LIMIT) {
				//need to clean up the two array: mapping_flag_data, su[i].address for next time using
				while (already_matched_size != 0) {
				already_matched_size--;
				already_mapping_flag_data[g_actual_mapping[already_matched_size]] = 0;
				search_iterator[already_matched_size].address = NULL;
				}
				already_mapping_flag_data[root_cand_id] = 0;
				cost.clear();
				//return found_mapping_enumeration;
				return;
				}
				*/

				already_mapping_flag_data[g_actual_mapping[g_matching_order_size_of_core - 1]] = 0;
				already_matched_size--;
				cost_now -= cost[cost.size() - 1];
				cost.pop_back();
				continue;
			}

			// matching_unit contains the information of query data
			unit = &g_matching_order_unit_of_query[already_matched_size];
			temp_search_unit = &search_iterator[already_matched_size];
			current_id = unit->node;
			parent_id = g_matching_order_unit_of_query[unit->pt_index].node;
			index_unit_current = &indexSet[current_id];

			if (g_result_heap.size() >= TOPK) {
				if (g_max_cost_in_heap == 0) {
					temp_search_unit->address = NULL;

					while (already_matched_size != 0) {
						already_matched_size--;
						already_mapping_flag_data[g_actual_mapping[already_matched_size]] = 0;
						search_iterator[already_matched_size].address = NULL;
					}
					already_mapping_flag_data[root_cand_id] = 0;
					return;
				}

				if ((g_cnt_node_query_graph - g_matching_order_size_of_core) * NODE_MISS_COST >= g_max_cost_in_heap) {
					// backtrace
					temp_search_unit->address = NULL;
					already_matched_size--;
					cost_now -= cost[cost.size() - 1];
					cost.pop_back();
					already_mapping_flag_data[g_actual_mapping[already_matched_size]] = 0;
					continue;
				}

				if (cost_now > g_max_cost_in_heap) {
					// backtrace
					temp_search_unit->address = NULL;
					already_matched_size--;
					cost_now -= cost[cost.size() - 1];
					cost.pop_back();
					already_mapping_flag_data[g_actual_mapping[already_matched_size]] = 0;
					continue;
				}
			}


			long long parent_pos_index = g_cand_pos_in_indexset[parent_id];

			if (temp_search_unit->address == NULL) { // has no value

				temp_search_unit->address = index_unit_current->index_N_up_u[parent_pos_index];
				temp_search_unit->address_size = index_unit_current->size_of_index[parent_pos_index];

				if (temp_search_unit->address_size == 0) {
					temp_search_unit->address = NULL;//clear the temp_address and it index position
					already_matched_size--; // roll back one node in the matching sequence
					cost_now = cost_now - cost[cost.size() - 1];
					cost.pop_back();



					if (already_matched_size != 0) {
						already_mapping_flag_data[g_actual_mapping[already_matched_size]] = 0;
					}

					continue;
				}
				temp_search_unit->address_pos = 0;
			}
			else { // has value
				temp_search_unit->address_pos++; // update the index by one
												 // iterate all but not find
				if (temp_search_unit->address_pos == temp_search_unit->address_size) {
					temp_search_unit->address = NULL;//clear the temp_address and it index position
					already_matched_size--;
					cost_now = cost_now - cost[cost.size() - 1];
					cost.pop_back();

					already_mapping_flag_data[g_actual_mapping[already_matched_size]] = 0;
					continue;
				}
			}

			back_trace = 0; //actually, this line is not necessary, when processed here, the back_trace must be false...


							// iterate inside a cpi
			while (true) {
				pos = index_unit_current->index_N_up_u[g_cand_pos_in_indexset[parent_id]][temp_search_unit->address_pos].index;
				int bridge_cost = index_unit_current->index_N_up_u[g_cand_pos_in_indexset[parent_id]][temp_search_unit->address_pos].bridge_length - 1;
				bridge_cost = bridge_cost * BRIDGE_COST;
				double simCost = index_unit_current->index_N_up_u[g_cand_pos_in_indexset[parent_id]][temp_search_unit->address_pos].similarity;
				simCost = (1 - simCost) * 3;
				data_id = index_unit_current->candidates[pos];

				mapping_OK = 1;
				// add this
				temp_search_unit->node_id = data_id;


				if (!already_mapping_flag_data[data_id]) { //first check: this id should have not been mapped before

					if (unit->nte_length) { //second check: check and validate the nontree edge
						int unmatch = 0;
						// check all neighbor
						for (long long j = unit->start_pos; j < unit->start_pos + unit->nte_length; j++) {

							if (exact) {
								// exact condition
								if (SIZEOK == 1) {
									if (g_data_edge_matrix[data_id * SIZE_OF_EDGE_MATRIX + g_actual_mapping[nte_array_for_matching_unit[j]]] == 0) {
										mapping_OK = 0;
										break;
									}
								}
								else {
									if (g_data_edge_matrix[(data_id % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (g_actual_mapping[nte_array_for_matching_unit[j]] % SIZE_OF_EDGE_MATRIX)] == 0) {
										mapping_OK = 0;
										break;
									}
									else {
										//validate the non tree edge by querying the edgeIndex
										if (!g_hash_table[data_id]->query(g_actual_mapping[nte_array_for_matching_unit[j]])) {
											mapping_OK = 0;
											break;
										}
									}
								}

							}
							else {
								if (IS_ONE_HOP_DATA_GRAPH > 0) {
									// this is one hop
									if (SIZEOK == 1) {
										if (g_data_edge_matrix[data_id * SIZE_OF_EDGE_MATRIX + g_actual_mapping[nte_array_for_matching_unit[j]]] == 0) {
											unmatch++;
										}
									}
									else {
										if (g_data_edge_matrix[(data_id % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (g_actual_mapping[nte_array_for_matching_unit[j]] % SIZE_OF_EDGE_MATRIX)] == 0) {
											unmatch++;
										}
										else {
											//validate the non tree edge by querying the edgeIndex
											if (!g_hash_table[data_id]->query(g_actual_mapping[nte_array_for_matching_unit[j]])) {
												unmatch++;
											}
										}
									}

									// check wheter edge match

									//int data_edge_from_id = data_id;
									//int query_edge_from_id = current_id;
									//int data_edge_to_id = g_actual_mapping[nte_array_for_matching_unit[j]];
									//int query_edge_to_id = nte_array_for_matching_unit[j];
									// if check here, should use hasmap<string, id>
									// id should be decide in backwardprune

								}
								else {

									long long neigborId = g_actual_mapping[nte_array_for_matching_unit[j]];


									if (g_adj_list_one_hop_distance_data_graph[data_id].query(neigborId)) {
										int dis = 1;
										bridge_cost += ((dis - 1) * BRIDGE_COST);
									}
									else if (g_adj_list_two_hop_distance_data_graph[data_id].query(neigborId)) {
										int dis = 2;
										bridge_cost += ((dis - 1) * BRIDGE_COST);
									}
									else {
										unmatch++;
									}


								}

							}

						}

						if (!exact) {

							if (unmatch > MISSING_EDGE_THRESHOLD_VALUE) {
								mapping_OK = 0;
								//break; should not break
							}
							bridge_cost += (unmatch * 1.0 / unit->nte_length) * EDGE_MISSING_COST;
						}
					}

				}
				else
					mapping_OK = 0;

				if (mapping_OK) {
					g_actual_mapping[already_matched_size] = data_id;
					g_cand_pos_in_indexset[current_id] = pos;
					already_mapping_flag_data[data_id] = 1;
					cost.push_back(bridge_cost);
					cost_now = cost_now + bridge_cost;

					break;
				}
				else { //mapping NOT OK!
					temp_search_unit->address_pos++;//not ok, then we need the next result
					if (temp_search_unit->address_pos == temp_search_unit->address_size) { // no more data id, so cannot find a match for this query node
						back_trace = 1; // indicate that no result is being found, so we need to trace back_trace
						break;
					}
				}


			} //end while
			  // not find, back_trace is zero

			if (back_trace) { //BACK TRACE

							  //----------------------------//------------------------------------
#ifdef PARTIAL_MATCHING
							  // record the max partial match
				if (already_matched_size > g_maxPartialNum) {
					g_maxPartialNum = already_matched_size;
					// record the match
					// partialMapping->clear();

					for (long long i = 0; i < g_matching_order_size_of_core; i++) {
						if (already_mapping_flag_data[g_actual_mapping[i]]) {
							all_mapping[g_matching_order_unit_of_query[i].node] = g_actual_mapping[i];
						}
					}

					//if (branchControl(all_mapping)) {
					found_mapping_enumeration++;
					// print partial mapping
					cout << "||||||||||||| Partial Mapping length " << already_matched_size << " :" << found_mapping_enumeration << " => ";
					for (long long i = 0; i < g_matching_order_size_of_core; i++) {
						if (all_mapping[i] >= 0) {
							cout << i << ":" << all_mapping[i] << " ";
						}
					}
					cout << endl;
					//}
				}

#endif
				//------------------------------------

				back_trace = 0;
				temp_search_unit->address = NULL;//clear the temp_address and it index position
				already_matched_size--;
				already_mapping_flag_data[g_actual_mapping[already_matched_size]] = 0;

				cost_now = cost_now - cost[cost.size() - 1];
				cost.pop_back();

				checkNow(already_matched_size, cost);

				back_trace_time++;



				int _a = back_trace_time % 100000;
				if (_a == 0) {
					double duration = (clock() - g_clock) * 1.0 / CLOCKS_PER_SEC;
					if (duration > MAX_TIME) {
						search_iterator[already_matched_size].address = NULL;
						while (already_matched_size != 0) {
							already_matched_size--;
							already_mapping_flag_data[g_actual_mapping[already_matched_size]] = 0;
							search_iterator[already_matched_size].address = NULL;
						}
						already_mapping_flag_data[root_cand_id] = 0;
						return;
					}
				}

				/*if (back_trace_time > MAX_BACK_TRACE) {
				search_iterator[already_matched_size].address = NULL;
				while (already_matched_size != 0) {
				already_matched_size--;
				already_mapping_flag_data[g_actual_mapping[already_matched_size]] = 0;
				search_iterator[already_matched_size].address = NULL;
				}
				already_mapping_flag_data[root_cand_id] = 0;
				return;

				}*/

			}
			else {
				already_matched_size++;
			}

		}
		//===========================================================================================
		already_mapping_flag_data[root_cand_id] = 0;
	}
	return;

}


inline int bridge(long long from, long long to) {
	for (long long j = g_nodes_adj_list_start_index_data_graph[from]; j < g_nodes_adj_list_start_index_data_graph[from + 1]; j++) {
		long long neighbor = g_nodes_adj_list_with_edge_type_data_graph[j].node_id;

		if (SIZEOK == 1) {
			if (g_data_edge_matrix[neighbor * SIZE_OF_EDGE_MATRIX + to] == 0) {
				continue;
			}
			else {
				return neighbor;
			}
		}
		else {
			if (g_data_edge_matrix[(neighbor % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (to % SIZE_OF_EDGE_MATRIX)] == 0) {
				continue;
			}
			else {
				//validate the non tree edge by querying the edgeIndex
				if (g_hash_table[neighbor]->query(to)) {
					return neighbor;
				}
			}
		}

	}
	return -1;
}

inline int adj_check(int neighbor, int to) {
	if (SIZEOK == 1) {
		if (g_data_edge_matrix[neighbor * SIZE_OF_EDGE_MATRIX + to] == 0) {
			return -1;
		}
		else {
			return neighbor;
		}
	}
	else {
		if (g_data_edge_matrix[(neighbor % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (to % SIZE_OF_EDGE_MATRIX)] == 0) {
			return -1;
		}
		else {
			//validate the non tree edge by querying the edgeIndex
			if (g_hash_table[neighbor]->query(to)) {
				return neighbor;
			}
			else {
				return -1;
			}
		}
	}
}

inline vector<long long> bridge3(long long from, long long to) {
	for (long long j = g_nodes_adj_list_start_index_data_graph[from]; j < g_nodes_adj_list_start_index_data_graph[from + 1]; j++) {
		long long neighbor = g_nodes_adj_list_with_edge_type_data_graph[j].node_id;

		for (long long three = g_nodes_adj_list_start_index_data_graph[neighbor]; three < g_nodes_adj_list_start_index_data_graph[neighbor + 1]; three++) {
			long long threeNeighbor = g_nodes_adj_list_with_edge_type_data_graph[three].node_id;

			if (adj_check(threeNeighbor, to)) {
				vector<long long> res;
				res.push_back(neighbor);
				res.push_back(threeNeighbor);
				return res;
			}
		}
	}
	vector<long long> empty;
	return empty;
}

inline void output_result() {
	vector<long long> intermediate_node;
	for (int i = 0; i < g_result_heap.size(); i++) {

		Result r = g_result_heap[i];

		if (IS_ONE_HOP_DATA_GRAPH > 0) {
			for (int i = 0; i < r.result.size(); i++) {
				cout << i << ":" << r.result[i] << " ";
			}
			cout << "   cost: " << r.cost << endl;

			continue;
		}

		intermediate_node.clear();
		for (int j = 1; j < g_cnt_node_query_graph; j++) {
			MatchOrderUnit * unit = &g_matching_order_unit_of_query[j];
			long long cur_index = unit->node;
			if (cur_index < 0) {
				continue;
			}
			if (cur_index > r.result.size()) {
				continue;
			}
			long long cur_data_id = r.result[cur_index];

			// no satisfy node
			if (cur_data_id == -1) {
				continue;
			}

			if (cur_data_id < 0) {
				continue;
			}

			long long parent_index = g_matching_order_unit_of_query[unit->pt_index].node;
			// find connect to parent by bridge
			long long parent_data_id = r.result[parent_index];

			if (parent_data_id == -1) {
				continue;
			}

			if (g_adj_list_two_hop_distance_data_graph[parent_data_id].query(cur_data_id)) {
				// dis = 2;
				long long intermediate = bridge(parent_data_id, cur_data_id);
				intermediate_node.push_back(intermediate);
			}
			else if (g_adj_list_one_hop_distance_data_graph[parent_data_id].query(cur_data_id)) {
				int good = 0;
			}
			else {
				vector<long long> t = bridge3(parent_data_id, cur_data_id);
				if (t.size() != 0) {
					intermediate_node.insert(intermediate_node.end(), t.begin(), t.end());
				}
			}

			if (unit->nte_length) {
				for (long long j = unit->start_pos; j < unit->start_pos + unit->nte_length; j++) {
					// find connection to neighbor by bridge
					long long nei_index = nte_array_for_matching_unit[j];
					long long nei_data_id = r.result[g_matching_order_unit_of_query[nei_index].node];
					// neighbor missing?
					if (nei_data_id == -1) {
						continue;
					}

					if (g_adj_list_two_hop_distance_data_graph[parent_data_id].query(cur_data_id)) {
						// dis = 2;
						long long intermediate = bridge(parent_data_id, cur_data_id);
						intermediate_node.push_back(intermediate);
					}
					else if (g_adj_list_one_hop_distance_data_graph[parent_data_id].query(cur_data_id)) {
						int good = 0;
					}
					else {
						vector<long long> t = bridge3(parent_data_id, cur_data_id);
						if (t.size() != 0) {
							intermediate_node.insert(intermediate_node.end(), t.begin(), t.end());
						}
					}

				}
			}

		}

		for (long long j = 0; j < r.result.size(); ++j) {
			cout << j << ":" << r.result[j] << " ";
		}
		for (long long j = 0; j < intermediate_node.size(); ++j) {
			cout << j << ":" << intermediate_node[j] << " ";
		}

		cout << "   cost: " << r.cost << endl;

	}

}




#endif // !DYNAMIC_FAST_FIND_RESULT_H_



