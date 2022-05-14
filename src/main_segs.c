/**
 * Filename: main.c
 * 
 * Descript: wrapper function for timeseries segmentation (SWAB or mSWAB)
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
#include "segm_struct.h"

#define DEBUG 0
#define LOGFILE "/tmp/mswab.txt"
#define TXTOUT 0

int main(int argc, char **argv)
{
	// if no parameters provided: print "help"
	if(argc==1)
	{
		printf("--------------------------------------------------------------------------------------------------\n");
		printf("usage: \n\t./main_segs [SWABsegs|mSWABsegs] <thresh> <bufsize> <raw_file> <segs_file> \n");
		printf("--------------------------------------------------------------------------------------------------\n");
		printf("example: \n\t./bin/main_segs mSWABsegs 30 100 ./data/example.bin ./data/segments.bin  \n");
		printf("\t./bin/main_segs  SWABsegs 15  80 ./data/example.bin ./data/segments.bin  \n");
		printf("--------------------------------------------------------------------------------------------------\n");
		return 0;
	}
	
	clock_t tic=0,toc=0;	// time measurements
	FILE *log;				// logfile handler
	
	char *segs_algo;		// algorithm to use for segmentation
	char *datafile;			// binary data file containing raw accelerometer data
	char *segsfile;			// binary data file that will contain the segments
	
	uint8 *ac;				// pointer to 'ac' array
	int *ind;				// pointer to 'indices' array
	uint8 *val;				// pointer to 'values' array
	int len;				// filesize == length of arrays
	uint8 dim;
	char str[127];			// filename string
	int size=0;				// length of ind/val segments array
	int i,j;				// loop counters
	int MT = 0;				// merging threshold
	int BS = 0;				// buffer size
	float err=0;			// accumulative error
	
	// parse the parameters
	argc--; segs_algo = *argv++;		// skip function name
	argc--; segs_algo = *argv++;		// segmentation algorith = mSWAB
	argc--; MT = (int) atoi(*argv++);	// merging threshold
	argc--; BS = (int) atoi(*argv++);	// initial buffer size
	argc--; datafile = *argv++;			// binary raw data file path & name
	argc--; segsfile = *argv++;			// segsfile name to store segments to
	
	if(DEBUG)
	{
		log = fopen(LOGFILE, "w");
		fprintf(log, " log for approximating a timeseries with following parameters \n");
		fprintf(log, "==============================================================\n");
		fprintf(log, " algorithm     : %s", segs_algo);
		fprintf(log, "  with threshold %d and bufsize %d \n", MT, BS);
		fprintf(log, " raw data file : %s \n", datafile);
		fprintf(log, " segments file : %s \n\n", segsfile);
		fclose(log);
	}
	
	/////////////////////////////////
	// read binary data from datafile -> uint8* pointer ac and length of array
	ac = read_binary_data( &dim, &len, datafile );
	
	// alloc memory for the [ind val] output arrays
	ind = (int*)  malloc( sizeof(int)*len  );
	val = (uint8*) malloc( sizeof(uint8)*len );
	
	// segmentation structure init:
	struct Segm* segm;
	segm = segm_create(dim);
	
	tic = clock();		// start time measurements
	for (j=0; j<dim; j++)
	{
		for( i=0; i<len; i++ )
			ind[i]=0;
		
		if( strncmp(segs_algo,"SWABsegs",8) == 0 )
			size = get_SWABsegs(&ac[len*j], len, BS, MT, ind, val);
		else if( strncmp(segs_algo,"mSWABsegs",9) == 0 )
			size = get_mSWABsegs(&ac[len*j], len, BS, MT, ind, val);
		
		segm_create_dim(segm, j, size);
		segm_fill_dim(segm, j, ind, val);
	}
	toc = clock();		// stop time measurements
	
	// write segments to text files
	if(TXTOUT)
	{
		for (i=0; i<dim; i++)
		{
			snprintf(str, sizeof(str), "%s.%d.txt", segsfile, i);
			log = fopen(str, "w");			
			for(j=0; j<segm->segs[i].size; j++)
				fprintf(log, "%d %d\n", segm->segs[i].ind[j], segm->segs[i].val[j] );
			fclose(log);
		}
	}
		
	// write segments to a binary file
	segm_write_delta(segm, segsfile);
		
	// write report file
	if(DEBUG)
	{
		log = fopen(LOGFILE, "aw");
		fprintf(log," size of \n");
		fprintf(log,"  raw data file: %d byte \n", len*3);
		fprintf(log,"  segments file: %d byte \n\n", 
				2*((int)segm->segs[0].size + (int)segm->segs[1].size + (int)segm->segs[2].size) );
		fprintf(log," elapsed time  : %f seconds\n", (float)(toc-tic)/CLOCKS_PER_SEC);
		
		err = 0;
		for (i=1; i<segm->dim; i++)
			err += calc_error(&ac[len*i], len, (segm->segs[i].ind), (segm->segs[i].val), segm->segs[i].size-10 );
		fprintf( log, " Error = %f\n", err );
		fclose(log);
	}
	
// 	log = fopen("/tmp/mswab_eval.txt","aw");
// 	fprintf(log, "%d\t%d\t%d\t%d\t%f\t%f\n\r",
// 		MT, BS, len*3, 
// 		2*((int)segm->segs[0].size + (int)segm->segs[1].size + (int)segm->segs[2].size), 
// 		sqrt(err),
// 		(float)(toc-tic)/CLOCKS_PER_SEC);
// 	fclose(log);
	
	return 0;
}
