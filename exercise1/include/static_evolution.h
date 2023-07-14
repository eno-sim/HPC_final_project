#ifndef STATIC_EVOLUTION_H
#define STATIC_EVOLUTION_H

void update_cell_static(unsigned char *top_row, unsigned char *bottom_row, unsigned char *local_playground, unsigned char *updated_playground, int xsize, int ysize, int x, int y);
void static_evolution(unsigned char *local_playground, int xsize, int my_chunk, int my_offset,  int n, int s);
#endif //STATIC_EVOLUTION_H
