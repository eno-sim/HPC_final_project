#ifndef ORDERED_EVOLUTION_H
#define ORDERED_EVOLUTION_H

void update_cell_ordered(unsigned char * playground, int xsize, int ysize, int x, int y);
void ordered_evolution(unsigned char * playground, int xsize, int ysize, int n);
void ordered_evolution_MPI(unsigned char *playground, int xsize, int ysize, int n, int s);
void update_cell_ordered_MPI(unsigned char *top_row, unsigned char *bottom_row,
 unsigned char *local_playground, int xsize, int ysize, int x, int y);

#endif //ORDERED_EVOLUTION_H
