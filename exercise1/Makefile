CC=mpicc -O3 -fopenmp -march=native -g -Iinclude
loc = $(location)
OBJDIR=obj
INCDIR=include

RW=$(OBJDIR)/read_write_pgm.o
STATIC= $(OBJDIR)/static_evolution.o
ORDERED= $(OBJDIR)/ordered_evolution.o
OBJECTS=main.o $(RW) $(STATIC) $(ORDERED)


par: $(loc)/main.x


$(loc)/main.x: $(OBJECTS)
	$(CC) -lm $(OBJECTS) -o $@

main.o: main.c
	$(CC) -c main.c

$(OBJDIR)/%.o: src/%.c $(INCDIR)/%.h
	mkdir -p $(OBJDIR)
	$(CC) -c $< -o $@

clean: x_clean clean_img

x_clean:
	rm -rf $(loc)/*.x $(OBJDIR)/*.o $(OBJDIR)/*.a 

clean_img:
	rm $(loc)/*.pgm
