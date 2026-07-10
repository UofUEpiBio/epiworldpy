#include "virus.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <utility>

using namespace epiworld;
using namespace epiworldpy;
using namespace pybind11::literals;
namespace py = pybind11;

static auto new_virus(std::string name, double prevalence, bool as_proportion,
					  double prob_infecting, double prob_recovery,
					  double prob_death, double post_immunity,
					  double incubation) -> Virus<int> {
	Virus<int> virus(std::move(name), prevalence, as_proportion);

	virus.set_prob_infecting(prob_infecting);
	virus.set_prob_recovery(prob_recovery);
	virus.set_prob_death(prob_death);
	virus.set_incubation(incubation);

	if (post_immunity > 0.0) {
		virus.set_post_immunity(post_immunity);
	}

	return virus;
}

static auto get_queue(Virus<int> &virus) -> py::dict {
	epiworld_fast_int init;
	epiworld_fast_int end;
	epiworld_fast_int removed;

	virus.get_queue(&init, &end, &removed);

	/* Return to Python. */
	py::dict ret("init"_a = init, "end"_a = end, "removed"_a = removed);

	return ret;
}

static auto get_state(Virus<int> &virus) -> py::dict {
	epiworld_fast_int init;
	epiworld_fast_int end;
	epiworld_fast_int removed;

	virus.get_state(&init, &end, &removed);

	/* Return to Python. */
	py::dict ret("init"_a = init, "end"_a = end, "removed"_a = removed);

	return ret;
}

void epiworldpy::export_virus(py::class_<Virus<int>> &c) {
	c.def(py::init(&new_virus), "Create a new virus.", py::arg("name"),
		  py::arg("prevalence"), py::arg("as_proportion"),
		  py::arg("prob_infecting"), py::arg("prob_recovery"),
		  py::arg("prob_death"), py::arg("post_immunity") = 0.0,
		  py::arg("incubation") = 0.0)
		.def("get_id", &Virus<int>::get_id, "Get the ID of this virus.")
		.def("get_name", &Virus<int>::get_name, "Get the virus name.")
		.def("set_name", &Virus<int>::set_name, "Set the virus name.",
			 py::arg("name"))
		.def("get_date", &Virus<int>::get_date, "Get the date introduced.")
		.def("set_date", &Virus<int>::set_date, "Set the date introduced.",
			 py::arg("date"))
		.def(
			"set_prob_infecting",
			py::overload_cast<epiworld_double>(&Virus<int>::set_prob_infecting),
			"Set the probability of infection (constant).",
			py::arg("prob_infecting"))
		.def("set_prob_recovery",
			 py::overload_cast<epiworld_double>(&Virus<int>::set_prob_recovery),
			 "Set the probability of recovery (constant).",
			 py::arg("prob_recovery"))
		.def("set_prob_death",
			 py::overload_cast<epiworld_double>(&Virus<int>::set_prob_death),
			 "Set the probability of death (constant).", py::arg("prob_death"))
		.def("set_incubation",
			 py::overload_cast<epiworld_double>(&Virus<int>::set_incubation),
			 "Set the incubation period (constant).", py::arg("incubation"))
		.def("set_prob_infecting",
			 py::overload_cast<std::string>(&Virus<int>::set_prob_infecting),
			 "Bind probability of infection to a model parameter.",
			 py::arg("param"))
		.def("set_prob_recovery",
			 py::overload_cast<std::string>(&Virus<int>::set_prob_recovery),
			 "Bind probability of recovery to a model parameter.",
			 py::arg("param"))
		.def("set_prob_death",
			 py::overload_cast<std::string>(&Virus<int>::set_prob_death),
			 "Bind probability of death to a model parameter.",
			 py::arg("param"))
		.def("set_incubation",
			 py::overload_cast<std::string>(&Virus<int>::set_incubation),
			 "Bind incubation period to a model parameter.", py::arg("param"))
		.def("get_prob_infecting", &Virus<int>::get_prob_infecting,
			 "Get the probability of infection.", py::arg("model"))
		.def("get_prob_recovery", &Virus<int>::get_prob_recovery,
			 "Get the probability of recovery.", py::arg("model"))
		.def("get_prob_death", &Virus<int>::get_prob_death,
			 "Get the probability of death.", py::arg("model"))
		.def("get_incubation", &Virus<int>::get_incubation,
			 "Get the incubation period.", py::arg("model"))
		.def("set_prob_infecting_fun", &Virus<int>::set_prob_infecting_fun,
			 "Set the probability-of-infection callback.", py::arg("fun"))
		.def("set_prob_recovery_fun", &Virus<int>::set_prob_recovery_fun,
			 "Set the probability-of-recovery callback.", py::arg("fun"))
		.def("set_prob_death_fun", &Virus<int>::set_prob_death_fun,
			 "Set the probability-of-death callback.", py::arg("fun"))
		.def("set_incubation_fun", &Virus<int>::set_incubation_fun,
			 "Set the incubation callback.", py::arg("fun"))
		.def("set_mutation", &Virus<int>::set_mutation,
			 "Set the mutation callback.", py::arg("fun"))
		.def("set_post_recovery", &Virus<int>::set_post_recovery,
			 "Set the post-recovery callback.", py::arg("fun"))
		.def("set_post_immunity",
			 py::overload_cast<epiworld_double>(&Virus<int>::set_post_immunity),
			 "Set post-recovery immunity (constant).", py::arg("prob"))
		.def("set_post_immunity",
			 py::overload_cast<std::string>(&Virus<int>::set_post_immunity),
			 "Bind post-recovery immunity to a model parameter.",
			 py::arg("param"))
		.def("post_recovery", &Virus<int>::post_recovery,
			 "Execute the post-recovery callback.", py::arg("model"))
		.def("set_state", &Virus<int>::set_state,
			 "Set the state transitions for this virus.", py::arg("init"),
			 py::arg("end"), py::arg("removed"))
		.def("set_queue", &Virus<int>::set_queue,
			 "Set the queue transitions for this virus.", py::arg("init"),
			 py::arg("end"), py::arg("removed"))
		.def("get_state", &get_state, "Get the state transitions.")
		.def("get_queue", &get_queue, "Get the queue transitions.")
		.def("set_distribution", &Virus<int>::set_distribution,
			 "Set the initial distribution function.", py::arg("fun"))
		.def("distribute", &Virus<int>::distribute,
			 "Distribute the virus according to its distribution function.",
			 py::arg("model"))
		.def("set_sequence", &Virus<int>::set_sequence,
			 "Set the genetic sequence.", py::arg("sequence"))
		.def("mutate", &Virus<int>::mutate, "Trigger a mutation event.",
			 py::arg("model"))
		.def("print", &Virus<int>::print,
			 "Print information about this virus.");
}

static auto
new_distribution_fun(const std::function<void(Virus<int> &, Model<int> *)> &fun)
	-> VirusToAgentFun<int> {
	return {fun};
}

static auto new_virus_fun(
	std::function<epiworld_double(Agent<int> *, Virus<int> &, Model<int> *)>
		&fun) -> VirusFun<int> {
	return {fun};
}

static auto new_random_distribution(double prevalence, bool as_proportion)
	-> VirusToAgentFun<int> {
	return distribute_virus_randomly(prevalence);
}

static auto new_distribute_to_set(std::vector<size_t> ids)
	-> VirusToAgentFun<int> {
	/* epiworldr does a negative check, do we need to do this? */
	return distribute_virus_to_set(std::move(ids));
}

void epiworldpy::export_virus_to_agent_fun(
	pybind11::class_<VirusToAgentFun<int>> &c) {
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
