#include"stdafx.h"

#include<iostream>
#include<istream>
#include<vector>
#include<string>
#include<fstream>
#include<string.h>
#include"global_function.h"
#include"global_variable.h"

using namespace std;

void transfer_TurboISOQuery_to_myquery(string querygraphFileFolder) {

	ifstream fin(querygraphFileFolder);
	string line;
	vector<string> v;
	double degree_query = 0;
	long long id = 0;
	long long node_index = 0;
	long long cnt_node_query = 0;
	long long nodes_label_query[380];
	vector<vector<long long> > nodes_query;

	getline(fin, line);

	while (getline(fin, line)) {

		if (line.at(0) == 't') {

			cout << "t " << id << " " << cnt_node_query << " " << degree_query * 2 << endl;
			for (long long i = 0; i < cnt_node_query; i++) {
				cout << i << " " << nodes_label_query[i] << " " << nodes_query[i].size() << " ";
				for (long long j = 0; j < nodes_query[i].size(); j++) {
					cout << nodes_query[i][j] << " ";
				}
				cout << endl;
			}

			nodes_query.clear();
			node_index = 0;
			cnt_node_query = 0;
			degree_query = 0;
			id++;
		}

		if (line.at(0) == 'v') {
			vector <long long> node;
			nodes_query.push_back(node);
			split(line, v, ' ');
			long long label = atoi(v[2].c_str());
			nodes_label_query[node_index] = label;
			node_index++;
			cnt_node_query++;
		}

		if (line.at(0) == 'e') {
			split(line, v, ' ');
			long long int_node_left = atoi(v[1].c_str());
			long long int_node_right = atoi(v[2].c_str());
			//put the nodes into the adjacent list
			nodes_query[int_node_left].push_back(int_node_right);
			nodes_query[int_node_right].push_back(int_node_left);
			degree_query++;
		}

	}
	cout << "t " << id << " " << cnt_node_query << " " << degree_query * 2 << endl;
	for (long long i = 0; i < cnt_node_query; i++) {
		cout << i << " " << nodes_label_query[i] << " " << nodes_query[i].size() << " ";
		for (long long j = 0; j < nodes_query[i].size(); j++)
			cout << nodes_query[i][j] << " ";
		cout << endl;
	}

	fin.close();
}