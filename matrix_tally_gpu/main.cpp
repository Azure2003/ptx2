// File: main.cpp
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <stdexcept>
#include "tally_gpu.h"
#include <unordered_map>

DecodeResult decode(int64_t incode) {
    const int64_t two32 = 1LL << 32;
    const int64_t mult = 1001;
    const int64_t multmult = mult * mult;

    int64_t fibre_count = incode / two32;
    incode %= two32;
    int64_t fibre_prop1 = incode / multmult;
    incode %= multmult;
    int64_t fibre_prop2 = incode / mult;
    incode %= mult;

    int fibcnt1 = static_cast<int>(std::round(fibre_prop1 * 0.001 * fibre_count));
    int fibcnt2 = static_cast<int>(std::round(fibre_prop2 * 0.001 * fibre_count));
    int fibcnt3 = static_cast<int>(fibre_count - fibcnt1 - fibcnt2);

    return { fibcnt1, fibcnt2, fibcnt3 };
}

#include <utility> // for std::pair

std::pair<int64_t, int64_t> read_num_rows_and_columns(const std::string& meta_file_path) {
    std::cout << "Reading matrix from: " << meta_file_path << std::endl;

    std::ifstream meta_file(meta_file_path);
    if (!meta_file.is_open()) throw std::runtime_error("Cannot open meta file");

    int64_t num_rows, num_columns;
    meta_file >> num_rows >> num_columns;

    if (!meta_file) throw std::runtime_error("Failed to read meta file");

    return {num_rows, num_columns};
}


std::vector<int64_t> read_column_counts(const std::string& path, int64_t num_columns) {
    std::cout << "Reading matrix from: " << path << std::endl;
    std::ifstream fin(path, std::ios::binary);
    if (!fin) throw std::runtime_error("Cannot open column counts file");

    std::vector<int64_t> counts(num_columns);
    for (int64_t i = 0; i < num_columns; ++i) {
        fin.read(reinterpret_cast<char*>(&counts[i]), sizeof(int64_t));
        if (!fin) throw std::runtime_error("Unexpected EOF in column counts");
    }
    return counts;
}

void process_matrix_and_tally(
    const std::string& matrix_file,
    const std::vector<int64_t>& col_counts,
    const std::string& output_path,
    int64_t num_rows,
    int64_t num_cols
) {
    std::cout << "Reading matrix from: " << matrix_file << std::endl;
    std::ifstream fin(matrix_file, std::ios::binary);
    if (!fin) throw std::runtime_error("Cannot open matrix file");

    using RowIndex = int64_t;
    using ColIndex = int64_t;
    using Entry = std::pair<ColIndex, DecodeResult>;
    std::unordered_map<RowIndex, std::vector<Entry>> row_to_entries;

    int64_t total_entries = 0;
    int64_t max_row_index = -1;
    int64_t max_col_index = -1;

    // Step 1: Read and decode, fill map
    for (int64_t col = 0; col < static_cast<int64_t>(col_counts.size()); ++col) {
        for (int64_t i = 0; i < col_counts[col]; ++i) {
            int64_t row_index = 0, incode = 0;
            fin.read(reinterpret_cast<char*>(&row_index), sizeof(int64_t));
            fin.read(reinterpret_cast<char*>(&incode), sizeof(int64_t));
            if (!fin) throw std::runtime_error("Unexpected EOF in matrix");

            row_index -= 1;  // convert to 0-based
            DecodeResult decoded = decode(incode);
            row_to_entries[row_index].emplace_back(col, decoded);

            if (row_index > max_row_index) max_row_index = row_index;
            if (col > max_col_index) max_col_index = col;

            total_entries++;
        }
    }
    fin.close();

    std::cout << "Decoded " << total_entries << " entries"<< std::endl;
    std::cout << "Building CSR structures...\n";

    // Step 2: Build CSR arrays from map
    std::vector<int> row_offsets;
    std::vector<int> entry_cols;
    std::vector<DecodeResult> entry_fibs;

    row_offsets.reserve(num_rows + 1);
    row_offsets.push_back(0);

    int64_t running_count = 0;

    for (int64_t r = 0; r < num_rows; ++r) {
        auto it = row_to_entries.find(r);
        if (it != row_to_entries.end()) {
            for (const auto& [col, fib] : it->second) {
                entry_cols.push_back(static_cast<int>(col));
                entry_fibs.push_back(fib);
                running_count++;
            }
        }
        row_offsets.push_back(static_cast<int>(running_count));
    }

    std::cout << "CSR structure: " << entry_cols.size() << " non-zero entries across "
              << num_rows << " rows and " << num_cols << " columns."<<std::endl;

    // Step 3: Prepare result vector
    std::vector<std::pair<std::pair<int,int>, unsigned long long>> result;  // empty

    std::cout << "Calling GPU tally..."<<std::endl;
    tally_gpu(row_offsets, entry_cols, entry_fibs, result, num_rows, num_cols);
    std::cout << "Tally complete."<<std::endl;

    // Step 4: Output
    std::ofstream fout(output_path);
    if (!fout) throw std::runtime_error("Cannot open output file");

    int64_t lines_written = 0;
    for (const auto& entry : result) {
        int colA_0based = entry.first.first;
        int colB_0based = entry.first.second;
        unsigned long long count = entry.second;

        int colA_1based = colA_0based + 1;
        int colB_1based = colB_0based + 1;

        fout << colA_1based << " " << colB_1based << " " << count << "\n";
        lines_written++;
    }
    fout << num_cols << " " << num_cols << " 0\n";
    std::cout << "Output written to: " << output_path << " (" << lines_written << " lines + footer)"<<std::endl;
}



int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <meta> <counts> <matrix> <output>\n";
        return 1;
    }

    try {
        std::string meta_path   = argv[1];
        std::string counts_path = argv[2];
        std::string matrix_path = argv[3];
        std::string output_dot  = argv[4];

        auto [num_rows, num_cols] = read_num_rows_and_columns(meta_path);
        auto col_counts = read_column_counts(counts_path, num_cols);
        process_matrix_and_tally(matrix_path, col_counts, output_dot, num_rows, num_cols);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
