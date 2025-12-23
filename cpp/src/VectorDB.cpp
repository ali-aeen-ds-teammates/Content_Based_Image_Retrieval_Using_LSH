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
        if (dot > 0) hash |= (1 << i);
    }
    return hash;
}

void VectorDB::insert(int id, const std::vector<float> &vec) {
    if (vec.size() != dim) throw std::runtime_error("Dimension mismatch!");

    // Store Data
    storage[id] = {id, vec};

    // Index Data (LSH)
    for (int i = 0; i < num_tables; i++) {
        uint32_t hash = hash_vector(vec, i);
        hash_tables[i][hash].push_back(id);
    }
}

float VectorDB::cosine_dist(const std::vector<float> &a, const std::vector<float> &b) {
    float dot = 0, norm_a = 0, norm_b = 0;
    for (size_t i = 0; i < a.size(); i++) {
        dot += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }
    return 1.0f - (dot / std::sqrt(norm_a) * std::sqrt(norm_b));
}

std::vector<int> VectorDB::query(const std::vector<float> &query_vec, int k) {
    std::unordered_map<int, bool> candidates;

    // Collect candidates from buckets (LSH)
    for (int i = 0; i < num_tables; i++){
        uint32_t hash = hash_vector(query_vec, i);
        if (hash_tables[i].count(hash)) {
            for(int id : hash_tables[i][hash]) {
                candidates[id] = true;
            }
        }
    }

    // Exact Search on candidates (Reranking)
    std::vector<std::pair<float, int>> results;
    for (auto const& [id, _] : candidates) {
        float dist = cosine_dist(query_vec, storage[id].data);
        results.push_back({dist, id});
    }

    // Sort and return top K
    std::sort(results.begin(), results.end());

    std::vector<int> final_ids;
    for (int i = 0; i < std::min((int)results.size(), k); i++) {
        final_ids.push_back(results[i].second);
    }

    return final_ids;
}

void VectorDB::save_to_disk(const std::string &filename) {
    std::ofstream out(filename, std::ios::binary);
    size_t size = storage.size();
    out.write((char*)&size, sizeof(size));

    for(auto const& [id, record] : storage) {
        out.write((char*)&record.id, sizeof(record.id));
        out.write((char*)record.data.data(), dim * sizeof(float));
    }
    out.close();
}

void VectorDB::load_from_disk(const std::string &filename) {
    std::ifstream in(filename, std::ios::binary);
    if(!in.is_open()) return;

    size_t size;
    in.read((char*)&size, sizeof(size));

    storage.clear();
    // Rebuild hash tables
    for (auto& t : hash_tables) t.clear();

    for (size_t i = 0; i < size; i++) {
        int id;
        std::vector<float> vec(dim);
        in.read((char*)&id, sizeof(id));
        in.read((char*)vec.data(), dim * sizeof(float));

        insert(id, vec); // Re-insert to rebuild index in memory
    }
    in.close();
}