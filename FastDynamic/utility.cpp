#include "stdafx.h"

#include "utility.h"
#include <string>
#include <iostream>
#include <fstream>
#include <assert.h>

using namespace std;

CUtility* CGlobal::time = new CUtility();
CUtility* CGlobal::heaptime = new CUtility();
CUtility* CGlobal::rtreetime = new CUtility();
CUtility* CGlobal::comparetime = new CUtility();
CUtility* CGlobal::gamblingtime = new CUtility();
CUtility* CGlobal::sorttime = new CUtility();
CUtility* CGlobal::listtime = new CUtility();
CUtility* CGlobal::rtreequerytime = new CUtility();
CUtility* CGlobal::maintain = new CUtility();
CUtility* CGlobal::m_preprocess = new CUtility();

// bool CGlobal::flag = false;
// bool CGlobal::countSpace = false;
// 
// long CUtility::leafIO = 0;
// long CUtility::indexIO = 0;
// long CUtility::gambling = 0;
// long CUtility::gamblingtrial = 0;
// long CUtility::numExtraskylines = 0;
// long CUtility::filter_rtree = 0;
// long CUtility::filter_list = 0;
// long CUtility::filter_backword = 0;
// long CUtility::filter_sky = 0;
// long CUtility::filter_inobj = 0;
// long CUtility::filter_endobj = 0;
// 
// long CUtility::low_prune = 0;
// long CUtility::upper_prune = 0;
// 
// 
// long CUtility::u_count = 0;
// 
// 
// long CUtility::round = 0;

CUtility::CUtility()
	:m_suspended(0.0)
{
}

CUtility::~CUtility()
{
}


int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
	// until 00:00:00 January 1, 1970 
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}

/*
void findCombine( int data[], int result[],int& index , int depth , const int N
,const int k )
{
if ( depth == k )
{
//check
int pre = -1;
for ( int i=0; i< k; i++ )
if ( data[i] <= pre )
return;
else
pre = data[i];

for ( int i=0; i< k; i++ )
{
result[index++] = data[i];
}
}
else
for ( int i=0; i< N; i++ )
{
data[depth] = i;
findCombine( data, result, index, depth+1 , N, k  );
}
}


long factorial( long N )
{
long num = 1;
while ( N != 0 )
num*= N --;
return num;
}

void CUtility::findOptFile(const char filename[], char optfile[], int k )
{
string s(filename);
int pos=0;
pos =  s.find_last_of("/");
string s1( s.substr(0, pos+1) );

string s2( s.substr(pos+1));
// remove the surfix

pos = s2.find_last_of(".");
string s3 ( s2.substr(0,pos )) ;

sprintf( optfile, "%sopt/%s_%d.txt" , s1.c_str(), s3.c_str(), k );

// cerr << optfile << endl;

}

double CUtility::combNum( const int N , const int K )
{
double sum=1;
long step = N;
while ( step > N-K )
sum*=step--;

long kf= factorial(K);
return sum/kf;
}

int* CUtility::getCombination(const int N, const int K, long& length )
{
int* data = new int[K];
int  depth = 0;
// length =  factorial(N)/( factorial(K)* factorial(N-K)) ;
length = (long)combNum( N, K);
// cerr << length << endl;
// cerr << "COM_BUFFER" << length * K << endl;

int* result = new int[length*K];
int index=0;
findCombine(data, result, index ,depth, N , K );
return result;

}


void findCombineFile( int data[], ofstream& out, int depth , const int N ,const
int k )
{
if ( depth == k )
{
//check
int pre = -1;
for ( int i=0; i< k; i++ )
if ( data[i] <= pre )
return;
else
pre = data[i];

for ( int i=0; i< k; i++ )
{
out<< data[i] << " ";
}
out<<endl;
}
else
for ( int i=0; i< N; i++ )
{
data[depth] = i;
findCombineFile( data, out, depth+1 , N, k  );
}
}


bool CUtility::printCombination(const int N, const int K, char* filename )
{
int* data = new int[K];
int  depth = 0;
// length =  factorial(N)/( factorial(K)* factorial(N-K)) ;
long length = (long) combNum( N, K);
if ( length > 500000000 )
{
cerr << " TOO MUCH COMBINATION " << endl;
return false;
}
// cerr << length << endl;
// cerr << "COM_BUFFER" << length * K << endl;
ofstream out( filename );
assert ( out.is_open());
findCombineFile(data, out, depth, N , K );
return true;

}



*/


/*
void CUtility::showCurTime( FILE * stream  )
{
time_t tim=time(NULL);
tm *now=localtime(&tim);
fprintf( stream , "(%02d:%02d)\n", now->tm_hour, now->tm_min);
}


*/
