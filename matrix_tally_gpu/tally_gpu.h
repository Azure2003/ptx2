#pragma once
#include <vector>
#include <cstdint>

struct DecodeResult {
    int fibcnt1;
    int fibcnt2;
    int fibcnt3;
};

// GPU tally function interface
void tally_gpu(
    const std::vector<int>& row_offsets,
    const std::vector<int>& entry_cols,
    const std::vector<DecodeResult>& entry_fibs,
    std::vector<std::pair<std::pair<int,int>, unsigned long long>>& results,
    int nrows,
    int ncols
);
