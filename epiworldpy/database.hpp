#ifndef EPIWORLDPY_DATABASE_HPP
#define EPIWORLDPY_DATABASE_HPP

#include "common.hpp"
#include <pybind11/pybind11.h>

namespace epiworldpy {
void export_database(
	pybind11::class_<epiworld::DataBase<int>,
					 std::shared_ptr<epiworld::DataBase<int>>> &c);
}

#endif /* EPIWORLDPY_DATABASE_HPP */
