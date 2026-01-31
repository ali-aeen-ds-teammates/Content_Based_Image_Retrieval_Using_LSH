#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "lsh/VectorDB.h"

namespace py = pybind11;

PYBIND11_MODULE(lsh_core, m) {
    py::class_<VectorDB>(m, "VectorDB")
        .def(py::init<int, int, int>(), py::arg("dim"), py::arg("tables"), py::arg("bits"))
        .def("insert", &VectorDB::insert)
        .def("get", &VectorDB::get)
        .def("update", &VectorDB::update)
        .def("delete", &VectorDB::remove)
        .def("query", &VectorDB::query)
        .def("exact_query", &VectorDB::exact_query)
        .def("save", &VectorDB::save_to_disk)
        .def("load", &VectorDB::load_from_disk)
        .def("get_all", &VectorDB::get_all_vectors);
}