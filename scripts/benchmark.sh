#!/bin/bash
echo "--- Starting Benchmark ---"
echo "Compiling..."
make clean && make
echo ""
echo "Running Pthreads (Fixed 4 threads):"
time ./bin/pi_pthreads
echo ""
echo "Running OpenMP (System default threads):"
time ./bin/pi_openmp
