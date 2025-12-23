cmake_minimum_required(VERSION 3.10)

find_package(pybind11 REQUIRED)

include_directories(include)

pybind11_add_module(lsh_core
    src/VectorDB.cpp
        bindings/bindings.cpp
)

target_compile_features(lsh_core PUBLIC cxx_std_17)