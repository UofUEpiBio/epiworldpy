#include "virus.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace epiworld;
using namespace epiworldpy;
using namespace pybind11::literals;
namespace py = pybind11;

static epiworld::Virus<int> new_virus(std::string name, double prevalence,
                                      bool as_proportion, double prob_infecting,
                                      double prob_recovery, double prob_death,
                                      double post_immunity, double incubation) {
  Virus<int> virus(name, prevalence, as_proportion);

  virus.set_prob_infecting(prob_infecting);
  virus.set_prob_recovery(prob_recovery);
  virus.set_prob_death(prob_death);
  virus.set_incubation(incubation);

  if (post_immunity > 0.0) {
    virus.set_post_immunity(post_immunity);
  }

  return virus;
}

static py::dict get_queue(epiworld::Virus<int> virus) {
  epiworld_fast_int init;
  epiworld_fast_int end;
  epiworld_fast_int removed;

  virus.get_queue(&init, &end, &removed);

  /* Return to Python. */
  py::dict ret("init"_a = init, "end"_a = end, "removed"_a = removed);

  return ret;
}

static py::dict get_state(epiworld::Virus<int> virus) {
  epiworld_fast_int init;
  epiworld_fast_int end;
  epiworld_fast_int removed;

  virus.get_state(&init, &end, &removed);

  /* Return to Python. */
  py::dict ret("init"_a = init, "end"_a = end, "removed"_a = removed);

  return ret;
}

static void print_virus(epiworld::Virus<int> virus) { virus.print(); }

void epiworldpy::export_virus(pybind11::class_<epiworld::Virus<int>> &c) {
  c.def(py::init(&new_virus), "Create a new virus (evil...)", py::arg("name"),
        py::arg("prevalence"), py::arg("as_proportion"),
        py::arg("prob_infecting"), py::arg("prob_recovery"),
        py::arg("prob_death"), py::arg("post_immunity"), py::arg("incubation"))
      .def("get_name", &epiworld::Virus<int>::get_name, "Get the tool name.")
      .def("set_name", &epiworld::Virus<int>::set_name, "Set the tool name.",
           py::arg("name"))
      .def("set_state", &epiworld::Virus<int>::set_state, "Set some state.",
           py::arg("init"), py::arg("end"), py::arg("removed"))
      .def("set_prob_infecting",
           pybind11::detail::overload_cast_impl<epiworld_double>()(
               &epiworld::Virus<int>::set_prob_infecting),
           "Set the probability of infection.", py::arg("prob_infecting"))
      .def("set_prob_recovery",
           pybind11::detail::overload_cast_impl<epiworld_double>()(
               &epiworld::Virus<int>::set_prob_recovery),
           "Set the probability for recovery.", py::arg("prob_recovery"))
      .def("set_prob_death",
           pybind11::detail::overload_cast_impl<epiworld_double>()(
               &epiworld::Virus<int>::set_prob_death),
           "Set the probability for mortality.", py::arg("prob_death"))
      .def("set_incubation",
           pybind11::detail::overload_cast_impl<epiworld_double>()(
               &epiworld::Virus<int>::set_incubation),
           "Set the incubation period.", py::arg("incubation"))
      .def("set_post_recovery", &epiworld::Virus<int>::set_post_recovery, "",
           py::arg("Set the post recovery."))
      .def("set_post_immunity",
           pybind11::detail::overload_cast_impl<epiworld_double>()(
               &epiworld::Virus<int>::set_post_immunity),
           "Set the post immunity.", py::arg(""))
      .def("set_distribution_fun", &epiworld::Virus<int>::set_distribution,
           "Set the distribution function.", py::arg("fun"))
      .def("set_prob_infecting_fun",
           &epiworld::Virus<int>::set_prob_infecting_fun,
           "Set the probability of infection callback.", py::arg("fun"))
      .def("set_prob_recovery_fun",
           &epiworld::Virus<int>::set_prob_recovery_fun,
           "Set the probability of recovery callback.", py::arg("fun"))
      .def("set_prob_death_fun", &epiworld::Virus<int>::set_prob_death_fun,
           "Set the probability of death callback.", py::arg("fun"))
      .def("set_incubation_fun", &epiworld::Virus<int>::set_incubation_fun,
           "Set the incubation callback.", py::arg("fun"))
      .def("set_queue", &epiworld::Virus<int>::set_queue, py::arg("init"),
           py::arg("end"), py::arg("removed"))
      .def("set_date", &epiworld::Virus<int>::set_date, py::arg("date"))
      .def("set_sequence", &epiworld::Virus<int>::set_sequence,
           "Set the sequence value.", py::arg("sequence"))
      .def("get_queue", &get_queue, "Get the queue.")
      .def("get_state", &get_state, "Set some state.")
      .def("get_incubation", &epiworld::Virus<int>::get_incubation,
           "Get the incubation value.", py::arg("model"))
      .def("get_prob_infecting", &epiworld::Virus<int>::get_prob_infecting,
           "Get the probability of infection.", py::arg("model"))
      .def("get_prob_recovery", &epiworld::Virus<int>::get_prob_recovery,
           "Get the probability of recovery.", py::arg("model"))
      .def("get_prob_death", &epiworld::Virus<int>::get_prob_death,
           "Get the probability of death.", py::arg("model"))
      .def("get_id", &epiworld::Virus<int>::get_id, "Get the ID of this virus.")
      .def("get_date", &epiworld::Virus<int>::get_date, "Get the date.") // ?
      .def("distribute", &epiworld::Virus<int>::distribute, py::arg("model"))
      .def("post_recovery", &epiworld::Virus<int>::post_recovery,
           py::arg("model"))
      .def("print", &print_virus, "Print information about this virus.");
}

static VirusToAgentFun<int> new_distribution_fun(
    const std::function<void(Virus<int> &, Model<int> *)> &fun) {
  return VirusToAgentFun<int>(fun);
}

static VirusFun<int> new_virus_fun(
    std::function<epiworld_double(Agent<int> *, Virus<int> &, Model<int> *)>
        &fun) {
  return VirusFun<int>(fun);
}

static VirusToAgentFun<int> new_random_distribution(double prevalence,
                                                    bool as_proportion) {
  return distribute_virus_randomly(prevalence);
}

static VirusToAgentFun<int> new_distribute_to_set(std::vector<size_t> ids) {
  /* epiworldr does a negative check, do we need to do this? */
  return distribute_virus_to_set(ids);
}

void epiworldpy::export_virus_to_agent_fun(
    pybind11::class_<epiworld::VirusToAgentFun<int>> &c) {
  c.def_static("new_virus_fun", &new_virus_fun,
               "Create a new, generic, virus callback function.",
               py::arg("fun"))
      .def_static("new_distribution_fun", &new_distribution_fun,
                  "Create a new distribution function based off a lambda.",
                  py::arg("fun"))
      .def_static("new_random_distribution", &new_random_distribution,
                  "Randomly infect agents in the model.")
      .def_static("new_distribute_to_set", &new_distribute_to_set,
                  "Infect a set of agents in the model.", py::arg("ids"));
}
