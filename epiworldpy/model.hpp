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

void export_diffnet(MODEL_CHILD_TYPE(DiffNet) & c);
void export_seir(MODEL_CHILD_TYPE(SEIR) & c);
void export_seirconn(MODEL_CHILD_TYPE(SEIRCONN) & c);
void export_seird(MODEL_CHILD_TYPE(SEIRD) & c);
void export_sir(MODEL_CHILD_TYPE(SIR) & c);
void export_sirconn(MODEL_CHILD_TYPE(SIRCONN) & c);
void export_sird(MODEL_CHILD_TYPE(SIRD) & c);
void export_sirdconn(MODEL_CHILD_TYPE(SIRDCONN) & c);
void export_sis(MODEL_CHILD_TYPE(SIS) & c);
void export_sisd(MODEL_CHILD_TYPE(SISD) & c);
void export_surv(MODEL_CHILD_TYPE(SURV) & c);
} // namespace epiworldpy

#endif /* EPIWORLDPY_MODEL_HPP */
