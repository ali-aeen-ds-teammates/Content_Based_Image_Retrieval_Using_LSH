#ifndef UNTITLED_VECTORDB_H
#define UNTITLED_VECTORDB_H

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <mutex>

struct VectorRecord {
    int id;
    std::vector<float> data;
};

class VectorDB {
public:
    VectorDB(int input_dim, int num_hash_tables, int hash_size);

    // Full CRUD
    void insert(int id, const std::vector<float>& vec);
    std::vector<float> get(int id);           // Read
    void update(int id, const std::vector<float>& vec); // Update
    void remove(int id);                      // Delete
    
    // Search
    std::vector<int> query(const std::vector<float>& query_vec, int k);
    std::vector<int> exact_query(const std::vector<float>& query_vec, int k);

    // Persistence
    void save_to_disk(const std::string& filename);
    void load_from_disk(const std::string& filename);
    
    std::unordered_map<int, std::vector<float>> get_all_vectors();

private:
    int dim;
    int num_tables;
    int num_bits;
    std::unordered_map<int, VectorRecord> storage;
    std::vector<std::unordered_map<uint32_t, std::vector<int>>> hash_tables;
    std::vector<std::vector<std::vector<float>>> planes;
    mutable std::mutex db_mutex;

    void generate_hyperplanes();
    uint32_t hash_vector(const std::vector<float>& vec, int table_idx);
    float cosine_similarity(const std::vector<float>& a, const std::vector<float>& b);
};

#endif