include Makefile.defs

OPT+= -fPIC -DLIBREBOUND

SOURCES=rebound.c tree.c particle.c gravity.c integrator.c integrator_whfast.c integrator_ias15.c integrator_sei.c integrator_leapfrog.c integrator_hermes.c integrator_mercurius.c boundary.c input.c output.c collision.c communication_mpi.c display.c tools.c derivatives.c simulationarchive.c glad.c integrator_janus.c transformations.c
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(SOURCES:.c=.h)

all: $(SOURCES) librebound.a 

%.o: %.c $(HEADERS)
	@echo "Compiling source file $< ..."
	$(CC) -c $(OPT) $(PREDEF) -o $@ $<

librebound.a: $(OBJECTS)
	@echo ""        
	@echo "Linking static library $@ ..."
	ar rcs $@ $(OBJECTS)
	#$(CC) $(OPT) -shared $(OBJECTS) $(LIB) -o $@ 
	
	@echo ""        
	@echo "The static library $< has been created successfully."
	
	
clean:
	@echo "Removing object files *.o ..."
	@-rm -f *.o
	@echo "Removing static library librebound.a ..."
	@-rm -f *.a
	@echo "Removing coverage files ..."
	@-rm -f *.gcda
	@-rm -f *.gcno
	
