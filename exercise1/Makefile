CC=mpicc -O3 -fopenmp -march=native -g -Iinclude
loc = $(location)
OBJDIR=obj
INCDIR=include

RW=$(OBJDIR)/read_write_pgm.o
STATIC= $(OBJDIR)/mem_static_evolution.o
ORDERED= $(OBJDIR)/mem_ordered_evolution.o
OBJECTS=par_main.o $(RW) $(STATIC) $(ORDERED)

# all: $(loc)/par_main.x $(loc)/main.x 
par: $(loc)/par_main.x
#ser: $(loc)/main.x


#$(loc)/main.x: $(OBJECTS)
#	$(CC) -lm $(OBJECTS) -o $@

#main.o: main.c 
#	$(CC) -c main.c

$(loc)/par_main.x: $(OBJECTS)
	$(CC) -lm $(OBJECTS) -o $@

par_main.o: par_main.c
	$(CC) -c par_main.c

$(OBJDIR)/%.o: src/%.c $(INCDIR)/%.h
	mkdir -p $(OBJDIR)
	$(CC) -c $< -o $@

clean: x_clean img_clean

x_clean:
	rm -rf *.x $(OBJDIR)/*.o $(OBJDIR)/*.a 

clean_img:
	rm ./*.pgm
