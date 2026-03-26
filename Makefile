CC = gcc
CFLAGS = -O3 -fopenmp -pthread
BIN_DIR = bin

all: $(BIN_DIR)/pi_openmp $(BIN_DIR)/pi_pthreads

$(BIN_DIR)/pi_openmp: src/openmp/pi.c
	$(CC) $(CFLAGS) src/openmp/pi.c -o $(BIN_DIR)/pi_openmp

$(BIN_DIR)/pi_pthreads: src/pthreads/pi.c
	$(CC) $(CFLAGS) src/pthreads/pi.c -o $(BIN_DIR)/pi_pthreads

clean:
	rm -rf $(BIN_DIR)/*
