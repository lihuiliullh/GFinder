#pragma once

#ifndef FAST_DYNAMIC_DATA_STRUCTURE_H_
#define FAST_DYNAMIC_DATA_STRUCTURE_H_

#include <iostream> 
#include <vector>
#include <cstdlib>
#include <set>
#include <unordered_map>

#include <limits>
#include <algorithm>

using namespace std;

#define HT_LENGTH 1000

struct CPIPath 
{
	long long parent_id;
	int size;
	long long * intermediate_id;
};

//struct EdgeType {
//	int node_id;
//	long long edge_type = 0;
//
//	EdgeType(int n_id, set<long long> e_type) {
//		// parse edge_type_string
//		node_id = n_id;
//		set<long long>::iterator it;
//		for (it = e_type.begin(); it != e_type.end(); ++it) {
//			edge_type |= (1 << *it);
//		}
//	}
//
//	EdgeType() {
//
//	}
//
//	inline bool is_in_edge_type(long long e_type) {
//		long long res = edge_type & e_type;
//		return res == e_type;
//	}
//
//	inline bool is_in_edge_type(EdgeType * e_type) {
//		long long res = edge_type & e_type->edge_type;
//		return res == e_type->edge_type;
//	}
//
//};



/*
struct AdjElement
{
	int to_node_id;
	int cnt_node_of_data_graph_;

	AdjElement() {}

	AdjElement(int node_id, int edge_label, int cnt_node_of_data_graph) {
		to_node_id = node_id + edge_label * cnt_node_of_data_graph;
		cnt_node_of_data_graph_ = cnt_node_of_data_graph;
	}

	int getNodeId() {
		return to_node_id % cnt_node_of_data_graph_;
	}

	int getEdgeLabel() {
		return to_node_id / cnt_node_of_data_graph_;
	}
};
*/

struct SimCell {
	long long id;
	double sim;

	bool operator < (const SimCell& cell) const
	{
		return id < cell.id;
	}
};

struct CPICell {
	long long index;
	long long bridge_length;
	double similarity;
};

struct CPINode
{
	long path_contained_sum; // total path sum
	long * path; // record the path number of each candidate
	int size; // the size of candidate; Notice the size of path is equal to the size of candidate
	int * candidates; // candidate set, the value is nodeID
	//CPIPath ** index_N_up_u = NULL; // 
	CPICell ** index_N_up_u = NULL;
	long long * size_of_index = NULL; // size of index_N_up_u
	int parent_cand_size;
	int useful_cand_size = 0;


};

struct NEC_element {
	// my label
	long long label;
	// my parent id
	long long parent_id;
	// my id in query graph
	long long represent_node;

	NEC_element() {

	}

	NEC_element(long long label, long long parent_id, long long represent_node) {
		this->label = label;
		this->parent_id = parent_id;
		this->represent_node = represent_node;
	}
};

struct NEC_set_array_element {
	long long parent_id;
	long long represent_node;
	long long sum;
	NEC_set_array_element() {
	}
	NEC_set_array_element(long long parent_id, long long represent_node, long long sum) {
		this->parent_id = parent_id;
		this->represent_node = represent_node;
		this->sum = sum;
	}
};

struct NEC_Node {
	long long node;
	NEC_Node * nextAddress = NULL;
};


struct CPINodeSize {

	long long CPI_id_in_query;
	long long size;
	long long degree;
	long long pre_visited_num;
	long long level;

	bool operator <(CPINodeSize right) const {
		
		double a = 0.1;
		double b = 1;
		double c = 1;

		double x = size - right.size;
		double y = degree - right.degree;
		double z = pre_visited_num - right.pre_visited_num;


		double res = a * x + b * y + c * z;

		return res < 0;

	}

