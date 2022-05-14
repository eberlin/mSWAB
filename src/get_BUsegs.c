/**
 * Filename: get_BUsegs.c
 * 
 * Descript: Bottom-Up piecewise linear approximation algorithm
 * 
 * Input:   ac   - one-dimensional timeseries
 *          len  - lenght of the timeseries
 *          mt   - merging threshold
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

int		get_min(float c[], int l);
void	calc_bu_error( uint8 ac[], int inds[], uint8 vals[], float costs[], int i );


uint get_BUsegs( uint8 ac[], int len, int mt, int inds[], uint8 vals[] )
{
	int i = 0;					// loop counter
	int costs_len = len-2;		// interpolation costs array length
	float costs[len-2];			// interpolation costs array
	float sum = 0;				// sum of initial interpolation costs
	int coord = 0;				// index of the cheapest pair of segments
	
	// copy i/ac[i] as initial indices and values
	for( i=0; i<len; i++ )
	{
		inds[i] = i;
		vals[i] = ac[i];
	}
	
	// compute initial costs for each adjacent pair of segments
	for( i=0; i<costs_len; i++ )
	{
		calc_bu_error( ac, inds, vals, costs, i );
		sum += costs[i];
	}
	
	// if sum of costs is zero -> shortcut -> one resulting segment
	if (sum==0)
	{
		inds[1] = inds[len-2]; vals[1] = vals[len-2];
		inds[2] = inds[len-1]; vals[2] = vals[len-1];
		len = 3;
		return len;
	}
	
	// loop: merge cheapest pairs of segments until threshold is exceeded
	while( 1 )
	{
		coord = get_min(costs, costs_len);
		
		// if bad coordinate or costs exceed threshold -> exit while loop
		if( coord < 0 || costs[coord] >= mt )
			break;
		
		// merge segments by shifting val/ind/costs[i+1:end] one space to the left
		for( i=coord+1; i<len; i++ )
		{
			inds[i] = inds[i+1];
			vals[i] = vals[i+1];
			if( i<costs_len )
				costs[i-1] = costs[i];
		}
		len--;			// decrease actual lengh of array
		costs_len--;	// ... and costs array
		
		if (len == 2)	// if only one segment left -> exit while loop
			break;
		
		// update costs array now:
		// 1) costs for ( coord | coord+1 )
		if( coord<costs_len )
			calc_bu_error( ac, inds, vals, costs, coord );
		
		// 2) costs for ( coord-1 | coord )
		if( coord>0 /*&& coord<costs_len*/ )
			calc_bu_error( ac, inds, vals, costs, coord-1 );
	}
	
	return len;
}



/// calculates interpolation error/cost for merging two adjacent segments
void calc_bu_error( uint8 ac[], int inds[], uint8 vals[], float costs[], int i )
{
	
	float slp;		// slope of segment 
	float intpk;	// interpolation value at postition k
	
	slp = (float)(vals[i+2]-vals[i]) / (float)(inds[i+2]-inds[i]);
	
	costs[i] = 0;
	int k;
	for( k=inds[i]+1; k<inds[i+2]; k++ )
	{
		intpk = (float)vals[i] + (float)(k-inds[i])*slp;
		costs[i] += pow( intpk-(float)ac[k] , 2);
	}
	
	costs[i] = sqrt( costs[i] );
	return;
}
