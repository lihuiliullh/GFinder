// FastDynamic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <ostream>
#include <iostream>
#include <cmath>
#include <time.h>
#include "transform_query_file.h"
#include "pre_process_data_graph.h"
#include "time.h"
#include "utility.h"
#include "cfl_decomposition.h"
#include "seed_finder.h"
#include "cpi_builder.h"
#include "matching_order.h"
#include "enumeration.h"
#include "sim_base_build_cpi.h"
#include "core_query_tree_build.h"
#include "inexact_matching_order.h"
#include "find_result.h"

using namespace std;


CUtility * cu_total = new CUtility();
CUtility *cu_readQuery = new CUtility();
CUtility *cu_prepare = new CUtility();
CUtility *cu_simulation = new CUtility();
CUtility *cu_getSequence = new CUtility();
CUtility *cu_querying = new CUtility();

#define TOTAL_BEGIN  cu_total->startCT();
#define TOTAL_END g_time_total = cu_total->endCT(false);

#define READ_QUERY_BEGIN cu_readQuery->startCT();
#define READ_QUERY_END g_time_readQuery = cu_readQuery->endCT(false);

#define PREPARE_BEGIN cu_prepare->startCT();
#define PREPARE_END g_time_prepare = cu_prepare->endCT(false);

#define SIMULATION_BEGIN cu_simulation->startCT();
#define SIMULATION_END g_time_simulation = cu_simulation->endCT(false);

#define GET_SEQ_BEGIN  cu_getSequence->startCT();
#define GET_SEQ_END g_time_getSequence = cu_getSequence->endCT(false);

#define SEARCH_BEGIN cu_querying->startCT();
#define SEARCH_END g_time_search = cu_querying->endCT(false);


void getLimit_full(string str_full_limit, long & LIMIT) {
	if (str_full_limit == "1K")
		LIMIT = 1000;
	else if (str_full_limit == "10K")
		LIMIT = 10000;
	else if (str_full_limit == "100K")
		LIMIT = 100000;
	else if (str_full_limit == "100M")
		LIMIT = 100000000;
	else if (str_full_limit == "1B")
		LIMIT = 100000000000;
	else
		LIMIT = atol(str_full_limit.c_str());
}


void printVectorInt(vector<int> &vet) {
	for (vector<int>::iterator pos = vet.begin(); pos != vet.end(); pos++) {
		printf("%d ", *pos);
	}
	putchar('\n');
}

void myy(vector<int> &a) {
	for (int i = 0; i < a.size(); i++) {
		cout << i;
	}
}


inline bool is_contain_empty_indexset() {
	for (int step = 0; step < g_core_size; step++) {
		CPINode* tmp_node = &indexSet[step];
		if (tmp_node->size == 1 && tmp_node->candidates[0] == -1) {
			return true;
		}
	}
	return false;
}

