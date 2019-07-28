#pragma once

#ifndef	FAST_DYNAMIC_MATCHING_ORDER_H_
#define FAST_DYNAMIC_MATCHING_ORDER_H_

#include "global_variable.h"
#include <algorithm>

#define MAX_NTE_QUERY 2000

extern long long g_query_node_id_to_matching_order_num[MAX_QUERY_NODE];
extern long long g_leaf_nodes_index;
extern MatchOrderUnit g_matching_order_unit_of_query[MAX_QUERY_NODE];
extern long long g_matching_order_size_of_core;
extern long long nte_array_for_matching_unit[MAX_NTE_QUERY];

// this variable will be used only in this file
extern long long temp_array_query_index;
extern long long sum_nontree_edge_of_every_node[MAX_QUERY_NODE];
extern long long leaf_nodes[MAX_QUERY_NODE];
extern long long stack_array_query[MAX_QUERY_NODE];
extern long long temp_array_query[MAX_QUERY_NODE];
extern long sum_path[MAX_QUERY_NODE];

extern double * path_acuu;
extern double * path_temp;

extern pair<long long, double> leaf_path_info[MAX_QUERY_NODE];

extern vector<long long> temp_vector;

inline void INIT_MODULE_MATCHING_ORDER() {

	path_acuu = new double[g_cnt_node_of_data_graph];
	path_temp = new double[g_cnt_node_of_data_graph];

}


inline void assignMatchOrderUnit(MatchOrderUnit & unit, long long node, long long parent_index, long long nte_length, long long start_pos) {
	unit.node = node;
	unit.pt_index = parent_index;
	unit.nte_length = nte_length;
	unit.start_pos = start_pos;
}


inline void addIntoMatchOrderSequenceForWholeGraph(long long node_id, char isRoot, long long * g_query_node_id_to_matching_order_num) {

	if (node_id == 225) {
		int a = 0;
	}
	if (node_id == 221) {
		int a = 0;
	}

	// g_matching_sequence_index_of_core is 0 when first enter
	if (isRoot) {
		assignMatchOrderUnit(g_matching_order_unit_of_query[g_matching_order_size_of_core], node_id, -1, 0, 0);
		g_query_node_id_to_matching_order_num[node_id] = g_matching_order_size_of_core + 1;
		g_matching_order_size_of_core++;
		//		cerr << "set " << node << " 's parent to -1." << endl;
	}
	else {

		CoreQueryBFSTreeNode * tree_node = &core_query_tree[node_id];

		if (tree_node->nte.second != 0) {

			long long begin = g_nte_array_for_matching_unit_index;

			for (long long i = tree_node->nte.first; i < tree_node->nte.first + tree_node->nte.second; i++) {
				long long other_end = g_core_tree_node_nte_array[i];
				if (g_query_node_id_to_matching_order_num[other_end]) {
					nte_array_for_matching_unit[g_nte_array_for_matching_unit_index] = g_query_node_id_to_matching_order_num[other_end] - 1;
					g_nte_array_for_matching_unit_index++;
				}
			}

			long long end = g_nte_array_for_matching_unit_index;
			// unit, node_id, parent_index, nte_length, start_pos
			assignMatchOrderUnit(g_matching_order_unit_of_query[g_matching_order_size_of_core], node_id, g_query_node_id_to_matching_order_num[tree_node->parent_node] - 1, end - begin, begin);

		}
		else
			assignMatchOrderUnit(g_matching_order_unit_of_query[g_matching_order_size_of_core], node_id, g_query_node_id_to_matching_order_num[tree_node->parent_node] - 1, 0, 0);

		g_query_node_id_to_matching_order_num[node_id] = g_matching_order_size_of_core + 1;
		g_matching_order_size_of_core++;
	}

}


inline void calculate_path_cost_in_core_by_core_query_tree(long long & root, long long & first_path_leaf, long long & first_leaf_index) {

	double first_path_ranking = DBL_MAX;
	//To find all the leaf nodes and get the sum_nontree edges, we perform a top-down traverse of the query tree here
	long long stack_array_index = 0;
	stack_array_query[stack_array_index++] = root;

	// DFS
	while (stack_array_index != 0) {

		long long current_node = stack_array_query[stack_array_index - 1];
		double current_nte_number = (double)sum_nontree_edge_of_every_node[current_node];
		stack_array_index--;
		CoreQueryBFSTreeNode & tree_node = core_query_tree[current_node];

		if (tree_node.children.second == 0) { //This is a leaf node

											  //compute the sum of the path for the leaf node
			long long path_sum = 0;
			for (long long x = 0; x < indexSet[current_node].size; x++)
				path_sum += indexSet[current_node].path[x];

			sum_path[current_node] = path_sum;

			//====== compute the sum of non tree edge in this path, stop when reach current bcc's root id
			//====== note that, each leaf node's path is all the way to the core structure's root id
			//       so , must break when reach current bcc's root id
			//===== now compute the ranking value

			current_nte_number += 1; // this is the extra parameter...
			double ranking = (double)path_sum;

			//get the first path to select
			// I add
			if (ranking < first_path_ranking) {
				first_path_ranking = ranking;
				first_path_leaf = current_node;
				first_leaf_index = g_leaf_nodes_index;
			}
			leaf_nodes[g_leaf_nodes_index++] = current_node;

		}
		else { //This is not a leaf node.
			   //put the children nodes into the stack
			   // It's DFS
			for (long long i = tree_node.children.first; i < tree_node.children.first + tree_node.children.second; i++) {
				long long child = g_core_tree_node_child_array[i];
				sum_nontree_edge_of_every_node[child] = current_nte_number + core_query_tree[child].nte.second;
				stack_array_query[stack_array_index++] = child;
			}
		}
	}
	int a = 0;
}

