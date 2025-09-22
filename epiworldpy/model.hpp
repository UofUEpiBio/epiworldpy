#ifndef EPIWORLDPY_MODEL_HPP
#define EPIWORLDPY_MODEL_HPP

#include "common.hpp"
#include <pybind11/pybind11.h>

#define MODEL_CHILD_TYPE(model)                                                \
	pybind11::class_<epiworld::epimodels::Model##model<int>,                   \
					 epiworld::Model<int>>

namespace epiworldpy {
void export_update_fun(pybind11::class_<epiworld::UpdateFun<int>> &c);
void export_model(pybind11::class_<epiworld::Model<int>> &c);
void export_all_models(pybind11::module &m);
} // namespace epiworldpy

#endif /* EPIWORLDPY_MODEL_HPP */
