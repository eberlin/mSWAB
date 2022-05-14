#ifdef __cplusplus
extern "C" {
#endif
	
	// segmentation structures
	struct Seg
	{
		int size;
		int *ind;
		uint8 *val;
	};
	
	struct Segm
	{
		int dim;
		struct Seg *segs;
	};
	
	struct Segm* 	segm_create(int dimension);
	void			segm_create_dim(struct Segm* s, int dim, int size);
	void			segm_free(struct Segm* s);
	void			segm_fill_dim(struct Segm* s, int dim, int* ind, uint8* val);
	void			segm_write(struct Segm* s, const char* file);
	void			segm_write_delta(struct Segm* s, const char* file);
	struct Segm*	segm_read(const char* file);
	struct Segm*	segm_copy(struct Segm* s);
	void segm_shiftlefttozero(struct Segm* s);
	struct Segm* 	segm_create_subseq(struct Segm* ts, int x1, int x2);
	
#ifdef __cplusplus
}
#endif 