inline void deal_with_first_path(long long & first_path_leaf, long long & first_leaf_index) {

	// now we start to construct the heap to select the pathes
	{
		// deal with the first path
		// cerr << "first path leaf is " << first_path_leaf << endl;

		long long leaf_id = first_path_leaf;
		temp_array_query_index = 0; //always put to zero before using it
		while (g_query_node_id_to_matching_order_num[leaf_id] == 0) { // here, actually stop at the bcc root, which is not to be processed here
			temp_array_query[temp_array_query_index++] = leaf_id;
			leaf_id = core_query_tree[leaf_id].parent_node;//this node's parent
		}

		while (temp_array_query_index != 0) {// add the sequence in the temp array into the sequence array
			temp_array_query_index--;
			addIntoMatchOrderSequenceForWholeGraph(temp_array_query[temp_array_query_index], 0, g_query_node_id_to_matching_order_num);
		}

		leaf_nodes[first_leaf_index] = leaf_nodes[g_leaf_nodes_index - 1];//remove the first leaf
		g_leaf_nodes_index--;

	} //end dealing with the first path

}


inline void deal_with_first_path_(long long & first_path_leaf, long long & first_leaf_index, int * visited) {

	// now we start to construct the heap to select the pathes
	{
		// deal with the first path
		// cerr << "first path leaf is " << first_path_leaf << endl;

		long long leaf_id = first_path_leaf;
		temp_array_query_index = 0; //always put to zero before using it
		while (g_query_node_id_to_matching_order_num[leaf_id] == 0) { // here, actually stop at the bcc root, which is not to be processed here
			temp_array_query[temp_array_query_index++] = leaf_id;
			leaf_id = core_query_tree[leaf_id].parent_node;//this node's parent
		}

		while (temp_array_query_index != 0) {// add the sequence in the temp array into the sequence array
			temp_array_query_index--;
			temp_vector.push_back(temp_array_query[temp_array_query_index]);
			visited[temp_array_query[temp_array_query_index]] = 1;
		}

		leaf_nodes[first_leaf_index] = leaf_nodes[g_leaf_nodes_index - 1];//remove the first leaf
		g_leaf_nodes_index--;

	} //end dealing with the first path

}



