#ifndef UTILITY_H
#define UTILITY_H


#include <time.h>
#include <iostream>
#include <stdio.h>
#include <limits>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>



#ifndef DIMENSION
#define DIMENSION 2
#endif


#define SPACE_MAX 0.999999
#define EPSILON 0.0001
//#define CGlobal::GRID_WIDTH 64
#define RECTLEN 0.001
// #define NDEBUG


//int CGlobal::GRID_WIDTH=64;
// #define ICDE07
// #define STATISTIC

#define STEP_STA 200

#define MAX_DATA_VALUE  100000



#include <Windows.h>
#include <stdint.h> // portable: uint64_t   MSVC: __int64 

// MSVC defines this in winsock2.h!?
//typedef struct timeval {
//	long tv_sec;
//	long tv_usec;
//} timeval;

int gettimeofday(struct timeval * tp, struct timezone * tzp);



/**
@author Ying Zhang, Phd Student Student Room K17-501-7
*/
using namespace std;
class CUtility {
public:
	CUtility();
	~CUtility();
public:

	static long leafIO;
	static long indexIO;

	static long gambling;
	static long gamblingtrial;

	static long filter_rtree;
	static long filter_inobj; // filter at the half objects level
	static long filter_endobj; // filter at the half objects level

	static long low_prune;
	static long upper_prune;

	static long filter_list;
	static long filter_backword;
	static long filter_sky;
	static long u_count;
	static long round;
	static bool countSpace;


	static long numExtraskylines;
	static int* getCombination(const int N, const int K, long& length);
	static bool printCombination(const int N, const int K, char* filename);
	static double combNum(const int N, const int K);

	static void showCurTime(FILE* stream = stderr);
	static void  findOptFile(const char filename[], char optfile[], int k);

public:
	// begin to counting
	inline void startCT();
	// stop counting
	inline double endCT(const bool show = true);

	// stop counting time
	inline void suspend();
	// begin to count the time again
	inline void resume();

private:
	struct timeval qStart;
	struct timeval qEnd;

	struct timeval qS;
	struct timeval qR;
	// the time suspended
	double m_suspended;

};

class CGlobal
{
public:
	static CUtility* time;
	static CUtility* heaptime;
	static CUtility* rtreetime;
	static CUtility* comparetime;
	static CUtility* sorttime;
	static CUtility* gamblingtime;
	static CUtility* listtime;
	static CUtility* rtreequerytime;
	static CUtility* maintain;
	static CUtility* m_preprocess;

	static bool flag;
	static bool countSpace;
	static double IO_Time;
	static long global_IO;
	static long global_IO_Buff;
	static int GRID_WIDTH;
};

// begin to counting
inline void CUtility::startCT()
{
	m_suspended = 0.0;
	gettimeofday(&qStart, NULL);
}


// stop counting
inline double CUtility::endCT(const bool show)
{
	gettimeofday(&qEnd, NULL);
	double qTime = (double)(qEnd.tv_usec - qStart.tv_usec) / 1000 +
		(double)(qEnd.tv_sec - qStart.tv_sec) * 1000;

	qTime -= m_suspended;

	if (show)
		fprintf(stderr, "%.3lf #Process Time (ms) ( %.2lf s)\n", qTime, qTime / 1000);

	return qTime;
}

inline void CUtility::suspend()
{
	gettimeofday(&qS, NULL);
}
// begin to count the time again
inline void CUtility::resume()
{
	gettimeofday(&qR, NULL);
	double qTime = (double)(qR.tv_usec - qS.tv_usec) / 1000 +
		(double)(qR.tv_sec - qS.tv_sec) * 1000;

	m_suspended += qTime;
}


#endif