	bool operator >(CPINodeSize right) const {

		//long long degree_v = degree + pre_visited_num * 30;
		//long long degree_v_a = a.degree + a.pre_visited_num * 30;

		double a = 0.01;
		double b = 1;
		double c = 30;

		double x = size - right.size;
		double y = degree - right.degree;
		double z = pre_visited_num - right.pre_visited_num;

		
		double res = a * x + b * y + c * z;
		
		return res > 0;

	}

};

struct CPINodeSize2 {

	long long CPI_id;
	long long size;
	long long degree;
	long long pre_visited_num;

	bool operator <(CPINodeSize2 a) const {

		long long degree_v = degree + pre_visited_num * 30;
		long long degree_v_a = a.degree + a.pre_visited_num * 30;

		//if (abs(size - a.size) < 200) {
		//	return degree_v < degree_v_a;
		//}
		return size < a.size;
	}

	bool operator >(CPINodeSize2 right) const {

		//long long degree_v = degree + pre_visited_num * 30;
		//long long degree_v_a = a.degree + a.pre_visited_num * 30;

		double a = 0.01;
		double b = 1;
		double c = -30;
		double x = size - right.size;
		double y = degree - right.degree;
		double z = pre_visited_num - right.pre_visited_num;

		double res = a * x + b * y + c * z;
		return res > 0;

	}

};


struct Result {

	int cost;
	vector<long long> result;

	bool operator <(Result right) const {
		return cost < right.cost;
	}

	bool operator >(Result right) const {
		return cost > right.cost;
	}

};


struct MatchOrderUnit {
	long long node;
	long long pt_index; // parent index
	long long pt_id;
	long long nte_length;
	long long start_pos;
};

struct SearchIterator {
	CPICell * address = NULL;
	long long node_id = -1;
	long long query_id = -1;
	long long address_size;
	long long address_pos;
};


struct CoreQueryBFSTreeNode {
	long long parent_node = -1;
	pair<long long, long long> children; // start_pos, length
	pair<long long, long long> nte; // start_pos, length
	vector<int> * cross_lvl_nte;
};



struct Element {
	long long value;
	Element * nextAddress;
};



class HashTable {// able to handle all kinds of value

private:
	long long length;
	Element * * array;
	long long collison;
	long long keyStored;
	//	int seed;

public:
	HashTable() {
		this->length = HT_LENGTH;
		this->array = new Element *[HT_LENGTH];
		memset(array, NULL, sizeof(void *) * HT_LENGTH);
		//		seed = 131;
		collison = 0;
		keyStored = 0;
	}

	HashTable(long long degree) {
		this->length = degree * 2;
		this->array = new Element *[degree * 2];
		memset(array, NULL, sizeof(void *) * degree * 2);
		//		seed = 131;
		collison = 0;
		keyStored = 0;
	}

	inline long long getCollison() {
		return collison;
	}

	inline long long getKeyStored() {
		return keyStored;
	}

	inline void AddValue(long long node) {
		long long a = node;
		if (node < 0) {
			a = -node;
		}
		long long key = a % length;
		Element * e = array[key];
		if (e) { //exist
				 //no need to check here, because no node has two edges pointing to the same node
			collison++;
			Element * element = new Element();
			element->value = node;
			element->nextAddress = e;
			array[key] = element;
		}
		else { //not exist
			keyStored++;
			Element * element = new Element;
			element->value = node;
			element->nextAddress = NULL;
			array[key] = element;
		} //end if - else
	}


	inline bool query(long long node) {
		long long a = node;
		if (node < 0) {
			a = -node;
		}
		long long key = a % length;
		Element * e = array[key];
		while (e) {
			if (e->value == node)
				return true;
			else
				e = e->nextAddress;
		}
		return false;
	}
};