inline void generateMatchingOrderByDynamic() {

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

	// bottom-up for each remaining path and record each leaf path's connection node
	{
		long long min_leaf_index;
		// I add
		double min_ranking = DBL_MAX;

		for (long long i = 0; i < g_leaf_nodes_index; i++) {

			long long leaf = leaf_nodes[i];
			double cand_con;
			double path_sum = 0;

			//find its connection node
			long long node = core_query_tree[leaf].parent_node;
			long long before_connect = leaf; //set the node before connection node
			// long long parent_cand_size = indexSet[BFS_parent_query[leaf]].size;
			long long parent_cand_size = indexSet[node].size;

			for (long long i = 0; i < parent_cand_size; i++)
				path_acuu[i] = indexSet[leaf].size_of_index[i];

			while (g_query_node_id_to_matching_order_num[node] == 0) { // here, stop when reaching the connection node

				long long cand_size = indexSet[g_forward_build_parent[node]].size;
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
			//double ranking = path_sum;
			leaf_path_info[leaf].second = ranking;
			// cerr << "ranking is " << ranking << endl;
			// I add
			if (ranking < min_ranking) {
				min_ranking = ranking;
				min_leaf_index = i;
			}

		}//end for

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

	{//now we start to select the rest of the paths

		long long min_leaf_index;

		while (g_leaf_nodes_index != 0) {

			// I add
			double min_ranking = DBL_MAX;

			for (long long i = 0; i < g_leaf_nodes_index; i++) {

				long long leaf = leaf_nodes[i];
				if (g_query_node_id_to_matching_order_num[leaf_path_info[leaf].first] == 0) {
					//the last connection node is still unselected, which means its connection node remains unchanged
					// I add
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

				for (long long i = 0; i < indexSet[g_forward_build_parent[leaf]].size; i++)
					path_acuu[i] = unit_leaf.size_of_index[i];

				while (!g_query_node_id_to_matching_order_num[node]) { // here, stop when reaching the connection node

					CPINode & unit_node = indexSet[node];
					long long cand_size = indexSet[g_forward_build_parent[node]].size;

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
				// double ranking = path_sum;
				leaf_path_info[leaf].second = ranking;

				// I add
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



inline void calculateLayerCostInCoreByCoreQueryTree(long long & root, long long & first_path_leaf, long long & first_leaf_index) {

	double first_path_ranking = DBL_MAX;
	//To find all the leaf nodes and get the sum_nontree edges, we perform a top-down traverse of the query tree here
	long long stack_array_index = 0;
	stack_array_query[stack_array_index++] = root;

	// DFS
	while (stack_array_index != 0) {

		long long current_node = stack_array_query[stack_array_index - 1];
		double current_nte_number = (double)sum_nontree_edge_of_every_node[current_node];
		stack_array_index--;
		CoreQueryBFSTreeNode & tree_node = core_query_tree[current_node];

		long long path_sum = 0;
		for (long long x = 0; x < indexSet[current_node].size; x++)
			path_sum += indexSet[current_node].path[x];

		sum_path[current_node] = path_sum;

		if (tree_node.children.second == 0) { //This is a leaf node

			current_nte_number += 1; // this is the extra parameter...
			double ranking = (double)path_sum;

			//get the first path to select
			// I add
			if (ranking < first_path_ranking) {
				first_path_ranking = ranking;
				first_path_leaf = current_node;
				first_leaf_index = g_leaf_nodes_index;
			}

			leaf_nodes[g_leaf_nodes_index++] = current_node;

		}
		else { //This is not a leaf node.
			   //put the children nodes into the stack
			   // It's DFS
			for (long long i = tree_node.children.first; i < tree_node.children.first + tree_node.children.second; i++) {
				long long child = g_core_tree_node_child_array[i];
				sum_nontree_edge_of_every_node[child] = current_nte_number + core_query_tree[child].nte.second;
				stack_array_query[stack_array_index++] = child;
			}
		}
	}
	int a = 0;

}



inline bool sort_by_path_sum(long long p1, long long p2) {
	return (sum_path[p1] < sum_path[p2]);
}

inline void matchingOrderLayer() {

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

	calculateLayerCostInCoreByCoreQueryTree(root, first_path_leaf, first_leaf_index);

	long long leaf_num = g_leaf_nodes_index;
	long long layer_size = g_level_size - 2;

	deal_with_first_path(first_path_leaf, first_leaf_index);

	// sort node by layer and pathsum

	for (int i = 2; i < g_level_size; i++) {
		sort(g_level[i].begin(), g_level[i].end(), sort_by_path_sum);
		for (int j = 0; j < g_level[i].size(); j++) {
			addIntoMatchOrderSequenceForWholeGraph(g_level[i][j], 0, g_query_node_id_to_matching_order_num);
		}
	}

	int a = 0;


}


inline void test1() {

	int visited[MAX_QUERY_NODE] = { 0 };

	memset(g_query_node_id_to_matching_order_num, 0, sizeof(long long) * g_cnt_node_query_graph);
	g_leaf_nodes_index = 0;
	temp_array_query_index = 0;
	long long root = g_root_node_id_of_query;

	long long first_path_leaf; // the leaf node of the first path
	addIntoMatchOrderSequenceForWholeGraph(root, 1, g_query_node_id_to_matching_order_num);

	temp_vector.push_back(root);
	visited[root] = 1;

	sum_nontree_edge_of_every_node[root] = 0; // must initialize this


	long long first_leaf_index;

	calculate_path_cost_in_core_by_core_query_tree(root, first_path_leaf, first_leaf_index);

	// end DFS and get the sum_path of leaf nodes

	deal_with_first_path_(first_path_leaf, first_leaf_index, visited);

	// displaySequence();

	// just one path, so didn't need to select path, just return
	if (g_leaf_nodes_index == 0) {//check if there are still leaf nodes leaft
								  // displaySequence();
		return;
	}

	// bottom-up for each remaining path and record each leaf path's connection node
	{
		long long min_leaf_index;
		// I add
		double min_ranking = DBL_MAX;

		for (long long i = 0; i < g_leaf_nodes_index; i++) {

			long long leaf = leaf_nodes[i];
			double cand_con;
			double path_sum = 0;

			//find its connection node
			long long node = core_query_tree[leaf].parent_node;
			long long before_connect = leaf; //set the node before connection node
											 // long long parent_cand_size = indexSet[BFS_parent_query[leaf]].size;
			long long parent_cand_size = indexSet[node].size;

			for (long long i = 0; i < parent_cand_size; i++)
				path_acuu[i] = indexSet[leaf].size_of_index[i];

			while (visited[node] == 0) { // here, stop when reaching the connection node

				long long cand_size = indexSet[g_forward_build_parent[node]].size;
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
			//double ranking = path_sum;
			leaf_path_info[leaf].second = ranking;
			// cerr << "ranking is " << ranking << endl;
			// I add
			if (ranking < min_ranking) {
				min_ranking = ranking;
				min_leaf_index = i;
			}

		}//end for

		long long leaf_id = leaf_nodes[min_leaf_index];
		temp_array_query_index = 0; //always put to zero before using it, for safety

		while (!visited[leaf_id]) { // here, actually stop at an already selected node
			temp_array_query[temp_array_query_index++] = leaf_id;
			leaf_id = core_query_tree[leaf_id].parent_node;//this node's parent
		}

		// add the sequence in the temp array into the sequence array
		while (temp_array_query_index != 0) {
			temp_array_query_index--;
			temp_vector.push_back(temp_array_query[temp_array_query_index]);
			visited[temp_array_query[temp_array_query_index]] = 1;
		}

		//remove the selected leaf
		leaf_nodes[min_leaf_index] = leaf_nodes[g_leaf_nodes_index - 1];
		g_leaf_nodes_index--;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	{//now we start to select the rest of the paths

		long long min_leaf_index;

		while (g_leaf_nodes_index != 0) {

			// I add
			double min_ranking = DBL_MAX;

			for (long long i = 0; i < g_leaf_nodes_index; i++) {

				long long leaf = leaf_nodes[i];
				if (visited[leaf_path_info[leaf].first] == 0) {
					//the last connection node is still unselected, which means its connection node remains unchanged
					// I add
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

				for (long long i = 0; i < indexSet[g_forward_build_parent[leaf]].size; i++)
					path_acuu[i] = unit_leaf.size_of_index[i];

				while (!visited[node]) { // here, stop when reaching the connection node

					CPINode & unit_node = indexSet[node];
					long long cand_size = indexSet[g_forward_build_parent[node]].size;

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
				// double ranking = path_sum;
				leaf_path_info[leaf].second = ranking;

				// I add
				if (ranking < min_ranking) {
					min_ranking = ranking;
					min_leaf_index = i;
				}

			}//end for

			 //add this leaf and its path into the matching sequence
			long long leaf_id = leaf_nodes[min_leaf_index];
			temp_array_query_index = 0; //always put to zero before using it, for safety
			while (visited[leaf_id] == 0) { // here, actually stop at an already selected node
				temp_array_query[temp_array_query_index++] = leaf_id;
				leaf_id = core_query_tree[leaf_id].parent_node;//this node's parent
			}

			while (temp_array_query_index != 0) {// add the sequence in the temp array into the sequence array
				temp_array_query_index--;
				temp_vector.push_back(temp_array_query[temp_array_query_index]);
				visited[temp_array_query[temp_array_query_index]] = 1;
			}
			//=========================================

			//remove the selected leaf
			leaf_nodes[min_leaf_index] = leaf_nodes[g_leaf_nodes_index - 1];
			g_leaf_nodes_index--;

		} //end while
	}


	long long LEVEL[MAX_QUERY_NODE];
	for (int i = 1; i <= g_level_size; i++) {
		for (int j = 0; j < g_level[i].size(); j++) {
			LEVEL[g_level[i][j]] = i;
		}
	}

	int LL = 20;
	for (int i = 1; i < temp_vector.size(); i++) {
		long long cId = temp_vector[i];
		if (LEVEL[cId] >= 3) {
			LL--;
		}
		if (LL <= 1) {
			addIntoMatchOrderSequenceForWholeGraph(cId, 0, g_query_node_id_to_matching_order_num);
			for (int j = i + 1; j < temp_vector.size(); j++) {
				long long CCId = temp_vector[j];
				if (LEVEL[CCId] < 3) {
					addIntoMatchOrderSequenceForWholeGraph(CCId, 0, g_query_node_id_to_matching_order_num);
				}
			}
			for (int j = i + 1; j < temp_vector.size(); j++) {
				long long CCId = temp_vector[j];
				if (LEVEL[CCId] >= 3) {
					addIntoMatchOrderSequenceForWholeGraph(CCId, 0, g_query_node_id_to_matching_order_num);
				}
			}

			return;

		}
		else {
			addIntoMatchOrderSequenceForWholeGraph(cId, 0, g_query_node_id_to_matching_order_num);
		
		}
	}

	int a = 0;

}



#endif