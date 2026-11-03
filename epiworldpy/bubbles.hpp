#ifndef EPIWORLDPY_BUBBLES_HPP
#define EPIWORLDPY_BUBBLES_HPP

#include "common.hpp"
#include <pybind11/pybind11.h>

namespace epiworldpy {
/* The social-bubbles intervention, epiworld's Bubbles global event. */
void export_bubbles(pybind11::class_<epiworld::Bubbles<int>> &c);
} // namespace epiworldpy

#endif /* EPIWORLDPY_BUBBLES_HPP */
