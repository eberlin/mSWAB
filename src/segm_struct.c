
#include "main_segs.h"
#include "segm_struct.h"


////////////////////////////////////////////////////////////////////////////////
// segment structure functions (C)

struct Segm* 	segm_create(int dimension)
{
	int i = 0;
	struct Segm *s = 0;
	s = (struct Segm*) malloc(sizeof(struct Segm));
	s->dim  = dimension; 
	s->segs = (struct Seg*) malloc(sizeof(struct Seg)*dimension);
	for (i=0; i<dimension; i++) {
		s->segs[i].size = 0;
		s->segs[i].ind = 0;
		s->segs[i].val = 0;
	}
	return s;
}

void		segm_create_dim(struct Segm* s, int dim, int size)
{
	s->segs[dim].size = size;
	s->segs[dim].ind = (int*) malloc(sizeof(int)*size);		
	s->segs[dim].val = (uint8*) malloc(sizeof(uint8)*size);
}

void		segm_fill_dim(struct Segm* s, int dim, int* ind, uint8* val)
{
	memcpy(s->segs[dim].ind, ind, s->segs[dim].size*sizeof(int));
	memcpy(s->segs[dim].val, val, s->segs[dim].size);
}

void 		segm_free(struct Segm* s)
{
	int i=0;
	for (i=0; i<(s->dim); i++)
	{
		free(s->segs[i].ind);
		free(s->segs[i].val);
	}
	free(s->segs);
	free(s);
}

void		segm_write(struct Segm* s, const char* file)
{
	int i=0,j=0;
	FILE *fp;
	fp = fopen(file,"wb");
	// write header: [dim] [size_x] [size_y] ...
		fprintf(fp,"%c",s->dim); 
		for (j=0; j<(s->dim); j++)
			fwrite(&(s->segs[j].size),sizeof(int),1,fp);
	// write data:
		for (j=0; j<(s->dim); j++)
		{
			for (i=0; i<(s->segs[j].size); i++)
			{
				fwrite(&(s->segs[j].ind[i]),sizeof(int),1,fp);
				fprintf(fp,"%c", s->segs[j].val[i]);
			}
		}
	fclose(fp);
}

void		segm_write_delta(struct Segm* s, const char* file)
{
	int i=0,j=0;
	int delta=0;
	FILE *fp;
	fp = fopen(file,"wb");
	// write header: [dim] [size_x] [size_y] ...
		fprintf(fp,"%c",s->dim); 
		for (j=0; j<(s->dim); j++)
			fwrite(&(s->segs[j].size),sizeof(int),1,fp);
	// write data:
		for (j=0; j<(s->dim); j++)
		{
			for (i=0; i<(s->segs[j].size); i++)
			{
				if(i==0)
					delta = s->segs[j].ind[i];
				else
					delta = s->segs[j].ind[i] - s->segs[j].ind[i-1];
				
				fprintf(fp,"%c", delta);				// TODO: argh! int->char conversion
				fprintf(fp,"%c", s->segs[j].val[i]);
			}
		}
	fclose(fp);
}


/*
struct Segm*	segm_copy(struct Segm* s)
{
	struct Segm* scp=segm_create(s->dim);
	
	int j=0;
	for (j=0; j<(s->dim); j++)
	{
		segm_create_dim(scp,j,s->segs[j].size);
		segm_fill_dim(scp, j, s->segs[j].ind, s->segs[j].val); // 'doh
	}
	
	return scp;	// 'doh
}
*/

/*
struct Segm*	segm_read(const char* file)
{
	int i=0,j=0;
	uint8 dim;
	struct Segm* s=0;
	FILE *fp;
	fp = fopen(file,"rb");
	if (fp==NULL) return NULL; // error in opening file
	// read header: [dim] [size_x] [size_y] ...
	fscanf(fp,"%c",&dim);
	s = segm_create(dim);
	for (j=0; j<(s->dim); j++)
	{
		fread(&i,sizeof(int),1,fp);
		segm_create_dim(s,j,i);
	}
	// write data:
	for (j=0; j<(s->dim); j++)
	{
		for (i=0; i<(s->segs[j].size); i++)
		{
			fread(&(s->segs[j].ind[i]),sizeof(int),1,fp);
			fscanf(fp,"%c", &(s->segs[j].val[i]));
		}
	}
	fclose(fp);
	return s;
}
*/

/*
void segm_shiftlefttozero(struct Segm* s)
{
	int i=0; 
	int j=0;
	int offset=INT_MAX;
	for (j=0; j<(s->dim); j++)
		offset = (offset<s->segs[j].ind[i])?offset:s->segs[j].ind[0];
	for (j=0; j<(s->dim); j++)
	{
		for (i=0; i<(s->segs[j].size); i++)
		{
			s->segs[j].ind[i]-=offset;
		}
	}
}
*/

// return pointer to subsequence of ts that is situated between index x1 and x2 
/*
struct Segm* segm_create_subseq(struct Segm* ts, int x1, int x2)
{
	int i,j,d;
	// create new subsequence ... get start-stop ind pointers:
	struct Segm* tsi=segm_create(ts->dim);
	for(d=0; d<ts->dim; d++)
	{
		i=0;
		while( x1 > ts->segs[d].ind[i] )	i++;
		j=i;
		while( x2 > ts->segs[d].ind[j] )	j++;
		j--;
		// copy values
		segm_create_dim(tsi,d,j-i+1);
		segm_fill_dim(tsi, d, &(ts->segs[d].ind[i]), &(ts->segs[d].val[i]));
	}
	return tsi;		// pointer to zoomed-in/out subsequence
}
*/
