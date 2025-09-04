#include "tally_gpu.h"
#include <cuda_runtime.h>
#include <iostream>
#include <stdexcept>
#include <cmath>
// Helper macro for checking CUDA errors
#define CUDA_CHECK(call)                                                  \
    do {                                                                  \
        cudaError_t err = call;                                           \
        if (err != cudaSuccess) {                                         \
            std::cerr << "CUDA Error: " << cudaGetErrorString(err)        \
                      << " at " << __FILE__ << ":" << __LINE__ << "\n";   \
            throw std::runtime_error("CUDA call failed");                 \
        }                                                                 \
    } while(0)


__global__ void tally_dense_kernel(
    const int* row_offsets,
    const int* entry_cols,
    const DecodeResult* entry_fibs,
    unsigned long long* pair_matrix,
    int nrows,
    int ncols
) {
    int row = blockIdx.x * blockDim.x + threadIdx.x;
    if (row >= nrows) return;

    int start = row_offsets[row];
    int end = row_offsets[row + 1];
    int len = end - start;

    for (int i = 0; i < len; ++i) {
        int colA = entry_cols[start + i];
        DecodeResult fibA = entry_fibs[start + i];

        for (int j = i + 1; j < len; ++j) {
            int colB = entry_cols[start + j];
            DecodeResult fibB = entry_fibs[start + j];

            unsigned long long count = 0;
            if (fibA.fibcnt1 > 0 && fibB.fibcnt1 > 0)
                count += (unsigned long long)fibA.fibcnt1 + fibB.fibcnt1;
            if (fibA.fibcnt2 > 0 && fibB.fibcnt2 > 0)
                count += (unsigned long long)fibA.fibcnt2 + fibB.fibcnt2;
            if (fibA.fibcnt3 > 0 && fibB.fibcnt3 > 0)
                count += (unsigned long long)fibA.fibcnt3 + fibB.fibcnt3;

            if (count > 0) {
                int a = colA < colB ? colA : colB;
                int b = colA < colB ? colB : colA;
                atomicAdd(&pair_matrix[(unsigned long long)a * ncols + b], count);
            }
        }
    }
}

void tally_gpu(
    const std::vector<int>& h_row_offsets,
    const std::vector<int>& h_entry_cols,
    const std::vector<DecodeResult>& h_entry_fibs,
    std::vector<std::pair<std::pair<int,int>, unsigned long long>>& results,
    int nrows,
    int ncols
) {
    int nentries = (int)h_entry_cols.size();

    int* d_row_offsets = nullptr;
    int* d_entry_cols = nullptr;
    DecodeResult* d_entry_fibs = nullptr;
    unsigned long long* d_pair_matrix = nullptr;

    CUDA_CHECK(cudaMalloc(&d_row_offsets, sizeof(int) * (nrows + 1)));
    CUDA_CHECK(cudaMalloc(&d_entry_cols, sizeof(int) * nentries));
    CUDA_CHECK(cudaMalloc(&d_entry_fibs, sizeof(DecodeResult) * nentries));
    CUDA_CHECK(cudaMalloc(&d_pair_matrix, sizeof(unsigned long long) * (unsigned long long)ncols * ncols));

    CUDA_CHECK(cudaMemcpy(d_row_offsets, h_row_offsets.data(), sizeof(int) * (nrows + 1), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_entry_cols, h_entry_cols.data(), sizeof(int) * nentries, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_entry_fibs, h_entry_fibs.data(), sizeof(DecodeResult) * nentries, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemset(d_pair_matrix, 0, sizeof(unsigned long long) * (unsigned long long)ncols * ncols));

    int blockSize = 256;
    int gridSize = (nrows + blockSize - 1) / blockSize;

    tally_dense_kernel<<<gridSize, blockSize>>>(
        d_row_offsets, d_entry_cols, d_entry_fibs, d_pair_matrix, nrows, ncols
    );
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    std::vector<unsigned long long> h_pair_matrix((size_t)ncols * ncols);
    CUDA_CHECK(cudaMemcpy(h_pair_matrix.data(), d_pair_matrix, sizeof(unsigned long long) * (unsigned long long)ncols * ncols, cudaMemcpyDeviceToHost));

    CUDA_CHECK(cudaFree(d_row_offsets));
    CUDA_CHECK(cudaFree(d_entry_cols));
    CUDA_CHECK(cudaFree(d_entry_fibs));
    CUDA_CHECK(cudaFree(d_pair_matrix));
    results.clear();
    for (int a = 0; a < ncols; ++a) {
        for (int b = a; b < ncols; ++b) {
            unsigned long long count = h_pair_matrix[(unsigned long long)a * ncols + b];
            unsigned long long count_temp=static_cast<unsigned long long>(std::sqrt(count));
            if (count_temp > 0) {
                results.emplace_back(std::make_pair(a, b), count_temp);
            }
        }
    }
}
