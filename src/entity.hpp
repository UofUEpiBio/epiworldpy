#ifndef EPIWORLDPY_ENTITY_HPP
#define EPIWORLDPY_ENTITY_HPP

#include "epiworld-common.hpp"

namespace epiworldpy {
void export_entity(pybind11::class_<epiworld::Entity<int>,
									std::shared_ptr<epiworld::Entity<int>>> &c);
};

#endif /* EPIWORLDPY_ENTITY_HPP */
