#ifndef READ_WRITE_PGM_H
#define READ_WRITE_PGM_H

void write_pgm_image(void *image, int maxval, int xsize, int ysize, const char *image_name);
void read_pgm_image(void **image, int *maxval, int *xsize, int *ysize, const char *image_name);
void *init_playground(int xsize, int ysize);
void write_snapshot(unsigned char *playground, int maxval, int xsize, int ysize, const char *basename, int iteration);


unsigned char *parallel_init_playground(int xsize, int ysize);
void par_write_snapshot(unsigned char *playground, int maxval, int xsize, int ysize, const char *basename, int iteration, int offset);
void parallel_read_pgm_image(void **image, const char *image_name, int offset, int portion_size);
void parallel_write_pgm_image(void *image, int maxval, int xsize, int my_chunk, const char *image_name, int offset);
 
#endif //READ_WRITE_PGM_H
