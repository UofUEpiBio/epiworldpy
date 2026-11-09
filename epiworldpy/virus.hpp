#ifndef EPIWORLDPY_VIRUS_HPP
#define EPIWORLDPY_VIRUS_HPP

#include "common.hpp"
#include <pybind11/pybind11.h>

namespace epiworldpy {
/* A virus and its natural history; the values and durations of its
 * transitions. */
void export_virus(pybind11::class_<epiworld::Virus> &c);
void export_value(pybind11::class_<epiworld::Value> &c);
void export_duration(pybind11::class_<epiworld::Duration> &c);
} // namespace epiworldpy

#endif /* EPIWORLDPY_VIRUS_HPP */
