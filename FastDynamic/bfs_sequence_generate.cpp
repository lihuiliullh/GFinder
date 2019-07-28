#include "stdafx.h"
#include "bfs_sequence_generate.h"

vector < pair<long long, long long> > level_index;
long long BFS_level_query[MAX_QUERY_NODE];
long long BFS_parent_query[MAX_QUERY_NODE];
long long BFS_sequence_query_of_all[MAX_QUERY_NODE];
long long iterateCoreSequence[MAX_QUERY_NODE];