int main(int argc, char *argv[])
{

	// use for directed graph

	

	argv[1] = "C:\\Users\\lihui\\workspace\\github\\test_dataset\\data.format";
	argv[2] = "C:\\Users\\lihui\\workspace\\github\\test_dataset\\query.format";


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	argv[3] = "-f";
	argv[4] = "1";
	argv[5] = "1";


	// top 10
	TOPK = 5;
	NODE_MISS_COST = 10;
	BRIDGE_COST = 10;
	EDGE_MISSING_COST = 10;
	DYNAMIC = true;
	// IS_ONE_HOP_DATA_GRAPH should be equl to 1 or 0
	IS_ONE_HOP_DATA_GRAPH = 1;
	DISTINCT_LABEL = 0;

	string data_graph_file = argv[1];
	string query_graph_file = argv[2];

	//TOPK = stoi(argv[3]);
	//NODE_MISS_COST = stoi(argv[4]);
	//BRIDGE_COST = stoi(argv[5]);
	//EDGE_MISSING_COST = stoi(argv[6]);
	//IS_ONE_HOP_DATA_GRAPH = stoi(argv[7]);


	cout << endl << "*******************************************************************" << endl;
	cout << "Data File :" << argv[1] << endl;
	cout << "Query file:" << argv[2] << endl;
	cout << "Top-K: " << TOPK << endl;
	cout << "Missing Node Cost: " << NODE_MISS_COST << endl;
	cout << "Intermediate Cost: " << BRIDGE_COST << endl;
	cout << "Missing Edge Cost: " << EDGE_MISSING_COST << endl;
	cout << "Is One Hop: " << IS_ONE_HOP_DATA_GRAPH << endl;
	cout << "*******************************************************************" << endl;

	// ======= Begin clock ========
	g_clock = clock();

	//==== FIRST step: read and process the data graph ============================
	cout << "Reading data graph ..." << endl;
	preprocessDataGraph(data_graph_file);
	cout << "processing data graph ..." << endl;
	readAndProcessDataGraph(data_graph_file);
	initNECStructure();
	cout << "OK!" << endl;

	cout << "Building index..." << endl;
	cout << "Begin parametersInitilisingBeforeQuery..." << endl;
	initParametersBeforeQuery();
	cout << "Begin initializeStatisticParameters..." << endl;
	initializeStatisticParameters();
	cout << "OK!" << endl;
	//=======FIRST step END =====================================================


	cout << "ATTENTION: Finish data index cost: " << (clock() - g_clock) * 1.0 / CLOCKS_PER_SEC << endl;

	int count_query_file = atoi(argv[4]);
	count_query_file = 1;
	//string str_full_limit = argv[5];

	//getLimit_full(str_full_limit, LIMIT);

	char c;
	int query_id;

	for (long long i = 0; i < count_query_file; i++) {
		// clean heap here
		{
			g_result_heap.clear();
			g_maxPartialNum = 0;
			g_mapping_found = 0;
		}

		TOTAL_BEGIN;

		single_readQueryGraph(query_graph_file);

		PREPARE_BEGIN;

		coreDecompositionQuery();
		cout << "Decompose Query OK!" << endl;
		g_isTree = true;
		for (long long i = 0; i < g_cnt_node_query_graph; i++) {
			if (g_core_number_query_graph[i] >= 2) {
				g_isTree = false;
				break;
			}
		}

		if (g_isTree) {
			
		}
		else {
			// extract residual tree and NEC from query
			extractResidualStructures();
			g_root_node_id_of_query = selectRootFromQuery();
			//g_root_node_id_of_query = 1;
			findRootCandidate();
			int root_index_point = -1;
			while (g_root_candidates_size == 0) {
				root_index_point = root_index_point + 1;
				if (root_index_point == query_root_sort_list.size()) {
					cout << "All node in core has no candidate!" << endl;
					return 0;
				}
				g_root_node_id_of_query = query_root_sort_list[root_index_point] % ONE_M;
				findRootCandidate();
			}
			
			PREPARE_END;

			for (int region = 0; region < g_root_candidates_size; region++) {			

				long long root_cand_id = g_root_candidates[region];

				g_nte_array_for_matching_unit_index = 0;
				g_matching_sequence_index = 0;
				g_matching_order_size_of_core = 0;
				
				// BFS method
				if (!DYNAMIC) {
					buildBFSCoreQueryTree();
					buildBFSTreeCPI(root_cand_id);
					//backwardPrune();
					generateInexactMatchingOrder();
				}
				else {
					buildDynamicTreeCPI(root_cand_id);
					for (int ii = 0; ii < 20; ii++) {
						backwardPrune();

						//if (is_contain_empty_indexset()) {
						//	break;
						//}
					}
					
					// buildCoreQueryTree();
					generateMatchingOrderByDynamic();
				}

				// print correct partial match
				cout << "Exact Partial Match: ";
				for (int step = 0; step < g_core_size; step++) {
					CPINode* tmp_node = &indexSet[step];
					if (tmp_node->size == 1) {
						cout << step << ":" << tmp_node->candidates[0] << " ";
					}
				}
				cout << endl;

				/*if (g_core_size != g_cnt_node_query_graph) {
					continue;
				}*/

				// Dynamic method
				//forwardBuildQueue(root_cand_id);
				
				//buildDynamicTreeCPI(root_cand_id);
				//forwardBuild(root_cand_id);
				//buildCoreQueryTree();
				// build core query tree

				//cout << "forward Done" << endl;
				
				//cout << "backward Done" << endl;
				//buildSearchIterator();
				//cout << "build search iterator Done" << endl;
				//generateMatchingOrder();
				//generateInexactMatchingOrder();
				//generateMatchingOrderByCoreQueryTree();
				//matchingOrderLayer();
				//test1();
				//cout << "generateMatchingOrder Done" << endl;				
				//findAllMatching();
				find_inexact_result();
				
				int aaa = 0;
			}

		}
		output_result();
		cout << "ATEENTION: Cost: " << (clock() - g_clock) * 1.0 / CLOCKS_PER_SEC << endl;
	}


	//fin_query.close();
	system("pause");
    return 0;
}

