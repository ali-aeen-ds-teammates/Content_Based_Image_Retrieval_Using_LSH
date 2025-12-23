//
// Created by aeen on 12/23/25.
//

#include <vector>
#include <string>
#include <unordered_map>

#ifndef UNTITLED_VECTORDB_H
#define UNTITLED_VECTORDB_H
struct VectorRecord {
    int id;
    std::vector<float> data;
};

class VectorDB {
public:
    VectorDB(int input_dim, int num_hash_tables, int hash_size);

    // CRUD
    void insert(int id, const std::vector<float>& vec);
    std::vector<int> query(const std::vector<float>& query_vec, int k);

    //persistence
    void save_to_disk(const std::string& filename);
    void load_from_disk(const std::string& filename);

private:
    int dim;
    int num_tables; // L
    int num_bits; // K

    // Storage: ID -> Vector
    std::unordered_map<int, VectorRecord> storage;

    // LSH index: TableIndex -> (HashValue -> List of VectorIDs)
    std::vector<std::unordered_map<uint32_t, std::vector<int>>> hash_tables;

    // Hyperplanes for Random Projection: TableIndex -> BitIndex -> NormalVector
    std::vector<std::vector<std::vector<float>>> planes;

    // Helpers
    void generate_hyperplanes();
    uint32_t hash_vector(const std::vector<float>& vec, int table_idx);
    float cosine_dist(const std::vector<float>& a, const std::vector<float>& b);
};
#endif //UNTITLED_VECTORDB_H
