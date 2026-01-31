#include "lsh/VectorDB.h"
#include <fstream>
#include <cmath>
#include <algorithm>
#include <queue>
#include <set>

// ... (Constructor and hyperplanes remain same as previous step) ...

std::vector<float> VectorDB::get(int id) {
    std::lock_guard<std::mutex> lock(db_mutex);
    if (storage.find(id) != storage.end()) return storage[id].data;
    return {};
}

void VectorDB::remove(int id) {
    std::lock_guard<std::mutex> lock(db_mutex);
    if (storage.find(id) == storage.end()) return;

    // Remove from LSH tables
    std::vector<float> vec = storage[id].data;
    for (int i = 0; i < num_tables; i++) {
        uint32_t hv = hash_vector(vec, i);
        auto& bucket = hash_tables[i][hv];
        bucket.erase(std::remove(bucket.begin(), bucket.end(), id), bucket.end());
    }
    storage.erase(id);
}

void VectorDB::update(int id, const std::vector<float>& vec) {
    remove(id);
    insert(id, vec);
}

// (Insert, Query, Save/Load implementations follow existing logic with mutex protection)