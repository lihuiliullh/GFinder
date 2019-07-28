#pragma once
#ifndef FAST_DYNAMIC_TIME_H_
#define FAST_DYNAMIC_TIME_H_

#include "global_variable.h"

inline void initializeStatisticParameters() {
	g_time_search = 0;
	g_time_process = 0;
	g_time_simulation = 0;
	g_time_prepare = 0;
	g_time_preliminary = 0;
	g_time_readQuery = 0;
	g_time_getSequence = 0;
	g_time_rest = 0;
	g_time_query = 0;
	g_time_total = 0;
}


#endif // !FAST_DYNAMIC_TIME_H_
