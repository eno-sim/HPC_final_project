#ifndef ORDERED_EVOLUTION_H
#define ORDERED_EVOLUTION_H

void update_cell_ordered(unsigned char *top_row, unsigned char *bottom_row,  unsigned char *local_playground, int xsize, int ysize, int x, int y);
void ordered_evolution(unsigned char *local_playground, int xsize, int my_chunk, int offset,  int n, int s);



#endif //ORDERED_EVOLUTION_H
