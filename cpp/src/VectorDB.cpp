//
// Created by aeen on 12/23/25.
//
#include "lsh/VectorDB.h"
#include <fstream>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>

VectorDB::VectorDB(int input_dim, int num_hash_tables, int hash_size) : dim(input_dim), num_tables(num_hash_tables), num_bits(hash_size)
{
    generate_hyperplanes();
    hash_tables.resize(num_tables);
}

void VectorDB::generate_hyperplanes() {
    std::mt19937 rng(42); // fixed seed for reproducibility
    std::normal_distribution<float> dist(0.0, 1.0);

    planes.resize(num_tables);
    for (int i = 0; i < num_tables; i++){
        planes[i].resize(num_bits);
        for (int j = 0; j < num_bits; j++) {
            planes[i][j].resize(dim);
            for (int d = 0; d < dim; d++) {
                planes[i][j][d] = dist(rng);
            }
        }
    }
}

uint32_t VectorDB::hash_vector(const std::vector<float> &vec, int table_idx) {
    uint32_t hash = 0;
    for (int i = 0; i < num_bits; i++) {
        float dot = 0.0f;
        for (int d = 0; d < dim; d++) {
            dot += vec[d] * planes[table_idx][i][d];
        }
        if (dot > 0) hash |= (i << 1);
    }
    return hash;
}

void VectorDB::insert(int id, const std::vector<float> &vec) {
    
}