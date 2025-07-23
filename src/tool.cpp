#include "tool.hpp"

using namespace epiworld;
using namespace epiworldpy;
namespace py = pybind11;

static epiworld::Tool<int>
new_tool(std::string name, double prevalence, bool as_proportion,
         double susceptibility_reduction, double transmission_reduction,
         double recovery_enhancer, double death_reduction) {
  Tool<int> tool(name, prevalence, as_proportion);

  if (susceptibility_reduction > 0) {
    tool.set_susceptibility_reduction(susceptibility_reduction);
  }

  if (transmission_reduction > 0) {
    tool.set_transmission_reduction(transmission_reduction);
  }

  if (recovery_enhancer > 0) {
    tool.set_recovery_enhancer(recovery_enhancer);
  }

  if (death_reduction > 0) {
    tool.set_death_reduction(death_reduction);
  }

  return tool;
}

static std::string get_name(epiworld::Tool<int> &self) {
  return self.get_name();
}

static void set_name(epiworld::Tool<int> &self, std::string name) {
  self.set_name(name);
}

static void set_susceptibility_reduction(epiworld::Tool<int> &self,
                                         double susceptibility_reduction) {
  self.set_susceptibility_reduction(susceptibility_reduction);
}

static void set_transmission_reduction(epiworld::Tool<int> &self,
                                       double transmission_reduction) {
  self.set_transmission_reduction(transmission_reduction);
}

static void set_recovery_enhancer(epiworld::Tool<int> &self,
                                  double recovery_enhancer) {
  self.set_recovery_enhancer(recovery_enhancer);
}

static void set_death_reduction(epiworld::Tool<int> &self,
                                double death_reduction) {
  self.set_death_reduction(death_reduction);
}

static void print_tool(epiworld::Tool<int> &self) { self.print(); }

void epiworldpy::export_tool(pybind11::class_<epiworld::Tool<int>> &c) {
  c.def(py::init(&new_tool), "Create a new tool", py::arg("name"),
        py::arg("prevalence"), py::arg("as_proportion"),
        py::arg("susceptibility_reduction") = 0,
        py::arg("transmission_reduction") = 0, py::arg("recovery_enhancer") = 0,
        py::arg("death_reduction") = 0)
      .def("get_name", &get_name, "Get the tool name.")
      .def("set_name", &set_name, "Set the tool name.", py::arg("name"))
      .def("set_susceptibility_reduction", &set_susceptibility_reduction,
           "Sets the reduction in agent susceptibility associated with this "
           "tool.",
           py::arg("susceptibility_reduction"))
      .def("set_transmission_reduction", &set_transmission_reduction,
           "Sets the reduction in agent transmission associated with this "
           "tool.",
           py::arg("transmission_reduction"))
      .def("set_recovery_enhancer", &set_recovery_enhancer,
           "Sets the boost in agent recovery associated with this "
           "tool.",
           py::arg("recovery_enhancer"))
      .def("set_death_reduction", &set_death_reduction,
           "Sets the reduction in agent mortality associated with this "
           "tool.",
           py::arg("death_reduction"))
      .def("print", &print_tool, "Print information about this tool.");
}
