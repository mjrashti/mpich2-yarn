CFLAGS=
CLIBS= -lcgroup

SRC=$(wildcard *.c)

EXE=	container_wrapper\
	test_mpi\
	cpi
	
all: $(EXE)
	@cp container_wrapper ../target/

%:	
	@mpicc -o $@ $@.c $(CFLAGS) $(CLIBS)

clean: 
	@rm $(EXE)
