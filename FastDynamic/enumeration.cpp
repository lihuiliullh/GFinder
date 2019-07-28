#include "stdafx.h"
#include "enumeration.h"


SearchIterator search_iterator[MAX_QUERY_NODE];
long long actual_mapping_tree[MAX_QUERY_NODE];

SearchIterator search_unit_tree_iterator[MAX_QUERY_NODE];
pair<long long, pair<long long, double> >  NEC_set_ranking[MAX_QUERY_NODE];
pair <long long, long long>  v_nec_count[MAX_QUERY_NODE];
long long nec_count_set_size = -1;
long long  nec_count_set[MAX_QUERY_NODE];

extern long long m_already_mapped = 0;
extern long long m_back_trace = 0;