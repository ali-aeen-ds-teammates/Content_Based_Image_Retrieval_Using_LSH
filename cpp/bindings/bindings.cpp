//
// Created by aeen on 12/23/25.
//
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "lsh/VectorDB.h"

namespace py = pybind11;

PYBIND11_MODULE(lsh_core, m) {
    py::class_<VectorDB>(m, "VectorDB")
            .def(py::init<int, int, int>(),
                 py::arg("dim"), py::arg("num_tables"), py::arg("hash_size"))
            .def("insert", &VectorDB::insert)
            .def("query", &VectorDB::query)
            .def("save", &VectorDB::save_to_disk)
            .def("load", &VectorDB::load_from_disk);
}