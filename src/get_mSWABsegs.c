/**
 * Filename: get_mSWABsegs.c
 * 
 * Descript: modified Sliding Window And Bottom-up [1] algorithm
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

#define ssgn(sl,v1,v2) if((v1)<(v2)) sl=1; else if((v1)>(v2)) sl=-1; else sl=0

uint get_mSWABsegs( uint8 ac[], int len, int bs, int mt, int ind[], uint8 val[] )
{
	int itr = 0;				// index of the last array position
	int winlpos = 0; 			// buffer window left border position
	int winrpos = winlpos + bs; // buffer window right border position
	int size = 0;				// size of resulting ind/val segments array
	
	int l_bound = bs/2;	    	// higher and lower buffer bounds (as in [1])
	int h_bound = min(bs*2,255);
	
	uint8 swabbuf[h_bound];		// sliding buffer array
	
	// pointer to the inds and vals we get from Bottom-Up segmentation function
	int bu_ind[h_bound];
	uint8 bu_val[h_bound];
	
	int i, k, slope;			// variables for sliding the buffer
	int s; float dff;			// merging segments with similar segments (NOTE)
	
	memcpy(swabbuf, ac, bs);	// copy first values to buffer
	
	// add first point of the first segment
	ind[itr] = itr; 			// store X coordinates
	val[itr] = ac[itr];			// store Y values
	itr++;						// update array position
	
	// while there is more data -> loop
	while( 1 )
	{
		// 1) Bottom-Up segment the swab buffer
		size = get_BUsegs( swabbuf, bs, mt, bu_ind, bu_val );
		
		// 2) add the leftmost segment to the final segments list
		ind[itr] = winlpos + bu_ind[1];	// store X coordinates
		val[itr] = bu_val[1];			// store Y values
		itr++;
		
		// 3) compare last two segments and merge if slopes are close
		// NOTE: maximum delta is 255 (1 byte), beware of the overflow!
		if( itr>3 )
		{
			s = itr-3;
			if( ind[s+2]-ind[s] < 255 )
			{
				dff = (float)(val[s+2]-val[s+1])/(ind[s+2]-ind[s+1]) - (float)(val[s+1]-val[s])/(ind[s+1]-ind[s]);
				if( abs(dff)<0.01 )			/* ~0.5° difference */
				{
					// merge by shifting val/ind one step to the left
					for(k=s+1; k<itr; k++)
					{
						ind[k] = ind[k+1];
						val[k] = val[k+1];
					}
					itr--; // roll back 1 segment
				}
			}
		}
		
		// 4) shift the left border of the buffer window
		winlpos += bu_ind[1];
		
		// 5) shift the right border of the buffer window
		if( winrpos+1 < len )	// test if we can start over with new data
		{
			// find the next segment to add to the buffer (slope sign change)
			ssgn(slope,ac[winrpos],ac[winrpos+1]); 
			k = slope;
			i = 1;
			while( k==slope && i<=(h_bound-(winrpos-winlpos)) && winrpos+i<len )
			{
				ssgn(k,ac[winrpos+i],ac[winrpos+i+1]);
				i++;
			}
			
			// update buffer size and right buffer window position
			winrpos += i;
			bs = winrpos-winlpos;
			if( bs < l_bound )		// check that the buffer is not too small
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
		else				// end of timeseries -> dump rest of segments
		{
			for( k=2; k<size; k++ )
			{
				ind[itr] = winlpos-bu_ind[1]+bu_ind[k];
				val[itr] = bu_val[k];
				itr++;
			}
			break;			// ... and quit
		}
	}
	
	return itr;
}
