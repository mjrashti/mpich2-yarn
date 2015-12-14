CFLAGS= -DENABLE_CGROUPS
CLIBS= -lcgroup

SRC=$(wildcard *.c)

EXE=	container_wrapper\
	test_mpi\
	cpi
	
all: $(EXE)
	@echo "Make completed. Now run make install."
install:
	@cp container_wrapper ../target/

%:	
	@mpicc -o $@ $@.c $(CFLAGS) $(CLIBS)

clean: 
	@rm $(EXE)
