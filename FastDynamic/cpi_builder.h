#pragma once
#ifndef FAST_DYNAMIC_CPI_BUILDER_H_
#define FAST_DYNAMIC_CPI_BUILDER_H_

#include <algorithm>
#include <functional>
#include <assert.h>
#include "limits.h"
#include "data_structure.h"
#include "global_variable.h"

using namespace std;

extern long long queue_array_query[MAX_QUERY_NODE];
extern long long visited_int_for_query[MAX_QUERY_NODE];
extern long long visited_int_for_query2[MAX_QUERY_NODE];


/////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////

inline void resetTreeNodes() {
	for (long long i = 0; i < g_cnt_node_query_graph; i++) {
		CoreQueryBFSTreeNode & c = core_query_tree[i];
		c.parent_node = -1;
		c.children = make_pair(0, 0);
		c.nte = make_pair(0, 0);
		c.cross_lvl_nte = NULL;
	}
}


inline void initializeTreeNode(CoreQueryBFSTreeNode & treeNode, long long parent_node) {
	treeNode.parent_node = parent_node;
	treeNode.children = make_pair(0, 0);
	treeNode.nte = make_pair(0, 0);
	treeNode.cross_lvl_nte = NULL;
}

inline void addNonTreeEdgeToTreeNode(CoreQueryBFSTreeNode & treeNode, long long otherEnd) {
	g_core_tree_node_nte_array[g_core_tree_node_nte_array_index++] = otherEnd;
	if (treeNode.nte.second == 0)
		treeNode.nte = make_pair(g_core_tree_node_nte_array_index - 1, 1);
	else
		treeNode.nte.second++;
}

inline void addChildToTreeNode(CoreQueryBFSTreeNode & treeNode, long long child) {
	g_core_tree_node_child_array[g_core_tree_node_child_array_index++] = child;
	if (treeNode.children.second == 0)
		treeNode.children = make_pair(g_core_tree_node_child_array_index - 1, 1);
	else
		treeNode.children.second++;
}


inline void cleanLevel() {
	for (int i = 0; i < g_level_size; i++) {
		g_level[i].clear();
	}
	g_level_size = 0;
}


////////////////////////////////////////////////////////////////////////////////


inline void initCpiNodeByParent(CPINode & cur_node_unit, CPINode & parent_unit) {
	//make sure it wont "new" array every time
	if (cur_node_unit.parent_cand_size < parent_unit.size) {
		cur_node_unit.size_of_index = new long long[parent_unit.size];
		memset(cur_node_unit.size_of_index, 0, sizeof(long long) * parent_unit.size);
		cur_node_unit.index_N_up_u = new CPICell *[parent_unit.size];
		cur_node_unit.parent_cand_size = parent_unit.size;
	}
}



inline void generateNLFQueryNode(long long & current_node, long long & degree_cur, long long & max_nb_degree) {

	//============== generate the neighborhood label array =======================
	long long first = g_nodes_adj_list_start_index_query_graph[current_node];
	// clean the NLF_array
	memset(NLF_array_query, 0, sizeof(long long) * NLF_size);
	// generate NLF_array of current node
	for (long long j = first; j < first + degree_cur; j++) {
		long long local_label = g_nodes_label_query_graph[g_nodes_adj_list_with_edge_type_query_graph[j].node_id];
		long long idx = NLF_size - 1 - local_label / SIZEOF_INT;
		long long pos = local_label % SIZEOF_INT;
		NLF_array_query[idx] |= (1 << pos);
#ifdef CORE_AND_MAX_NB_FILTER
		long long nb_degree = g_node_degree_query_graph[g_nodes_adj_list_with_edge_type_query_graph[j].node_id];
		if (nb_degree > max_nb_degree) //find the max neighbor degree
			max_nb_degree = nb_degree;
#endif
	}


}



inline void initRootCPI(long long root_candidate_id) {
	// build root indexSet
	CPINode & root_node_unit = indexSet[g_root_node_id_of_query];
	root_node_unit.candidates[0] = root_candidate_id;
	root_node_unit.size = 1;
	fill(root_node_unit.path, root_node_unit.path + 1, 1);
}



inline bool sortByDegree_Query_dec(const long long id1, const long long id2) {
	return (g_node_degree_query_graph[id1] > g_node_degree_query_graph[id2]);
}



extern long long BFS_sequence_query_of_all[MAX_QUERY_NODE];
extern long long iterateCoreSequence[MAX_QUERY_NODE];




#endif // !FAST_DYNAMIC_CPI_BUILDER_H_


