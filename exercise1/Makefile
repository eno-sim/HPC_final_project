CC=mpicc -O3 -fopenmp -march=native -g -Iinclude

OBJDIR=obj
INCDIR=include

RW=$(OBJDIR)/read_write_pgm.o
STATIC=$(OBJDIR)/static_evolution.o
ORDERED=$(OBJDIR)/ordered_evolution.o
OBJECTS=main.o $(RW) $(STATIC) $(ORDERED)

all: main.x

main.x: $(OBJECTS)
	$(CC) -DTIME -lm $(OBJECTS) -o $@

main.o: main.c 
	$(CC) -DTIME -c main.c

$(OBJDIR)/%.o: src/%.c $(INCDIR)/%.h
	mkdir -p $(OBJDIR)
	$(CC) -c $< -o $@

clean: x_clean img_clean

x_clean:
	rm -rf *.x $(OBJDIR)/*.o $(OBJDIR)/*.a 

clean_img:
	rm images/snapshots/*.pgm
	rm images/*.pgm
