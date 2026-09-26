#ifndef EPIWORLDPY_MODEL_HPP
#define EPIWORLDPY_MODEL_HPP

#include "common.hpp"
#include <pybind11/pybind11.h>

#define MODEL_CHILD_TYPE(model)                                                \
	pybind11::class_<epiworld::epimodels::Model##model<int>,                   \
					 epiworld::Model<int>>

namespace epiworldpy {
/* An update function built in C++ (e.g., by UpdateFun.susceptible()). Python
 * only holds it and hands it back to the model unchanged. Converting the
 * std::function to a Python callable instead would call back into Python for
 * every agent and every day, and would hide its type from the model, which
 * recognizes sampler::UpdateSusceptible to push transmission. */
struct NativeUpdateFun {
	epiworld::UpdateFun<int> fun;
};

void export_native_update_fun(pybind11::class_<NativeUpdateFun> &c);
void export_update_fun(pybind11::class_<epiworld::UpdateFun<int>> &c);
void export_model(pybind11::class_<epiworld::Model<int>> &c);
void export_all_models(pybind11::module &m);
} // namespace epiworldpy

#endif /* EPIWORLDPY_MODEL_HPP */
