#include "entity.hpp"

#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace epiworld;
using namespace epiworldpy;
using namespace pybind11::literals;
namespace py = pybind11;

static epiworld::Entity<int> new_entity(std::string name,
                                        epiworld::EntityToAgentFun<int> fun) {
  Entity<int> entity(name, fun);

  return entity;
}

static epiworld::EntityToAgentFun<int> new_entity_to_agent_fun(
    const std::function<void(Entity<int> &, Model<int> *)> &fun) {
  return EntityToAgentFun<int>(fun);
}

void epiworldpy::export_entity(
    pybind11::class_<epiworld::Entity<int>,
                     std::shared_ptr<epiworld::Entity<int>>> &c) {
  c.def(py::init(&new_entity), "Add a list of user data.", py::arg("name"),
        py::arg("fun") = nullptr)
      .def_static("new_entity_to_agent_fun", &new_entity_to_agent_fun,
                  "Create a new entity to agent function from a lambda.",
                  py::arg("fun"));
}
