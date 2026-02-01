cmake_minimum_required(VERSION 3.10)
project(lsh_core LANGUAGES CXX)

execute_process(
        COMMAND python3 -c "import pybind11; print(pybind11.get_cmake_dir())"
        OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT PYBIND11_CMAKE_DIR)
    message(FATAL_ERROR "pybind11 not found. Run 'pip install pybind11' first.")
endif()

set(pybind11_DIR ${PYBIND11_CMAKE_DIR})
find_package(pybind11 REQUIRED)

pybind11_add_module(lsh_core
        src/VectorDB.cpp
        bindings/bindings.cpp
)

target_include_directories(lsh_core PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(lsh_core PUBLIC cxx_std_20)
