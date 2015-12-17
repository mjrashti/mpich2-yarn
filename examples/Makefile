CFLAGS= 
CLIBS= -lcgroup

SRC=$(wildcard *.c)

EXE=	container_wrapper\
	container_temp_cleanup\
	test_mpi\
	cpi
	
all: $(EXE)
	@echo "Make completed. Now run make install."
install:
	@cp container_wrapper ../target/
	@cp container_temp_cleanup ../target/

%:	
	@mpicc -o $@ $@.c $(CFLAGS) $(CLIBS)

clean: 
	@rm $(EXE)
