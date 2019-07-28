#pragma once
#ifndef FAST_DYNAMIC_GLOBAL_FUNCTION_H_
#define FAST_DYNAMIC_GLOBAL_FUNCTION_H_

#include <string>
#include <vector>
#include "global_variable.h"

#define NOMINMAX
#include <windows.h>

#ifdef max
#undef max
#endif

#include <limits>
#include <algorithm>

using namespace std;

inline unsigned long long split(const string &txt, vector<string> &strs, char ch) {
	//this is the general case
	size_t pos = txt.find(ch);
	size_t initialPos = 0;
	strs.clear();
	// Decompose statement
	while (pos != string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos + 1));
		initialPos = pos + 1;
		pos = txt.find(ch, initialPos);
	}
	// Add the last one
	strs.push_back(txt.substr(initialPos, min(pos, txt.size()) - initialPos + 1));
	//return the size of the vector
	return strs.size();
}

inline bool sortByLabelAndDegree(int node1, int node2) {
	if (g_nodes_label_after_change_data_graph[node1] == g_nodes_label_after_change_data_graph[node2])
		return (g_node_degree_data_graph[node1] < g_node_degree_data_graph[node2]);
	else
		return (g_nodes_label_after_change_data_graph[node1] < g_nodes_label_after_change_data_graph[node2]);
}


inline bool sortByLabelAndDegree_edge_type(EdgeType & a, EdgeType & b) {

	int node1 = a.node_id;
	int node2 = b.node_id;
	if (g_nodes_label_after_change_data_graph[node1] == g_nodes_label_after_change_data_graph[node2])
		return (g_node_degree_data_graph[node1] < g_node_degree_data_graph[node2]);
	else
		return (g_nodes_label_after_change_data_graph[node1] < g_nodes_label_after_change_data_graph[node2]);
}


inline set<long long> parse_edge_type_string(string & edge_types) {
	vector<string> v;
	split(edge_types, v, ',');
	set<long long> edge_type;
	for (int i = 0; i < v.size(); i++) {
		edge_type.insert(atoll(v[i].c_str()));
	}
	return edge_type;
}


#endif // !FAST_DYNAMIC_GLOBAL_FUNCTION_