/*
struct EdgeType {

	int node_id;
	HashTable * edge_hash = new HashTable(1);
	set<long long> edge_set;

	EdgeType(int n_id, set<long long> e_type) {
		// parse edge_type_string
		node_id = n_id;
		edge_set = e_type;
		set<long long>::iterator it;
		for (it = e_type.begin(); it != e_type.end(); ++it) {
			edge_hash->AddValue(*it);
		}
	}

	EdgeType() {

	}

	inline bool is_in_edge_type(EdgeType * e_type) {
		//long long res = edge_type & e_type->edge_type;
		//return res == e_type->edge_type;
		set<long long> * e = &(e_type->edge_set);
		set<long long>::iterator it;
		for (it = e->begin(); it != e->end(); ++it) {
			if (!edge_hash->query(*it)) {
				return false;
			}
		}
		return true;

	}

}; */


#define QUICK

#ifdef QUICK
struct EdgeType{
	static const int TypeSize = 10;
	int node_id;

	int from_to[TypeSize] = {-1};
	int to_from[TypeSize] = { -1 };
	int from_to_size = 0;
	int to_from_size = 0;

	int from_to_index[TypeSize] = { -1 };
	int to_from_index[TypeSize] = { -1 };

	unsigned long long split_(const string &txt, vector<string> &strs, char ch) {
		//this is the general case
		size_t pos = txt.find(ch);
		size_t initialPos = 0;
		strs.clear();
		// Decompose statement
		while (pos != string::npos) {
			strs.push_back(txt.substr(initialPos, pos - initialPos));
			initialPos = pos + 1;
			pos = txt.find(ch, initialPos);
		}
		// Add the last one
		strs.push_back(txt.substr(initialPos, min(pos, txt.size()) - initialPos + 1));
		//return the size of the vector
		return strs.size();
	}

	void build(vector<int> &from_to_info, vector<int> &to_from_info) {
		// ! vector need test
		for (int i = 0; i < from_to_info.size(); i = i + 2) {
			from_to[from_to_info[i]] = from_to_info[i + 1];
			from_to_index[i / 2] = from_to_info[i];
		}
		from_to_size = from_to_info.size() / 2;
		for (int i = 0; i < to_from_info.size(); i = i + 2) {
			to_from[to_from_info[i]] = to_from_info[i + 1];
			to_from_index[i / 2] = to_from_info[i];
		}
		to_from_size = to_from_info.size() / 2;
	}

	EdgeType(int n_id, string & edge_types, bool is_left_to_right) {
		node_id = n_id;
		vector<string> v;
		split_(edge_types, v, ';');

		vector<string> from_to_;
		vector<string> to_from_;

		//! v[0] or v[1] may not exist
		split_(v[0], from_to_, ',');
		split_(v[1], to_from_, ',');

		vector<int> from_to_int;
		vector<int> to_from_int;

		if (from_to_.size() > 1) {
			for (int i = 0; i < from_to_.size(); i++) {
				from_to_int.push_back(atoi(from_to_[i].c_str()));
			}
		}

		if (to_from_.size() > 1) {
			for (int i = 0; i < to_from_.size(); i++) {
				to_from_int.push_back(atoi(to_from_[i].c_str()));
			}
		}

		if (is_left_to_right) {
			build(from_to_int, to_from_int);
		}
		else {
			build(to_from_int, from_to_int);
		}
	}


	EdgeType() {

	}


	// this one is data, the parameter is query
	inline bool is_in_edge_type(EdgeType * e_type) {

		//return true;
		// e_type should be in this

		if (from_to_size < e_type->to_from_size || to_from_size < e_type->from_to_size) {
			return false;
		}

		// hashmap iterator need test

		for (int i = 0; i < TypeSize; i++) {
			int eType = e_type->from_to_index[i];
			if (eType == -1) {
				break;
			}
			int count = e_type->from_to[eType];

			if (to_from[eType] < count) {
				return false;
			}
		}

		for (int i = 0; i < TypeSize; i++) {
			int eType = e_type->to_from_index[i];
			if (eType == -1) {
				break;
			}
			int count = e_type->to_from[eType];

			if (from_to[eType] < count) {
				return false;
			}
		}

		return true;

	}

}
;


#else

struct EdgeType {

