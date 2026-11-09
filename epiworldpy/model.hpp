#ifndef EPIWORLDPY_MODEL_HPP
#define EPIWORLDPY_MODEL_HPP

#include "common.hpp"
#include <pybind11/pybind11.h>

namespace epiworldpy {
void export_model(pybind11::class_<epiworld::Model> &c);
/* The registry (make_model(), list_models(), get_model_info()) and the
 * models that keep their own constructor (ModelDiffNet). */
void export_all_models(pybind11::module &m);
} // namespace epiworldpy

#endif /* EPIWORLDPY_MODEL_HPP */
