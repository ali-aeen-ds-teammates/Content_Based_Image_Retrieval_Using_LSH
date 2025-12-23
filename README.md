Here is a comprehensive `README.md`. It is designed to serve as the **master document** for your project. You should commit this to the root of your GitHub repository immediately.

It outlines the architecture, the technical specification, and the step-by-step roadmap without assigning specific names to tasks.

***

# Content-Based Image Retrieval (CBIR) System
### *SBU Data Structures & Algorithms - Fall 2025*

## 1. Project Overview
This project implements a high-performance **Vector Database** capable of indexing and retrieving images based on visual similarity.
*   **Core Logic:** Custom C++ Data Structures (Vector Storage, LSH Indexing).
*   **Intelligence:** Python/PyTorch (ResNet18 embeddings).
*   **Interface:** Tauri (Modern Desktop GUI).
*   **Communication:** Pybind11 (Direct memory access between C++ and Python).

## 2. Architecture: "The Hybrid Engine"
We utilize a **Shared Library Architecture** to combine raw C++ performance with Python's ML ecosystem.

1.  **Layer 1: The Engine (C++)**
    *   Compiles into a shared object (`.pyd` / `.so`).
    *   Handles **Persistence** (Binary File I/O).
    *   Handles **Indexing** (Locality Sensitive Hashing & Exact k-NN).
    *   Handles **Concurrency** (Thread-safe operations).
2.  **Layer 2: The API & Logic (Python/FastAPI)**
    *   Imports the C++ Engine using `pybind11`.
    *   Runs the ResNet18 model to convert Images $\to$ Vectors.
    *   Exposes a local REST API (`localhost:8000`) for the UI.
    *   Generates visualization plots (PCA/t-SNE).
3.  **Layer 3: The View (Tauri/React)**
    *   A lightweight frontend.
    *   Sends HTTP requests to the Python Layer.
    *   Displays image results and scatter plots.

---

## 3. Implementation Roadmap

### Phase 1: Infrastructure & Binding (The Foundation)
*Goal: Establish communication between languages.*
- [ ] **Project Setup:** Initialize Git, CMake (C++), Poetry/Pip (Python), and Tauri (Node.js).
- [ ] **Pybind11 Integration:** Configure CMake to compile a C++ file into a Python module.
- [ ] **"Hello World" Test:** Verify Python can import the C++ module and call a simple function `add(a, b)`.
- [ ] **Define Data Structures:** Create the `VectorDB` class skeleton in C++.

### Phase 2: Core Database & Persistence (The Basics)
*Goal: Save, load, and perform exact search.*
- [ ] **Binary Storage Design:** Implement `save_to_disk` and `load_from_disk`.
    - *Format:* Header (Metadata) + Body (Fixed-size records).
- [ ] **CRUD Operations:** Implement `insert`, `get_vector_by_id`, and `delete`.
- [ ] **Metric Implementation:** Implement `Euclidean Distance` and `Cosine Similarity` in C++.
- [ ] **Brute-Force Search:** Implement $O(N)$ linear scan for k-NN.
- [ ] **Python Integration:** Expose these methods to Python via Pybind11.

### Phase 3: The ML Pipeline & API
*Goal: Connect images to the database.*
- [ ] **Feature Extraction:** Implement PyTorch script to load pre-trained ResNet18 and strip the final layer.
- [ ] **API Setup:** Create FastAPI endpoints:
    - `POST /add`: Receives image $\to$ Generates Embedding $\to$ Calls C++ Insert.
    - `POST /search`: Receives image $\to$ Generates Embedding $\to$ Calls C++ Search.
- [ ] **Tauri Connection:** Build the frontend to upload an image and console log the API response.

### Phase 4: Advanced Algorithms (LSH)
*Goal: Sub-linear search performance.*
- [ ] **LSH Theory:** Implement **Random Projection (SimHash)** logic.
    - Generate $K$ random hyperplanes.
    - Compute hash signatures (bit manipulation).
- [ ] **Indexing:** Implement the `HashTable` logic (Bucketing) in C++.
- [ ] **Approximate Search:** Implement the retrieval logic (fetch bucket $\to$ filter candidates).
- [ ] **Optimization:** Tune hyperparameters (Number of planes, Number of tables) for accuracy vs. speed.

### Phase 5: Visualization & UI Polish
*Goal: Meet the GUI requirements.*
- [ ] **Scatter Plot:** Implement Python endpoint to run PCA/t-SNE on vectors and return a Plot image.
- [ ] **Gallery UI:** specific UI component to display search results (scrollable grid).
- [ ] **Visual Feedback:** Add loading spinners while ResNet processes images.

### Phase 6: Bonus & Benchmarking
*Goal: Extra credit and final report.*
- [ ] **Concurrency (Bonus):** Add `std::mutex` to C++ critical sections. Ensure thread safety during simultaneous API calls.
- [ ] **Performance Tests:** Script to run 100 queries and compare `BruteForce` vs `LSH` execution time.
- [ ] **Complexity Analysis:** Document Big-O notation for all operations.

---

## 4. Technical Specifications

### A. Directory Structure
```text
/
├── core_cpp/              # C++ Source
│   ├── CMakeLists.txt     # Build Config
│   ├── src/
│   │   ├── vectordb.cpp   # Main Class & Pybind defs
│   │   ├── storage.cpp    # Binary I/O logic
│   │   └── lsh.cpp        # Hashing logic
├── app_python/            # Python Server
│   ├── server.py          # FastAPI Entrypoint
│   ├── ml_engine.py       # ResNet Logic
│   └── tests/             # Benchmarking scripts
├── ui_tauri/              # Frontend
│   ├── src/               # React/TypeScript
│   └── src-tauri/         # Tauri Config
└── data/                  # Storage for .bin files
```

### B. Storage Format (Binary)
**File: `db_header.bin`**
*   `magic_number` (4 bytes): Validation
*   `vector_count` (4 bytes): $N$
*   `vector_dim` (4 bytes): $D=512$

**File: `vectors.bin`**
*   Repeated structure for every vector:
    *   `id` (4 bytes)
    *   `data` ($512 \times 4$ bytes = 2048 bytes)
    *   *Total Record Size:* 2052 bytes.

### C. API Contract
**Endpoint:** `POST /search`
*   **Input:** `multipart/form-data` (The Image File)
*   **Output:** JSON
    ```json
    {
      "time_taken_ms": 12,
      "method": "LSH",
      "results": [
        {"id": 402, "score": 0.98},
        {"id": 115, "score": 0.91}
      ]
    }
    ```

---

## 5. Development Workflow

### Step 1: Build the C++ Core
```bash
cd core_cpp
mkdir build && cd build
cmake ..
make
# This generates 'vectordb.so' or 'vectordb.pyd'
# Move this file to /app_python/
```

### Step 2: Run the Python Server
```bash
cd app_python
# Ensure vectordb.pyd is present here
uvicorn server:app --reload
```

### Step 3: Run the UI
```bash
cd ui_tauri
npm run tauri dev
```

---

## 6. Resources & References
1.  **Pybind11 Docs:** [pybind11.readthedocs.io](https://pybind11.readthedocs.io)
2.  **FastAPI:** [fastapi.tiangolo.com](https://fastapi.tiangolo.com)
3.  **LSH Theory:** *Mining of Massive Datasets* (Ch. 3) - Leskovec.
4.  **ResNet:** PyTorch Torchvision models.