	int node_id;
	// key is eType, value is count
	unordered_map<int, int> from_to;
	int from_to_size;
	unordered_map<int, int> to_from;
	int to_from_size;


	unsigned long long split_(const string &txt, vector<string> &strs, char ch) {
		//this is the general case
		size_t pos = txt.find(ch);
		size_t initialPos = 0;
		strs.clear();
		// Decompose statement
		while (pos != string::npos) {
			strs.push_back(txt.substr(initialPos, pos - initialPos));
			initialPos = pos + 1;
			pos = txt.find(ch, initialPos);
		}
		// Add the last one
		strs.push_back(txt.substr(initialPos, min(pos, txt.size()) - initialPos + 1));
		//return the size of the vector
		return strs.size();
	}

	void build(vector<int> &from_to_info, vector<int> &to_from_info) {
		// ! vector need test
		for (int i = 0; i < from_to_info.size(); i = i + 2) {
			from_to[from_to_info[i]] = from_to_info[i + 1];
		}
		from_to_size = from_to_info.size() / 2;
		for (int i = 0; i < to_from_info.size(); i = i + 2) {
			to_from[to_from_info[i]] = to_from_info[i + 1];
		}
		to_from_size = to_from_info.size() / 2;
	}

	EdgeType(int n_id, string & edge_types, bool is_left_to_right) {
		node_id = n_id;
		vector<string> v;
		split_(edge_types, v, ';');

		vector<string> from_to_;
		vector<string> to_from_;

		//! v[0] or v[1] may not exist
		split_(v[0], from_to_, ',');
		split_(v[1], to_from_, ',');

		vector<int> from_to_int;
		vector<int> to_from_int;

		if (from_to_.size() > 1) {
			for (int i = 0; i < from_to_.size(); i++) {
				from_to_int.push_back(atoi(from_to_[i].c_str()));
			}
		}

		if (to_from_.size() > 1) {
			for (int i = 0; i < to_from_.size(); i++) {
				to_from_int.push_back(atoi(to_from_[i].c_str()));
			}
		}

		if (is_left_to_right) {
			build(from_to_int, to_from_int);
		}
		else {
			build(to_from_int, from_to_int);
		}
	}

	EdgeType(int n_id, EdgeType &et) {
		node_id = n_id;
		this->from_to = et.to_from;
		this->to_from = et.from_to;
		this->from_to_size = et.to_from_size;
		this->to_from_size = et.from_to_size;
	}
	/*
	EdgeType(int n_id, vector<int> &from_to_info, vector<int> &to_from_info) {
	// parse edge_type_string
	node_id = n_id;
	// ! vector need test
	for (int i = 0; i < from_to_info.size(); i = i + 2) {
	from_to[from_to_info[i]] = from_to_info[i + 1];
	}
	from_to_size = from_to_info.size() / 2;
	for (int i = 0; i < to_from_info.size(); i = i + 2) {
	to_from[to_from_info[i]] = to_from_info[i + 1];
	}
	to_from_size = to_from_info.size() / 2;
	}
	*/

	EdgeType() {

	}

	// this one is data, the parameter is query
	inline bool is_in_edge_type(EdgeType * e_type) {
		// e_type should be in this

		if (from_to_size < e_type->to_from_size || to_from_size < e_type->from_to_size) {
			return false;
		}

		// hashmap iterator need test

		for (auto it = e_type->from_to.begin(); it != e_type->from_to.end(); ++it) {
			int eType = it->first;
			int count = it->second;

			if (to_from.find(eType) == to_from.end()) {
				return false;
			}

			if (to_from[eType] < count) {
				return false;
			}

		}

		for (auto it = e_type->to_from.begin(); it != e_type->to_from.end(); ++it) {
			int eType = it->first;
			int count = it->second;

			if (from_to.find(eType) == from_to.end()) {
				return false;
			}

			if (from_to[eType] < count) {
				return false;
			}
		}

		return true;

	}

};

#endif

#endif // !DATASTRUCTURE_HEAD

