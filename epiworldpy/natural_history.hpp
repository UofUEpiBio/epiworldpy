#ifndef EPIWORLDPY_NATURAL_HISTORY_HPP
#define EPIWORLDPY_NATURAL_HISTORY_HPP

/* Conversions shared by the natural-history bindings: transition values,
 * placements (epiworldpy.assign), and stages named by their labels. They
 * mirror epiworldR's src/natural-history.h and src/assign.h. */

#include "common.hpp"

#include <functional>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace epiworldpy {

/* A placement as Python builds it (epiworldpy.assign): the form ("share",
 * "count", "shares", "counts", "group_shares", "group_counts", "agents",
 * "labels", "by") and what the form takes. Labels are kept as Python objects
 * (a stage name, a group number, or None) until the placement is used, when
 * the stage names are resolved against the virus or the model. */
struct PyAssign {
	std::string form;
	std::vector<double> values;
	std::vector<pybind11::object> labels;
	std::vector<size_t> ids;
	std::string layer;
	pybind11::object fun = pybind11::none();
};

using LabelOf = std::function<int(const std::string &)>;

/* A transition's value: a number, a model parameter (its name), or a Value
 * (Value.inverse(), Value.complement(), Value.fun()). */
inline epiworld::Value value_from_py(const pybind11::handle &x) {
	namespace py = pybind11;
	if (py::isinstance<epiworld::Value>(x))
		return x.cast<epiworld::Value>();
	if (py::isinstance<py::str>(x))
		return epiworld::Value(x.cast<std::string>());
	if (py::isinstance<py::float_>(x) ||
		(py::isinstance<py::int_>(x) && !py::isinstance<py::bool_>(x)))
		return epiworld::Value(x.cast<double>());
	throw py::type_error(
		"A value is a number, the name of a model parameter, or a Value "
		"(Value.inverse(), Value.complement() or Value.fun()).");
}

/* The natural history of `virus` (raises if it has no stages yet). */
inline const epiworld::NaturalHistory &virus_nh(const epiworld::Virus &virus) {
	const epiworld::NaturalHistory *nh = virus.get_natural_history();
	if (nh == nullptr)
		throw std::invalid_argument("The virus \"" + virus.get_name() +
									"\" has no stages yet: add them with "
									"add_stage().");
	return *nh;
}

/* The natural history of the model (its stages come from its first virus). */
inline const epiworld::NaturalHistory &model_nh(const epiworld::Model &model) {
	const epiworld::NaturalHistory *nh = model.get_natural_history();
	if (nh == nullptr)
		throw std::invalid_argument(
			"The model has no stages yet: add a virus with a natural history "
			"(Model.add_virus()).");
	return *nh;
}

/* The stage of `virus` labeled `label` (a ValueError naming the virus if
 * there is none) */
inline int stage_from_label(const epiworld::Virus &virus,
							const std::string &label) {
	const epiworld::NaturalHistory &nh = virus_nh(virus);
	try {
		return nh.stage_id(label);
	} catch (const std::logic_error &e) {
		throw std::invalid_argument(e.what());
	}
}

/* A stage or a list of stages, by label. */
inline epiworld::Nodes stages_from_py(const epiworld::Virus &virus,
									  const pybind11::handle &x) {
	namespace py = pybind11;
	std::vector<int> ids;
	if (py::isinstance<py::str>(x))
		ids.push_back(stage_from_label(virus, x.cast<std::string>()));
	else
		for (auto l : x.cast<std::vector<std::string>>())
			ids.push_back(stage_from_label(virus, l));
	return epiworld::Nodes(ids);
}

/* A placement's stage labels, resolved against a copy of the stages' labels.
 * The resolver is kept with the placement (assign.by() calls it at every
 * reset), so it owns what it reads: never the caller's virus, which may be
 * gone by then. */
inline LabelOf stage_resolver(const epiworld::NaturalHistory &nh) {
	return [name = nh.get_name(),
			labels = nh.get_stages()](const std::string &label) -> int {
		for (size_t s = 0u; s < labels.size(); ++s)
			if (labels[s] == label)
				return static_cast<int>(s);
		throw std::invalid_argument("The natural history \"" + name +
									"\" has no stage \"" + label + "\".");
	};
}

/* A label: a stage name (resolved by `label_of`), a whole number (a group),
 * or None (no label). */
inline int label_from_py(const pybind11::handle &x, const LabelOf &label_of) {
	namespace py = pybind11;
	if (x.is_none())
		return -1;
	if (py::isinstance<py::str>(x)) {
		if (!label_of)
			throw std::invalid_argument("This placement takes no stage names.");
		return label_of(x.cast<std::string>());
	}
	if (py::isinstance<py::int_>(x) && !py::isinstance<py::bool_>(x))
		return x.cast<int>();
	throw py::type_error("A label is a stage name, a whole number (a group), "
						 "or None (no label).");
}

inline epiworld::Assign assign_from_py(const PyAssign *a,
									   const LabelOf &label_of) {
	namespace py = pybind11;
	namespace assign = epiworld::assign;

	// No placement: the use's default (e.g., add_virus() without one)
	if (a == nullptr)
		return epiworld::Assign();

	const std::string &form = a->form;
	if (form == "share")
		return assign::share(a->values.at(0));
	if (form == "count")
		return assign::count(static_cast<size_t>(a->values.at(0)));
	if (form == "shares") {
		std::vector<std::pair<int, epiworld_double>> s;
		for (size_t i = 0; i < a->values.size(); ++i)
			s.emplace_back(label_from_py(a->labels[i], label_of), a->values[i]);
		return assign::shares(std::move(s));
	}
	if (form == "counts") {
		std::vector<std::pair<int, size_t>> c;
		for (size_t i = 0; i < a->values.size(); ++i)
			c.emplace_back(label_from_py(a->labels[i], label_of),
						   static_cast<size_t>(a->values[i]));
		return assign::counts(std::move(c));
	}
	if ((form == "group_shares") || (form == "group_counts")) {
		std::vector<epiworld_double> v(a->values.begin(), a->values.end());
		return (form == "group_shares") ? assign::group_shares(a->layer, v)
										: assign::group_counts(a->layer, v);
	}
	if (form == "agents")
		return assign::agents(a->ids);
	if (form == "labels") {
		std::vector<int> v(a->labels.size());
		for (size_t i = 0; i < v.size(); ++i)
			v[i] = label_from_py(a->labels[i], label_of);
		return assign::labels(std::move(v));
	}
	if (form == "by") {
		// Calls Python once per agent at each reset
		py::function fun = a->fun;
		return assign::by(
			[fun, label_of](epiworld::Agent &agent, epiworld::Model &model) {
				py::object res =
					fun(py::cast(&agent, py::return_value_policy::reference),
						py::cast(&model, py::return_value_policy::reference));
				return label_from_py(res, label_of);
			});
	}
	throw std::invalid_argument("Unknown placement \"" + form + "\".");
}

} // namespace epiworldpy

#endif /* EPIWORLDPY_NATURAL_HISTORY_HPP */
