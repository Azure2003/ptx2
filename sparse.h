#ifndef sparse_h
#define sparse_h
#include <iostream>
#include <map>

template <typename T>
class SparseMatrix {
public:
    std::map<int, std::map<int, T>> data; // row -> (col -> value)
    int rows, cols;

    SparseMatrix() : rows(0), cols(0) {}

    SparseMatrix(int r, int c) : rows(r), cols(c) {}

    void resize(int r, int c) {
        rows = r;
        cols = c;
        data.clear();
    }

    void set(int r, int c, T value) {
        if (value != T{})
            data[r][c] = value;
        else if (data.count(r))
            data[r].erase(c);
    }

    T get(int r, int c) const {
        auto row_it = data.find(r);
        if (row_it != data.end()) {
            auto col_it = row_it->second.find(c);
            if (col_it != row_it->second.end())
                return col_it->second;
        }
        return T{};
    }

    void print() const {
        for (const auto& row : data) {
            for (const auto& col : row.second) {
                std::cout << "Row " << row.first << ", Col " << col.first 
                          << " -> " << col.second << "\n";
            }
        }
    }

    // Returns the number of non-zero elements in a specific row
int rowSize(int row) const {
    auto it = data.find(row);
    return (it != data.end()) ? it->second.size() : 0;
}

// Returns a reference to the map of columns and values in a specific row
const std::map<int, T>& getRow(int row) const {
    static const std::map<int, T> empty_row;
    auto it = data.find(row);
    return (it != data.end()) ? it->second : empty_row;
}
};

#endif
