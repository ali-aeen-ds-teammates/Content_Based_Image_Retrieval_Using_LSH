# SBU Fall 2025: CBIR Vector Database Project

This project implements a high-performance Content-Based Image Retrieval (CBIR) system featuring a custom-built Vector Database in C++ with LSH indexing and a React/FastAPI frontend.

## 1. Prerequisites

### Windows
- **Visual Studio 2022**: Install with "Desktop development with C++" workload.
- **CMake**: Version 3.10 or higher.
- **Python**: 3.10 - 3.13 (Must be added to PATH).
- **Node.js**: 18.x or 20.x (LTS recommended).

### Ubuntu / Linux
- **Build Essentials**: `sudo apt update && sudo apt install build-essential cmake python3-dev python3-pip nodejs npm` 

---

## 2. Installation & Build Step-by-Step

### Step 1: Clone the Repository
```bash
git clone <repository-url>
cd <project-folder>
```

### Step 2: Build C++ Core (Python Bindings)
The C++ logic must be compiled into a Python module using `pybind11`.

```bash
# Install pybind11 first so CMake can find it
pip install pybind11

# Navigate to C++ directory and build
cd cpp
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

**Architect Note**: After building, locate the compiled library (e.g., `lsh_core.cp313-win_amd64.pyd` on Windows or `lsh_core.so` on Linux). Copy this file into the `python/` directory so the scripts can import it.

### Step 3: Setup Python Environment
```bash
cd ../../python
pip install -r requirements.txt
```

### Step 4: Dataset Preparation
1. Create the data folder: `mkdir data` inside the `python/` directory.
2. Download the **Caltech-256** dataset (or your specific image set).
3. Ensure the structure is: `python/data/256_ObjectCategories/<category_folders>/<images.jpg>`.

### Step 5: Indexing the Manifold
Run the bulk indexer to extract ResNet-18 features and store them in the custom binary database.
```bash
python -c "from scripts.bulk_index import run_indexing; run_indexing()"
```
This generates `data/vectors.bin` (Binary I/O) and `data/metadata.json`.

### Step 6: Start the Backend API
```bash
# Remaining in the python/ directory
python -m uvicorn server:app --reload
```

### Step 7: Start the React UI
Open a new terminal window:
```bash
cd ui_react
npm install
npm run dev
```

---

## 3. Usage
1. Open the URL provided by Vite (usually `http://localhost:5173`).
2. Upload an image to the **Neural Query Input**.
3. Observe the speed comparison between **LSH Approximate Search** and **Brute-Force Exact Search**.
4. View the 2D Manifold projection generated via PCA on the dashboard.