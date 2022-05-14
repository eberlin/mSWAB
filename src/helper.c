/**
 * Filename: helper.c
 * 
 * Descript: helper functions collection
 * 
 * 
 * Copyright (C) 2011									Author: Eugen Berlin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
**/


#include "main_segs.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


////////////////////////////////////////////////////////////////////////////////
// read raw binary data file
uint8* read_binary_data( uint8* dim, int* len, void* datafile )
{
	// get file size
	struct stat file_stat;
	int fildes = open(datafile, O_RDWR);
	fstat(fildes, &file_stat);
	
	// file size (without the one byte for "number of dimensions")
	int l = (file_stat.st_size - 1);
	close(fildes);
	
	// loop counters
	int i,j;
	
	// allocate memory for ac array
	uint8* ac  = (uint8*) malloc( sizeof(uint8)*l );
	
	// read values from binary file
	FILE *log = fopen(datafile,"rb");
	
	// read number of dimensions (e.g. 3d for accelerometer)
	fscanf(log,"%c",dim);
	
	// input = x0,y0,z0,x1,y1,z1,...  ---> ac[] = x0,x1,...,y0,y1,...z0,z1,...
	l = l / *dim;
	for(i=0; i<l; i++)
		for(j=0; j<*dim; j++)
			fscanf(log,"%c",&ac[i+j*l]);
	
	fclose(log);
	*len = l;
	
	return ac;
}


// calculates error between a segmentation and original data
double calc_error( uint8 ac[], int len, int inds[], uint8 vals[], int s_len )
{
	// interpolation slope and cost
	double slp, intpk;
	double costs = 0;
	int i, k;
// 	printf("len=%d | inds[%d] = %d \n\n",len,s_len,inds[s_len]);
	
	if( inds[s_len-1] > len )
		return -1;
	
	for( i=0; i<s_len-1; i++ )
	{
		// calc slope
		slp = (double)(vals[i+1]-vals[i]) / (double)(inds[i+1]-inds[i]);
		
		for( k=inds[i]+1; k<inds[i+1]; k++ )
		{
			intpk = (double)vals[i] + (double)(k-inds[i])*slp;
			costs += pow( intpk-(double)ac[k] , 2);
		}
	}
	
// 	costs = sqrt( costs )/len;
	return costs;
}

// // check whether an overlap exists
// int check_overlap(int top_ind[], int ind, int len, int K)
// {
// 	int i;
// 	for( i=0; i<K; i++ )
// 	{
// 		if( abs(top_ind[i] - ind) < len )
// 			return i;
// 	}
// 	
// 	return -1;
// }

// // int fill_arrays( int ts_ind[], uint8 ts_val[], int len, int i, int winlen, int tsi_ind[], uint8 tsi_val[] )
// {
// 	// copy ts indices and values to candidate window
// 	int k=0;
// 	while( (i+k<len)  &&  (ts_ind[i+k]-ts_ind[i] < winlen) )
// 	{
// 		tsi_ind[k] = ts_ind[i+k] - ts_ind[i];		// subtract the offset
// 		tsi_val[k] = ts_val[i+k];
// 		k++;
// 	}
// 	return k;
// }



// returns the coordinate of the min value in the array c
int get_min(float c[], int l)
{
	float min = FLT_MAX;
	int coord = -1;
	
	int i;
	for( i=0; i<l; i++ )
	{
		if( c[i] < min )
		{
			min = c[i];
			coord = i;
		}
	}
	
	return coord;
}


// returns the coordinate of the max value in the array c
int get_max(float c[], int l)
{
	float max = FLT_MIN;
	int coord = -1;
	
	int i;
	for( i=0; i<l; i++ )
	{
		if( c[i] > max )
		{
			max = c[i];
			coord = i;
		}
	}
	
	return coord;
}
