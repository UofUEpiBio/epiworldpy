#include "assign.hpp"
#include "docstrings/assign.hpp"
#include "natural_history.hpp"

#include <pybind11/stl.h>

using namespace epiworldpy;
namespace py = pybind11;
namespace doc = epiworldpy::docstrings::assign;

static auto describe(const PyAssign &a) -> std::string {
	auto num = [](double x) {
		return py::str(py::float_(x)).cast<std::string>();
	};
	if (a.form == "share")
		return "a share (" + num(a.values[0]) + ") of the eligible agents";
	if (a.form == "count")
		return std::to_string(static_cast<size_t>(a.values[0])) +
			   " of the eligible agents";
	if ((a.form == "shares") || (a.form == "counts")) {
		std::string out = a.form + " by label:";
		for (size_t i = 0; i < a.values.size(); ++i)
			out += std::string(i ? ", " : " ") +
				   py::str(a.labels[i]).cast<std::string>() + " = " +
				   num(a.values[i]);
		return out;
	}
	if (a.form == "group_shares")
		return "a share of each group of the layer \"" + a.layer + "\"";
	if (a.form == "group_counts")
		return "a number of each group of the layer \"" + a.layer + "\"";
	if (a.form == "agents")
		return std::to_string(a.ids.size()) + " given agents";
	if (a.form == "labels")
		return "one label per agent (" + std::to_string(a.labels.size()) + ")";
	return "a label from a function of (agent, model)";
}

void epiworldpy::export_assign(py::module &m) {
	py::class_<PyAssign>(m, "Assign", doc::cls)
		.def(
			"get_form", [](const PyAssign &a) { return a.form; }, doc::get_form)
		.def("__repr__", [](const PyAssign &a) {
			return "<epiworldpy.assign.Assign: " + describe(a) + ">";
		});

	// The factory behind the epiworldpy.assign functions, which check the
	// arguments
	m.def(
		"_make",
		[](std::string form, std::vector<double> values,
		   std::vector<py::object> labels, std::vector<size_t> ids,
		   std::string layer, py::object fun) {
			return PyAssign{std::move(form),   std::move(values),
							std::move(labels), std::move(ids),
							std::move(layer),  std::move(fun)};
		},
		py::arg("form"), py::arg("values"), py::arg("labels"), py::arg("ids"),
		py::arg("layer"), py::arg("fun"));
}
