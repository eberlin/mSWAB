/**
 * Filename: get_SWABsegs.c
 * 
 * Descript: original Sliding Window And Bottom-up [1] algorithm
 *             [1] Keogh et al. "An Online Algorithm for Segmenting Time Series"
 * 
 * Input:   ac   - one-dimensional timeseries
 *          len  - length of the timeseries
 *          mt   - merging threshold
 *          bs   - initial buffer size
 * Output:  inds - segments indices
 *          vals - segments values
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

uint get_SWABsegs( uint8 ac[], int len, int bs, int mt, int ind[], uint8 val[] )
{
	int itr = 0;			// pointer to the last used array location
	int winlpos = 0; 		// buffer window left and right positions in timeseries 
	int winrpos = winlpos + bs;
	int size = 0;
	
	int l_bound = bs/2;	    // higher and lower bounds as suggested by [1]
	int h_bound = bs*2;
	
	uint8 swabbuf[h_bound];	// the buffer
	
	// inds and vals we get from the Bottom-Up segmentation
	int bu_ind[h_bound];	// BU-'indices' array
	uint8 bu_val[h_bound];	// BU-'values' array
	
	float cost;				// costs for the SW segment attachment
	float int_k;			// interpolation value for the SW segment attachment
	int i, k;
	
	clock_t tic=0,toc=0;	// time measurements
	float time_bu = 0;
	float time_sw = 0;
// 	FILE *fp = 0;
	
	// fill the buffer    ( ac[0] ... ac[bs-1] )
	memcpy(swabbuf, ac, bs);
	
	// add first point of the first segment to the result
	ind[itr] = itr; 		// X coordinate
	val[itr] = ac[itr];		// values
	itr++;
	
	// while there is more data (right of the buffer) -> loop
	while( 1 )
	{
		// 1) Bottom-Up segment the swab buffer
		tic = clock();
		size = get_BUsegs( swabbuf, bs, mt, bu_ind, bu_val );
		toc = clock();
		time_bu += toc-tic;		// accumulate time_bu
		
		// 2) add the leftmost segment to the final segments list
		ind[itr] = winlpos + bu_ind[1];	// store X coordinates
		val[itr] = bu_val[1];			// store Y values
		itr++;
		
		// 3) shift the left border of the buffer window
		winlpos += bu_ind[1];
		
		// 4) shift the right border of the buffer window
		if( winrpos+1 < len )	// test if we can start again with a new window
		{
			tic = clock();
			// run sliding window algo to find the next segment to add to the buffer
			k=0; i=1; cost=0;
			while( cost<mt && i<=(h_bound-(winrpos-winlpos)) && winrpos+i<len )
			{
				// calculate cost for segment [winlpos+bs,winlpos+bs+i] aka [winrpos,winrpos+i] 
				cost = 0;
				for( k=1; k<=i; k++)
				{
					// linear interpolate the points on the segment:
					int_k = (float)ac[winlpos] + k*((float)ac[winrpos+i]-(float)ac[winlpos])/(float)i;
					cost = cost + pow( (float)ac[winrpos+k] - int_k , 2);
				}
				cost = sqrt( cost );
				i++;
			}
			toc = clock();
			time_sw += toc-tic;		// accumulate time_sw
			
			// update buffer size and right buffer window position  
			winrpos += i;
			bs = winrpos-winlpos;
			if( bs < l_bound )
			{
				if( winlpos+l_bound<len )
				{
					winrpos = winlpos+l_bound;
					bs = winrpos-winlpos;
				}
			}
			
			// fill buffer
			for(k=winlpos; k<winrpos; k++)
				swabbuf[k-winlpos] = ac[k];
		}
		else
		{
			// add the leftover BU-segments ...
			for( k=2; k<size; k++ )
			{
				ind[itr] = winlpos-bu_ind[1]+bu_ind[k];
				val[itr] = bu_val[k];
				itr++;
			}
			break;			// ... and quit
		}
	}
	
// 	fp = fopen("timings.txt","a+t");
// 	fprintf(fp,"\t%f", (float)time_bu/CLOCKS_PER_SEC);		//bu
// 	fprintf(fp,"\t%f", (float)time_sw/CLOCKS_PER_SEC);	//sw
// 	fclose(fp);
	
	return itr;
}
