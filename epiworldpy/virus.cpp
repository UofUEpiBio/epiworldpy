#include "virus.hpp"
#include "docstrings/virus.hpp"
#include "natural_history.hpp"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <optional>
#include <utility>

using namespace epiworld;
using namespace epiworldpy;
namespace py = pybind11;
namespace doc = epiworldpy::docstrings::virus;

// The declaring methods return the virus, so they can be chained.
static constexpr auto self_ref = py::return_value_policy::reference;

// Names the transition just declared, if given a name
static void name_transition(TransitionRef ref,
							const std::optional<std::string> &name) {
	if (name)
		ref.name(*name);
}

static auto get_stages(const Virus &self) -> py::dict {
	py::list stage, carrier, infectious, contact;
	if (const NaturalHistory *nh = self.get_natural_history())
		for (int s = 0; s < static_cast<int>(nh->get_n_stages()); ++s) {
			stage.append(nh->get_stages()[static_cast<size_t>(s)]);
			carrier.append(nh->is_carrier(s));
			infectious.append(nh->is_infectious(s));
			contact.append(nh->get_contact(s));
		}
	return py::dict(py::arg("stage") = stage, py::arg("carrier") = carrier,
					py::arg("infectious") = infectious,
					py::arg("contact") = contact);
}

static auto py_value_fun(py::function fun) -> Value {
	// Calls Python each time the value is needed (once per agent and day it
	// applies to)
	return Value(ValueFun([fun](Agent &agent, Model &model) {
		return fun(py::cast(&agent, py::return_value_policy::reference),
				   py::cast(&model, py::return_value_policy::reference))
			.cast<epiworld_double>();
	}));
}

void epiworldpy::export_virus(py::class_<Virus> &c) {
	c.def(py::init<std::string>(), doc::init, py::arg("name"))
		.def("get_id", &Virus::get_id, doc::get_id)
		.def("get_name", &Virus::get_name, doc::get_name)
		.def("get_lineage_id", &Virus::get_lineage_id, doc::get_lineage_id)
		.def("set_name", &Virus::set_name, doc::set_name, py::arg("name"))
		.def("get_date", &Virus::get_date, doc::get_date)
		.def("set_date", &Virus::set_date, doc::set_date, py::arg("date"))
		.def(
			"add_stage",
			[](Virus &self, std::string label, bool infectious,
			   std::optional<bool> carrier) -> Virus & {
				const bool carries = carrier.value_or(infectious);
				if (infectious && !carries)
					throw std::invalid_argument(
						"An infectious stage is a carrier stage: leave "
						"`carrier` out, or set it to True.");
				self.add_stage(std::move(label),
							   infectious ? stage::infectious
										  : (carries ? stage::carrier : 0u));
				return self;
			},
			self_ref, doc::add_stage, py::arg("label"),
			py::arg("infectious") = false, py::arg("carrier") = py::none())
		.def(
			"set_contact",
			[](Virus &self, const std::string &stage,
			   epiworld_double weight) -> Virus & {
				self.set_contact(stage_from_label(self, stage), weight);
				return self;
			},
			self_ref, doc::set_contact, py::arg("stage"), py::arg("weight"))
		.def(
			"add_infection",
			[](Virus &self, const py::object &from, const std::string &to,
			   const py::object &rate,
			   std::optional<std::string> transition_name) -> Virus & {
				name_transition(self.infection(stages_from_py(self, from),
											   stage_from_label(self, to),
											   value_from_py(rate)),
								transition_name);
				return self;
			},
			self_ref, doc::add_infection, py::arg("from_"), py::arg("to"),
			py::arg("rate"), py::arg("transition_name") = py::none())
		.def(
			"add_rate",
			[](Virus &self, const py::object &from, const std::string &to,
			   const py::object &rate, const std::string &effect,
			   std::optional<std::string> transition_name) -> Virus & {
				TransitionRef ref =
					self.rate(stages_from_py(self, from),
							  stage_from_label(self, to), value_from_py(rate));
				if (effect == "recovery")
					ref.effect(Effect::recovery);
				else if (effect == "death")
					ref.effect(Effect::death);
				else if (effect != "none")
					throw std::invalid_argument(
						"The effect is one of \"none\", \"recovery\" and "
						"\"death\".");
				name_transition(ref, transition_name);
				return self;
			},
			self_ref, doc::add_rate, py::arg("from_"), py::arg("to"),
			py::arg("rate"), py::arg("effect") = "none",
			py::arg("transition_name") = py::none())
		.def(
			"add_after",
			[](Virus &self, const std::string &from, const std::string &to,
			   Duration duration, std::optional<std::string> since,
			   std::optional<std::string> transition_name) -> Virus & {
				if (since)
					duration = duration.since(stage_from_label(self, *since));
				name_transition(self.after(stage_from_label(self, from),
										   stage_from_label(self, to),
										   duration),
								transition_name);
				return self;
			},
			self_ref, doc::add_after, py::arg("from_"), py::arg("to"),
			py::arg("duration"), py::arg("since") = py::none(),
			py::arg("transition_name") = py::none())
		.def(
			"add_external",
			[](Virus &self, const py::object &from, const std::string &to,
			   std::string transition_name) -> Virus & {
				self.external(stages_from_py(self, from),
							  stage_from_label(self, to),
							  std::move(transition_name));
				return self;
			},
			self_ref, doc::add_external, py::arg("from_"), py::arg("to"),
			py::arg("transition_name"))
		.def(
			"use_param",
			[](Virus &self, std::string param,
			   std::string replacement) -> Virus & {
				self.use_param(std::move(param), std::move(replacement));
				return self;
			},
			self_ref, doc::use_param, py::arg("param"), py::arg("replacement"))
		.def("get_stages", &get_stages, doc::get_stages)
		.def(
			"clone",
			[](const Virus &self) {
				// A new variant: the model gives it its own id when it is
				// added
				std::unique_ptr<Virus> v = self.clone_ptr();
				v->set_id(-99);
				return v;
			},
			doc::clone)
		.def("set_mutation", &Virus::set_mutation, doc::set_mutation,
			 py::arg("fun"))
		.def("print", &Virus::print, doc::print);
}

void epiworldpy::export_value(py::class_<Value> &c) {
	c.def(py::init([](const py::object &x) { return value_from_py(x); }),
		  doc::value_init, py::arg("x"))
		.def_static("inverse", &Value::inverse, doc::value_inverse,
					py::arg("param"))
		.def_static("complement", &Value::complement, doc::value_complement,
					py::arg("param"))
		.def_static("fun", &py_value_fun, doc::value_fun, py::arg("fun"));
}

void epiworldpy::export_duration(py::class_<Duration> &c) {
	c.def_static(
		 "fixed",
		 [](const py::object &days) {
			 return Duration::fixed(value_from_py(days));
		 },
		 doc::duration_fixed, py::arg("days"))
		.def_static(
			"geometric",
			[](const py::object &mean) {
				return Duration::geometric(value_from_py(mean));
			},
			doc::duration_geometric, py::arg("mean"))
		.def_static(
			"lognormal",
			[](const py::object &mean, const py::object &sd) {
				return Duration::lognormal(value_from_py(mean),
										   value_from_py(sd));
			},
			doc::duration_lognormal, py::arg("mean"), py::arg("sd"))
		.def_static(
			"gamma",
			[](const py::object &mean, const py::object &sd) {
				return Duration::gamma(value_from_py(mean), value_from_py(sd));
			},
			doc::duration_gamma, py::arg("mean"), py::arg("sd"));
}
