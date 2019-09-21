#pragma once

#ifndef QUERY_AUGMENT_EXPERIMENT_
#define QUERY_AUGMENT_EXPERIMENT_
#include "vector"
#include <set> 
#include <cstdlib>      // std::rand, std::srand
#include <algorithm>    // std::random_shuffle
#include <ctime>        // std::time


using namespace std;


// random sample

inline void randomSample(vector<int> &arr, int sampleSize1) {
	srand(unsigned(std::time(0)));
	random_shuffle(arr.begin(), arr.end());
	if (arr.size() > sampleSize1) {
		arr.resize(sampleSize1);
	}
}

inline void statisticCandidateSize_single_direct(CPINode* indexSet_) {

	for (int i = 0; i < g_core_size; i++) {
		if (g_core_number_query_graph[i] < 2) continue;
		for (int j = i + 1; j < g_core_size; j++) {
			if (g_core_number_query_graph[j] < 2) continue;

			// if these two nodes are connected at query graph
			if (1 == g_query_adj_matrix[i * g_cnt_node_query_graph + j]) continue;


			CPINode* node1 = &indexSet_[i];
			CPINode* node2 = &indexSet_[j];
			
			// iterate all candidate
			int* candis_1 = node1->candidates;
			int* candis_2 = node2->candidates;

			int size1 = node1->size;
			int size2 = node2->size;

			vector<int> v1;
			vector<int> v2;

			for (int p1 = 0; p1 < size1; p1++) {
				if (candis_1[p1] < 0) continue;
				v1.push_back(candis_1[p1]);
			}

			for (int p2 = 0; p2 < size2; p2++) {
				if (candis_2[p2] < 0) continue;
				v2.push_back(candis_2[p2]);
			}

			int v1_size = v1.size();
			int v2_size = v2.size();

			vector<int>::iterator iter1 = v1.begin();
			while (iter1 != v1.end()) {
				vector<int>::iterator iter2 = v2.begin();
				bool flag = false;

				while (iter2 != v2.end()) {
					int data1 = *iter1;
					int data2 = *iter2;

					// if connect
					if (SIZEOK == 1) {
						if (g_data_edge_matrix[data1 * SIZE_OF_EDGE_MATRIX + data2] != 0 || g_data_edge_matrix[data2 * SIZE_OF_EDGE_MATRIX + data1]) {
							// remove
							iter2 = v2.erase(iter2);
							flag = true;
						}
						else {
							++iter2;
						}
					}
					else {
						if (g_data_edge_matrix[(data1 % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (data2 % SIZE_OF_EDGE_MATRIX)] != 0 ||
							g_data_edge_matrix[(data2 % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (data1 % SIZE_OF_EDGE_MATRIX)] != 0) {
							// remove
							flag = true;
							iter2 = v2.erase(iter2);
						}
						else {
							//validate the non tree edge by querying the edgeIndex
							if (g_hash_table[data1]->query(data2) || g_hash_table[data2]->query(data1)) {
								// remove
								flag = true;
								iter2 = v2.erase(iter2);
							}
							else {
								++iter2;
							}
						}
					}


				}
				if (flag == true) {
					iter1 = v1.erase(iter1);
				}
				else {
					++iter1;
				}
				
			}

			cout << i << " " << j << " " << "before: " << v1_size << " " << v2_size << " after: " << v1.size() << " " << v2.size() << endl;

		}
	}

}


inline void statisticCandidateSize_(CPINode* indexSet_) {

	for (int i = 0; i < g_core_size; i++) {
		if (g_core_number_query_graph[i] < 2) continue;
		for (int j = i + 1; j < g_core_size; j++) {
			if (g_core_number_query_graph[j] < 2) continue;

			// if these two nodes are connected at query graph
			if (1 == g_query_adj_matrix[i * g_cnt_node_query_graph + j]) continue;


			CPINode* node1 = &indexSet_[i];
			CPINode* node2 = &indexSet_[j];

			// iterate all candidate
			int* candis_1 = node1->candidates;
			int* candis_2 = node2->candidates;

			int size1 = node1->size;
			int size2 = node2->size;

			vector<int> v1;
			vector<int> v2;

			for (int p1 = 0; p1 < size1; p1++) {
				if (candis_1[p1] < 0) continue;
				v1.push_back(candis_1[p1]);
			}

			for (int p2 = 0; p2 < size2; p2++) {
				if (candis_2[p2] < 0) continue;
				v2.push_back(candis_2[p2]);
			}

			vector<int> v3;
			vector<int> v4;

			int v1_origin_size = v1.size();
			int v2_origin_size = v2.size();

			vector<int>::iterator iter1 = v1.begin();
			while (iter1 != v1.end()) {
				vector<int>::iterator iter2 = v2.begin();
				bool flag = false;

				while (iter2 != v2.end()) {
					int data1 = *iter1;
					int data2 = *iter2;

					// if connect
					if (SIZEOK == 1) {
						if (g_data_edge_matrix[data1 * SIZE_OF_EDGE_MATRIX + data2] != 0 || g_data_edge_matrix[data2 * SIZE_OF_EDGE_MATRIX + data1]) {
							// remove
							v4.push_back(*iter2);
							iter2 = v2.erase(iter2);
							flag = true;
						}
						else {
							++iter2;
						}
					}
					else {
						if (g_data_edge_matrix[(data1 % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (data2 % SIZE_OF_EDGE_MATRIX)] != 0 ||
							g_data_edge_matrix[(data2 % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (data1 % SIZE_OF_EDGE_MATRIX)] != 0) {
							// remove
							flag = true;
							v4.push_back(*iter2);
							iter2 = v2.erase(iter2);
						}
						else {
							//validate the non tree edge by querying the edgeIndex
							if (g_hash_table[data1]->query(data2) || g_hash_table[data2]->query(data1)) {
								// remove
								flag = true;
								v4.push_back(*iter2);
								iter2 = v2.erase(iter2);
							}
							else {
								++iter2;
							}
						}
					}


				}
				if (flag == true) {
					iter1 = v1.erase(iter1);
				}
				else {
					++iter1;
				}

			}

			iter1 = v4.begin();
			while (iter1 != v4.end()) {
				vector<int>::iterator iter2 = v1.begin();
				bool flag = false;

				while (iter2 != v1.end()) {
					int data1 = *iter1;
					int data2 = *iter2;

					// if connect
					if (SIZEOK == 1) {
						if (g_data_edge_matrix[data1 * SIZE_OF_EDGE_MATRIX + data2] != 0 || g_data_edge_matrix[data2 * SIZE_OF_EDGE_MATRIX + data1]) {
							// remove
							iter2 = v1.erase(iter2);
						}
						else {
							++iter2;
						}
					}
					else {
						if (g_data_edge_matrix[(data1 % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (data2 % SIZE_OF_EDGE_MATRIX)] != 0 ||
							g_data_edge_matrix[(data2 % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (data1 % SIZE_OF_EDGE_MATRIX)] != 0) {
							// remove
							iter2 = v1.erase(iter2);
						}
						else {
							//validate the non tree edge by querying the edgeIndex
							if (g_hash_table[data1]->query(data2) || g_hash_table[data2]->query(data1)) {
								// remove
								iter2 = v1.erase(iter2);
							}
							else {
								++iter2;
							}
						}
					}


				}
				
				++iter1;

			}



			cout << i << " " << j << " " << "before: " << v1_origin_size << " " << v2_origin_size << " after: " << v1.size() << " " << v2_origin_size - v4.size() << endl;

		}
	}

}




inline void statisticCandidateSize(CPINode* indexSet_) {

	for (int i = 0; i < g_core_size; i++) {
		if (g_core_number_query_graph[i] < 2) continue;
		for (int j = i + 1; j < g_core_size; j++) {
			if (g_core_number_query_graph[j] < 2) continue;

			// if these two nodes are connected at query graph
			if (1 == g_query_adj_matrix[i * g_cnt_node_query_graph + j]) continue;


			CPINode* node1 = &indexSet_[i];
			CPINode* node2 = &indexSet_[j];

			// iterate all candidate
			int* candis_1 = node1->candidates;
			int* candis_2 = node2->candidates;

			int size1 = node1->size;
			int size2 = node2->size;

			set <int> set1;
			set <int> set2;

			// just for debugging, see how many useful candidates
			vector<int> v1;
			vector<int> v2;

			for (int p1 = 0; p1 < size1; p1++) {
				if (candis_1[p1] < 0) continue;
				v1.push_back(candis_1[p1]);
			}

			for (int p2 = 0; p2 < size2; p2++) {
				if (candis_2[p2] < 0) continue;
				v2.push_back(candis_2[p2]);
			}
			randomSample(v1, 1000);
			randomSample(v2, 1000);

			int v1_size = v1.size();
			int v2_size = v2.size();
			//////////////////////////////

			for (int idx1 = 0; idx1 < v1_size; idx1++) {
				bool flag = false;
				int data1 = v1[idx1];
				//if (data1 < 0) continue;

				for (int idx2 = 0; idx2 < v2_size; idx2++) {
					int data2 = v2[idx2];
					//if (data2 < 0) continue;

					// if connect
					if (SIZEOK == 1) {
						if (g_data_edge_matrix[data1 * SIZE_OF_EDGE_MATRIX + data2] != 0 || g_data_edge_matrix[data2 * SIZE_OF_EDGE_MATRIX + data1]) {
							// remove
							set2.insert(data2);
							flag = true;
						}
					}
					else {
						if (g_data_edge_matrix[(data1 % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (data2 % SIZE_OF_EDGE_MATRIX)] != 0 ||
							g_data_edge_matrix[(data2 % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (data1 % SIZE_OF_EDGE_MATRIX)] != 0) {
							// remove
							flag = true;
							set2.insert(data2);
						}
						else {
							//validate the non tree edge by querying the edgeIndex
							if (g_hash_table[data1]->query(data2) || g_hash_table[data2]->query(data1)) {
								// remove
								flag = true;
								set2.insert(data2);
							}
						}
					}
				}

				if (flag) {
					set1.insert(data1);
				}
			}

			cout << i << " " << j << " " << "before: " << v1_size << " " << v2_size << " connect: " << set1.size() << " " << set2.size() << endl;

		}
	}

}


inline void printIndexTrueSize(CPINode* indexSet_) {
	set<int> res;
	for (int i = 0; i < g_core_size; i++) {
		if (g_core_number_query_graph[i] < 2) continue;

		CPINode* node1 = &indexSet_[i];
		// iterate all candidate
		int* candis_1 = node1->candidates;
		int size1 = node1->size;

		// just for debugging, see how many useful candidates
		vector<int> v1;

		for (int p1 = 0; p1 < size1; p1++) {
			if (candis_1[p1] < 0) continue;
			v1.push_back(candis_1[p1]);
			res.insert(candis_1[p1]);
		}

		cout << i << " " << v1.size() << endl;
	}
	set<int>::iterator it;
	for (it = res.begin(); it != res.end(); it++)
	{
		cout << *it << ",";
	}
	cout << endl;

}



inline void disableSomeIndexSet_keepConnect(CPINode* indexSet_, int index1, int index2, bool keepConnect) {
	for (int i = 0; i < g_core_size; i++) {
		if (g_core_number_query_graph[i] < 2) continue;
	
		CPINode* node1 = &indexSet_[i];
		// iterate all candidate
		int* candis_1 = node1->candidates;
		int size1 = node1->size;

		// just for debugging, see how many useful candidates
		vector<int> v1;

		for (int p1 = 0; p1 < size1; p1++) {
			if (candis_1[p1] < 0) continue;
			v1.push_back(candis_1[p1]);
		}

		cout << i << " " << v1.size() << endl;
	}



	CPINode* node1 = &indexSet_[index1];
	CPINode* node2 = &indexSet_[index2];

	// iterate all candidate
	int* candis_1 = node1->candidates;
	int* candis_2 = node2->candidates;

	int size1 = node1->size;
	int size2 = node2->size;

	set <int> set1;
	set <int> set2;


	for (int idx1 = 0; idx1 < size1; idx1++) {
		bool flag = false;
		int data1 = candis_1[idx1];
		if (data1 < 0) continue;

		for (int idx2 = 0; idx2 < size2; idx2++) {
			int data2 = candis_2[idx2];
			if (data2 < 0) continue;

			// if connect
			if (SIZEOK == 1) {
				if (g_data_edge_matrix[data1 * SIZE_OF_EDGE_MATRIX + data2] != 0 || g_data_edge_matrix[data2 * SIZE_OF_EDGE_MATRIX + data1]) {
					// remove
					set2.insert(data2);
					flag = true;
				}
			}
			else {
				if (g_data_edge_matrix[(data1 % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (data2 % SIZE_OF_EDGE_MATRIX)] != 0 ||
					g_data_edge_matrix[(data2 % SIZE_OF_EDGE_MATRIX) * SIZE_OF_EDGE_MATRIX + (data1 % SIZE_OF_EDGE_MATRIX)] != 0) {
					// remove
					flag = true;
					set2.insert(data2);
				}
				else {
					//validate the non tree edge by querying the edgeIndex
					if (g_hash_table[data1]->query(data2) || g_hash_table[data2]->query(data1)) {
						// remove
						flag = true;
						set2.insert(data2);
					}
				}
			}
		}

		if (flag) {
			set1.insert(data1);
		}
	}


	if (keepConnect) {
		// make candidates -1
		for (int idx1 = 0; idx1 < size1; idx1++) {
			int data1 = candis_1[idx1];
			if (data1 < 0) continue;
			// have edge
			if (set1.find(data1) == set1.end()) {
				candis_1[idx1] = -1;
			}
		}

		for (int idx1 = 0; idx1 < size2; idx1++) {
			int data1 = candis_2[idx1];
			if (data1 < 0) continue;

			if (set2.find(data1) == set2.end()) {
				candis_2[idx1] = -1;
			}
		}
	}
	else {
		for (int idx1 = 0; idx1 < size1; idx1++) {
			int data1 = candis_1[idx1];
			if (data1 < 0) continue;
			// have edge
			if (set1.find(data1) != set1.end()) {
				candis_1[idx1] = -1;
			}
		}

		for (int idx1 = 0; idx1 < size2; idx1++) {
			int data1 = candis_2[idx1];
			if (data1 < 0) continue;

			if (set2.find(data1) != set2.end()) {
				candis_2[idx1] = -1;
			}
		}
	}
}



#endif

