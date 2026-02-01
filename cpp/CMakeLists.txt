cmake_minimum_required(VERSION 3.10)

# Architect Note: Auto-detect pybind11 path using Python
execute_process(
    COMMAND python -c "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT PYBIND11_CMAKE_DIR)
    message(FATAL_ERROR "pybind11 not found. Run 'pip install pybind11' first.")
endif()

# Set the directory so find_package can see it
set(pybind11_DIR ${PYBIND11_CMAKE_DIR})
find_package(pybind11 REQUIRED)

include_directories(include)

pybind11_add_module(lsh_core
    src/VectorDB.cpp
    bindings/bindings.cpp
)

target_compile_features(lsh_core PUBLIC cxx_std_20)