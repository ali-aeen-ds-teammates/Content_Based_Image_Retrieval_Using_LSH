#include "lsh/VectorDB.h"
#include <fstream>
#include <cmath>
#include <algorithm>
#include <random>
#include <set>
#include <queue>

VectorDB::VectorDB(int input_dim, int num_hash_tables, int hash_size) 
    : dim(input_dim), num_tables(num_hash_tables), num_bits(hash_size) {
    generate_hyperplanes();
    hash_tables.resize(num_tables);
}

void VectorDB::generate_hyperplanes() {
    std::mt19937 rng(42);
    std::normal_distribution<float> dist(0.0, 1.0);
    planes.resize(num_tables);
    for (int i = 0; i < num_tables; i++) {
        planes[i].resize(num_bits);
        for (int j = 0; j < num_bits; j++) {
            planes[i][j].resize(dim);
            for (int d = 0; d < dim; d++) {
                planes[i][j][d] = dist(rng);
            }
        }
    }
}

float VectorDB::cosine_similarity(const std::vector<float>& a, const std::vector<float>& b) {
    float dot = 0.0f, norm_a = 0.0f, norm_b = 0.0f;
    for (int i = 0; i < dim; ++i) {
        dot += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }
    if (norm_a == 0 || norm_b == 0) return 0.0f;
    return dot / (std::sqrt(norm_a) * std::sqrt(norm_b));
}

uint32_t VectorDB::hash_vector(const std::vector<float> &vec, int table_idx) {
    uint32_t hash = 0;
    for (int i = 0; i < num_bits; i++) {
        float dot = 0.0f;
        for (int d = 0; d < dim; d++) {
            dot += vec[d] * planes[table_idx][i][d];
        }
        if (dot > 0) hash |= (1u << i);
    }
    return hash;
}

void VectorDB::insert(int id, const std::vector<float> &vec) {
    std::lock_guard<std::mutex> lock(db_mutex);
    storage[id] = {id, vec};
    for (int i = 0; i < num_tables; i++) {
        uint32_t hv = hash_vector(vec, i);
        hash_tables[i][hv].push_back(id);
    }
}

std::vector<int> VectorDB::exact_query(const std::vector<float>& query_vec, int k) {
    std::lock_guard<std::mutex> lock(db_mutex);
    std::priority_queue<std::pair<float, int>> pq;
    for (const auto& [id, record] : storage) {
        pq.push({cosine_similarity(query_vec, record.data), id});
    }
    std::vector<int> results;
    for (int i = 0; i < k && !pq.empty(); i++) {
        results.push_back(pq.top().second);
        pq.pop();
    }
    return results;
}

std::vector<int> VectorDB::query(const std::vector<float>& query_vec, int k) {
    std::lock_guard<std::mutex> lock(db_mutex);
    std::set<int> candidates;
    for (int i = 0; i < num_tables; i++) {
        uint32_t hv = hash_vector(query_vec, i);
        if (hash_tables[i].count(hv)) {
            for (int id : hash_tables[i][hv]) candidates.insert(id);
        }
    }
    std::priority_queue<std::pair<float, int>> pq;
    for (int id : candidates) {
        pq.push({cosine_similarity(query_vec, storage[id].data), id});
    }
    std::vector<int> results;
    for (int i = 0; i < k && !pq.empty(); i++) {
        results.push_back(pq.top().second);
        pq.pop();
    }
    return results;
}

void VectorDB::save_to_disk(const std::string& filename) {
    std::lock_guard<std::mutex> lock(db_mutex);
    std::ofstream out(filename, std::ios::binary);
    if (!out) return;
    out.write(reinterpret_cast<const char*>(&dim), sizeof(int));
    uint64_t size = storage.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(uint64_t));
    for (const auto& [id, record] : storage) {
        out.write(reinterpret_cast<const char*>(&id), sizeof(int));
        out.write(reinterpret_cast<const char*>(record.data.data()), dim * sizeof(float));
    }
}

void VectorDB::load_from_disk(const std::string& filename) {
    std::lock_guard<std::mutex> lock(db_mutex);
    std::ifstream in(filename, std::ios::binary);
    if (!in) return;
    storage.clear();
    for (auto& t : hash_tables) t.clear();
    int file_dim;
    uint64_t count;
    in.read(reinterpret_cast<char*>(&file_dim), sizeof(int));
    in.read(reinterpret_cast<char*>(&count), sizeof(uint64_t));
    for (uint64_t i = 0; i < count; i++) {
        int id;
        std::vector<float> vec(file_dim);
        in.read(reinterpret_cast<char*>(&id), sizeof(int));
        in.read(reinterpret_cast<char*>(vec.data()), file_dim * sizeof(float));
        // Manual internal insert to avoid double locking
        storage[id] = {id, vec};
        for (int t = 0; t < num_tables; t++) {
            hash_tables[t][hash_vector(vec, t)].push_back(id);
        }
    }
}

std::unordered_map<int, std::vector<float>> VectorDB::get_all_vectors() {
    std::lock_guard<std::mutex> lock(db_mutex);
    std::unordered_map<int, std::vector<float>> result;
    for (const auto& [id, rec] : storage) result[id] = rec.data;
    return result;
}