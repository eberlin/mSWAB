/**
 * header file
 *
 * other needed headers included, some helper functions as defines
 *
 * coded by Eugen Berlin
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include <string.h>
#include <float.h>
#include <limits.h>
#include <time.h>

// min, max, abs, sign helper functions
#define min(x,y) (((x)>=(y))?y:x)
#define max(x,y) (((x)<=(y))?y:x)

// #define min3(x,y,z) (min(x,(min(y,z))))
// #define max3(x,y,z) (max(x,(max(y,z))))

#define abs(x)   (((x)<0)?-(x):x)
#define sign(x)  (((x)==0)?0:(((x)>0)?1:-1))


// uint8 definition
typedef unsigned char uint8;
typedef unsigned int uint;


////////////////////////////////////////////////////////////////////////////////
// approximation
uint	get_BUsegs( uint8 ac[], int len, int mt, int inds[], uint8 vals[] );
uint	get_SWABsegs( uint8 ac[], int len, int bs, int mt, int ind[], uint8 val[] );
uint	get_mSWABsegs( uint8 ac[], int len, int bs, int mt, int ind[], uint8 val[] );

////////////////////////////////////////////////////////////////////////////////
// helper function section
uint8* read_binary_data( uint8* dim, int* len, void* datafile );

double calc_error( uint8 ac[], int len, int inds[], uint8 vals[], int s_len );



