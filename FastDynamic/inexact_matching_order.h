#pragma once

#ifndef FAST_DYNAMIC_INEXACT_MATCHING_ORDER_
#define FAST_DYNAMIC_INEXACT_MATCHING_ORDER_

#include "matching_order.h"

inline void generateInexactMatchingOrder() {

	g_nte_array_for_matching_unit_index = 0;
	g_matching_order_size_of_core = 0;
	memset(g_query_node_id_to_matching_order_num, 0, sizeof(long long) * g_cnt_node_query_graph);
	g_leaf_nodes_index = 0;
	temp_array_query_index = 0;
	long long root = g_root_node_id_of_query;

	long long first_path_leaf; // the leaf node of the first path

							   //include the root node first => this always has nothing to do with the matching option
	addIntoMatchOrderSequenceForWholeGraph(root, 1, g_query_node_id_to_matching_order_num); // need to change this function if used for bcc-ordered matching
																							// this array used to track the number of nontree edge of each node in this bcc
	sum_nontree_edge_of_every_node[root] = 0; // must initialize this


	long long first_leaf_index;

	calculate_path_cost_in_core_by_core_query_tree(root, first_path_leaf, first_leaf_index);

	// end DFS and get the sum_path of leaf nodes

	deal_with_first_path(first_path_leaf, first_leaf_index);

	// displaySequence();

	// just one path, so didn't need to select path, just return
	if (g_leaf_nodes_index == 0) {//check if there are still leaf nodes leaft
								  // displaySequence();
		return;
	}

	//	cerr << "remaining leaf node index is " << leaf_nodes_index << endl;

	// bottom-up for each remaining path and record each leaf path's connection node
	{
		long long min_leaf_index;
		double min_ranking = DBL_MAX;

		for (long long i = 0; i < g_leaf_nodes_index; i++) {

			long long leaf = leaf_nodes[i];
			double cand_con;
			double path_sum = 0;

			//find its connection node
			long long node = core_query_tree[leaf].parent_node;
			long long before_connect = leaf; //set the node before connection node
			long long parent_cand_size = indexSet[BFS_parent_query[leaf]].size;

			for (long long i = 0; i < parent_cand_size; i++)
				path_acuu[i] = indexSet[leaf].size_of_index[i];

			while (g_query_node_id_to_matching_order_num[node] == 0) { // here, stop when reaching the connection node
				long long cand_size = indexSet[BFS_parent_query[node]].size;
				for (long long x = 0; x < cand_size; x++) {
					path_temp[x] = 0;
					for (long long y = 0; y < indexSet[node].size_of_index[x]; y++) {
						long long pos = indexSet[node].index_N_up_u[x][y].index;
						path_temp[x] += path_acuu[pos];
					}
				}

				for (long long x = 0; x < cand_size; x++)
					path_acuu[x] = path_temp[x];

				before_connect = node;//set the node as the last node before the connection node
				node = core_query_tree[node].parent_node;//this node's parent
			}

			cand_con = indexSet[node].size;
			for (long long i = 0; i < cand_con; i++)
				path_sum += path_acuu[i];
			leaf_path_info[leaf].first = before_connect;//set the connection node array

														//===================================================
														// I change
			double ranking = path_sum / cand_con;
			if (cand_con == 0 && path_sum == 0) {
				ranking = 0;
			}
			leaf_path_info[leaf].second = ranking;
			// cerr << "ranking is " << ranking << endl;
			if (ranking < min_ranking) {
				min_ranking = ranking;
				min_leaf_index = i;
			}

		}//end for

		 //		cerr << " out of for " << endl;
		 //
		 //		cerr << "min_leaf_index " <<min_leaf_index << endl;
		 //
		 //		cerr << "leaf_id " << leaf_nodes[min_leaf_index] << endl;

		long long leaf_id = leaf_nodes[min_leaf_index];
		temp_array_query_index = 0; //always put to zero before using it, for safety

		while (!g_query_node_id_to_matching_order_num[leaf_id]) { // here, actually stop at an already selected node
			temp_array_query[temp_array_query_index++] = leaf_id;
			leaf_id = core_query_tree[leaf_id].parent_node;//this node's parent
		}

		// add the sequence in the temp array into the sequence array
		while (temp_array_query_index != 0) {
			temp_array_query_index--;
			addIntoMatchOrderSequenceForWholeGraph(temp_array_query[temp_array_query_index], 0, g_query_node_id_to_matching_order_num);
		}

		//remove the selected leaf
		leaf_nodes[min_leaf_index] = leaf_nodes[g_leaf_nodes_index - 1];
		g_leaf_nodes_index--;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	cerr << "dasda" << endl;


	{//now we start to select the rest of the paths

		long long min_leaf_index;

		while (g_leaf_nodes_index != 0) {

			double min_ranking = DBL_MAX;

			for (long long i = 0; i < g_leaf_nodes_index; i++) {

				long long leaf = leaf_nodes[i];
				if (g_query_node_id_to_matching_order_num[leaf_path_info[leaf].first] == 0) {//the last connection node is still unselected, which means its connection node remains unchanged
					if (leaf_path_info[leaf].second < min_ranking) {
						min_ranking = leaf_path_info[leaf].second;
						min_leaf_index = i;
					}
					continue;
				}

				double cand_con;
				double path_sum = 0;

				//========== find its connection node
				CPINode & unit_leaf = indexSet[leaf];
				long long node = core_query_tree[leaf].parent_node;
				long long before_con = leaf; //set the node before connection node

				for (long long i = 0; i < indexSet[BFS_parent_query[leaf]].size; i++)
					path_acuu[i] = unit_leaf.size_of_index[i];

				while (!g_query_node_id_to_matching_order_num[node]) { // here, stop when reaching the connection node

					CPINode & unit_node = indexSet[node];
					long long cand_size = indexSet[BFS_parent_query[node]].size;

					for (long long x = 0; x < cand_size; x++) {
						path_temp[x] = 0;
						for (long long y = 0; y < unit_node.size_of_index[x]; y++) {
							long long pos = unit_node.index_N_up_u[x][y].index;
							path_temp[x] += path_acuu[pos];
						}
					}

					for (long long x = 0; x < cand_size; x++)
						path_acuu[x] = path_temp[x];

					before_con = node;//set before connection node
					node = core_query_tree[node].parent_node;//this node's parent
				}

				cand_con = indexSet[node].size;
				leaf_path_info[leaf].first = before_con;//set the connection node array

				for (long long i = 0; i < cand_con; i++)
					path_sum += path_acuu[i];
				//==============================

				// I change
				
				double ranking = path_sum / (cand_con);
				if (cand_con == 0 && path_sum == 0) {
					ranking = 0;
				}
				// double ranking = path_sum;
				leaf_path_info[leaf].second = ranking;

				if (ranking < min_ranking) {
					min_ranking = ranking;
					min_leaf_index = i;
				}

			}//end for

			 //add this leaf and its path into the matching sequence
			long long leaf_id = leaf_nodes[min_leaf_index];
			temp_array_query_index = 0; //always put to zero before using it, for safety
			while (g_query_node_id_to_matching_order_num[leaf_id] == 0) { // here, actually stop at an already selected node
				temp_array_query[temp_array_query_index++] = leaf_id;
				leaf_id = core_query_tree[leaf_id].parent_node;//this node's parent
			}

			while (temp_array_query_index != 0) {// add the sequence in the temp array into the sequence array
				temp_array_query_index--;
				addIntoMatchOrderSequenceForWholeGraph(temp_array_query[temp_array_query_index], 0, g_query_node_id_to_matching_order_num);
			}
			//=========================================

			//remove the selected leaf
			leaf_nodes[min_leaf_index] = leaf_nodes[g_leaf_nodes_index - 1];
			g_leaf_nodes_index--;

		} //end while
	}

}


#endif // !FAST_DYNAMIC_INEXACT_MATCHING_ORDER_


